#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define HALF_WIDTH	30
#define HALF_LENGTH	28
#define WHEEL_RADIUS 8
#define MAX_COMPUTED_VEL 330
#define MAX_COMMAND 127
#define MAX_BUFFER 1024

#define MAGIC 0x47414e53

#define LISTEN_PORT 12313
#define MAX_PACKET 1500

#define UTIMEOUT 300e3
#define ATMEGA_INTERVAL 300e3

bool debug = false;

typedef struct state state_t;
struct state {
    int64_t last_utime;
    pthread_mutex_t last_utime_mutex;

    int atmegafd;
    pthread_mutex_t atmegafd_mutex;

    int clientfd;
    pthread_mutex_t clientfd_mutex;

    int sockfd;
    pthread_mutex_t sockfd_mutex;
};

struct vel_profile {
    int8_t for_left;
    int8_t for_right;
    int8_t rear_left;
    int8_t rear_right;
};

int64_t utime_now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t) tv.tv_usec * 1e6 + tv.tv_usec;
}

// v_x, v_y: measured in cm/s
// w: measured in rad/(100*s) - hundredths of a radian per second
struct vel_profile inverse_kinematics(int8_t v_x, int8_t v_y, int8_t w)
{
    struct vel_profile vels;
    int velocity; // measured in cm/s

    velocity = v_x - v_y - (HALF_WIDTH + HALF_LENGTH) * w / 100;
    vels.for_left = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    velocity = v_x + v_y + (HALF_WIDTH + HALF_LENGTH) * w / 100;
    vels.for_right = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    velocity = v_x - v_y + (HALF_WIDTH + HALF_LENGTH) * w / 100;
    vels.rear_right = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    velocity = v_x + v_y - (HALF_WIDTH + HALF_LENGTH) * w / 100;
    vels.rear_left = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    return vels;
}

void kill_robot(state_t *state)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 0;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 5);
    if(debug)
        printf("[atherosd] Killed the robot\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void kill_motors(state_t *state)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 1;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 5);
    if(debug)
        printf("[atherosd] Killed the motors\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void command_motor(state_t *state, int motor, int8_t value, bool control_ang_vels)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    if(!control_ang_vels)
        buffer[4] = 2;
    else
        buffer[4] = 6;

    buffer[5] = motor;
    buffer[6] = value;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 7);

    if(debug){
        printf("[atherosd] M%d: %4d, Control Angular Velocity: ", motor, value);
        if(control_ang_vels)
            printf("TRUE\n");
        else
            printf("FALSE\n");
    }

    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void command_velocity(state_t *state, int8_t v_x, int8_t v_y, int8_t w, bool control_ang_vels)
{
    int8_t buffer[MAX_BUFFER];

    struct vel_profile vels = inverse_kinematics(v_x, v_y, w);

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    if(!control_ang_vels)
        buffer[4] = 3;
    else
        buffer[4] = 7;

    buffer[5] = vels.for_left;
    buffer[6] = vels.for_right;
    buffer[7] = vels.rear_right;
    buffer[8] = vels.rear_left;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 9);

    if(debug){
        printf("[atherosd] M1: %4d, M2: %4d, M3: %4d, M4: %4d, Control Angular Velocity: ", buffer[5], buffer[6], buffer[7], buffer[8]);
        if(control_ang_vels)
            printf("TRUE\n");
        else
            printf("FALSE\n");
    }

    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void send_motor_ang_vels(state_t *state)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 4;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 5);
    if(debug)
        printf("[atherosd] Sending Motor Angular Velocities\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void fire_cannon(state_t *state)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 5;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 5);
    if(debug)
        printf("[atherosd] Fired the cannon\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void increase_pressure(state_t *state)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 8;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 5);
    if(debug)
        printf("[atherosd] Increasing pressure\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void send_pressure(state_t *state)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 9;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 5);
    if(debug)
        printf("[atherosd] Sending pressure\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void command_debug(state_t *state)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[5] = 10;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 6);
    if(debug)
        printf("[atherosd] Commanded debug mode for AVR chip. Expect verbose output on /dev/ttyATH0 from the AVR chip.\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void process_message(state_t *state, int8_t *msg, int len)
{
    if (len < 5) {
        printf("[atherosd] Short message\n");
        return;
    }

    if (msg[0] != (MAGIC >> 24 & 0xff) ||
        msg[1] != (MAGIC >> 16 & 0xff) ||
        msg[2] != (MAGIC >>  8 & 0xff) ||
        msg[3] != (MAGIC >>  0 & 0xff)) {
        printf("[atherosd] Bad magic bytes\n");
        return;
    }

    int8_t msg_type = msg[4];

    switch (msg_type) {
        case 0:
            kill_robot(state);
            break;

        case 1:
            kill_motors(state);
            break;

        case 2:
            command_motor(state, msg[5], msg[6], false);
            break;

        case 3:
            command_velocity(state, msg[5], msg[6], msg[7], false);
            break;

        case 4:
            send_motor_ang_vels(state);
            break;

        case 5:
            fire_cannon(state);
            break;

        case 6:
            command_motor(state, msg[5], msg[6], true);
            break;

        case 7:
            command_velocity(state, msg[5], msg[6], msg[7], true);
            break;

        case 8:
            increase_pressure(state);
            break;

        case 9:
            send_pressure(state);
            break;

        case 10:
            command_debug(state);
            break;

        case 11:
            debug = !debug;
            if(debug)
                printf("[atherosd] Turning on Debug Mode. Verbose output will ensue.\n");
            else
                printf("[atherosd] Turning off Debug Mode.\n");
            break;

    }
}

void network_connect(state_t *state)
{
    struct sockaddr_in sin;
    struct sockaddr_in pin;
    socklen_t addrlen;
    int orig_sock, new_sock;

    if ((orig_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(LISTEN_PORT);

    if (bind(orig_sock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(orig_sock, 5) < 0) {
        perror("listen");
        exit(1);
    }

    addrlen = sizeof(pin);
    if ((new_sock = accept(orig_sock, (struct sockaddr *) &pin, &addrlen)) == -1) {
        perror("accept");
        exit(1);
    }

    pthread_mutex_lock(&state->clientfd_mutex);
    state->clientfd = orig_sock;
    pthread_mutex_unlock(&state->clientfd_mutex);

    pthread_mutex_lock(&state->sockfd_mutex);
    state->sockfd = new_sock;
    pthread_mutex_unlock(&state->sockfd_mutex);
}

void *serial_monitor(void *arg)
{
    state_t *state = (state_t *) arg;

    uint8_t messageBuffer[64];
    uint8_t messageBufferIndex = 0;

    uint8_t magicBytes[] = {0x47, 0x41, 0x4e, 0x53};
    uint8_t messageLengthArray[] = {9, 6};

    FILE *fp = fdopen(state->atmegafd, "r");

    while(1)
    {
        pthread_mutex_lock(&state->atmegafd_mutex);
        int data = getc(fp);
        pthread_mutex_unlock(&state->atmegafd_mutex);

        if(data != EOF)
        {
            messageBuffer[messageBufferIndex] = data;

            if(messageBufferIndex <= 3 && messageBuffer[messageBufferIndex] != magicBytes[messageBufferIndex])
            {
                memset(messageBuffer, 0, messageBufferIndex + 1);
                messageBufferIndex = 0;
            }

            else if(messageLengthArray[ messageBuffer[4] ] <= messageBufferIndex + 1)
            {
                pthread_mutex_lock(&state->sockfd_mutex);
                write(state->sockfd, messageBuffer, messageBufferIndex + 1);
                pthread_mutex_unlock(&state->sockfd_mutex);

                memset(messageBuffer, 0, messageBufferIndex + 1);
                messageBufferIndex = 0;
            }

            else
                messageBufferIndex++;
        }

        usleep(10e3);
    }
}

int main()
{
    int sd, atmegafd, sd_current;
    int8_t buffer[MAX_PACKET];
    pthread_t serial_monitor_thread;
    state_t *state = calloc(1, sizeof(state_t));

    state->atmegafd = open("/dev/ttyATH0", O_RDWR | O_NOCTTY | O_NDELAY);

    pthread_mutex_init(&state->atmegafd_mutex, NULL);
    pthread_mutex_init(&state->clientfd_mutex, NULL);
    pthread_mutex_init(&state->sockfd_mutex, NULL);
    pthread_mutex_init(&state->last_utime_mutex, NULL);

    pthread_create(&serial_monitor_thread, NULL, serial_monitor, state);

    while(1){
        network_connect(state);

        while (1) {
            int len;

            if ((len = recv(state->sockfd, buffer, sizeof(buffer), 0)) <= 0) {
                if(debug){
                    perror("recv");
                    printf("[atherosd] Connection Broken\n");
                }
                break;
            }

            pthread_mutex_lock(&state->last_utime_mutex);
            state->last_utime = utime_now();
            pthread_mutex_unlock(&state->last_utime_mutex);

            process_message(state, buffer, len);
        }

        close(state->sockfd);
        close(state->clientfd);

        sleep(1);
        if(debug)
            printf("[atherosd] Restarting Connection\n");
    }

    return 0;
}
