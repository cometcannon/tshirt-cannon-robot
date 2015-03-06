#include <stdio.h>
#include <stdint.h>
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
    printf("killed the robot\n");
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
    printf("fired the cannon\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void set_pressure(state_t *state, int8_t pressure)
{
     int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 8;
    buffer[5] = pressure;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 6);
    printf("set the pressure\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void command_velocity(state_t *state, int8_t v_x, int8_t v_y, int8_t w)
{
    int8_t buffer[MAX_BUFFER];

    struct vel_profile vels = inverse_kinematics(v_x, v_y, w);

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 3;

    buffer[5] = vels.for_left;
    buffer[6] = vels.for_right;
    buffer[7] = vels.rear_right;
    buffer[8] = vels.rear_left;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 9);
    printf("commanded a velocity\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

void command_motor(state_t *state, int motor, int8_t value)
{
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 2;

    buffer[5] = motor;
    buffer[6] = value;

    pthread_mutex_lock(&state->atmegafd_mutex);
    write(state->atmegafd, buffer, 7);
    printf("commanded a motor\n");
    pthread_mutex_unlock(&state->atmegafd_mutex);
}

int process_message(state_t *state, int8_t *msg, int len)
{
    if (len < 5) {
        printf("short message\n");
        return 0;
    }

    if (msg[0] != (MAGIC >> 24 & 0xff) ||
        msg[1] != (MAGIC >> 16 & 0xff) ||
        msg[2] != (MAGIC >>  8 & 0xff) ||
        msg[3] != (MAGIC >>  0 & 0xff)) {
        printf("bad magic\n");
        return 0; // the magic bytes are wrong -> ignore message
    }

    int8_t msg_type = msg[4];

    switch (msg_type) {
        case 0: // kill robot motors
            kill_robot(state);
            break;
        case 1: // kill robot motors and terminate this daemon
            kill_robot(state);
            return -1;
        case 2: // keep alive message
            break;
        case 3: // velocity command
            command_velocity(state, msg[5], msg[6], msg[7]);
            break;
        case 4: // individual motor command
            command_motor(state, msg[5], msg[6]);
            break;
        case 5:
            fire_cannon(state);
            break;
        case 8:
            set_pressure(state, msg[5]);
            break;
    }

    return 0;
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

void *pulse_monitor(void *arg)
{
    state_t *state = (state_t *) arg;
    int kill = 0;
    
    while (1) {
        int64_t now = utime_now();
        
        pthread_mutex_lock(&state->last_utime_mutex);

        if (state->last_utime > 0 && now - state->last_utime > UTIMEOUT) {
            printf("timeout exceeded\n");
            kill_robot(state);
        }

        pthread_mutex_unlock(&state->last_utime_mutex);

        usleep(10);
    }
}

void *atmega_keep_alive(void *arg)
{
    state_t *state = (state_t *) arg;
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >>  8 & 0xff;
    buffer[3] = MAGIC >>  0 & 0xff;

    buffer[4] = 1;

    while (1) {
        pthread_mutex_lock(&state->atmegafd_mutex);
        write(state->atmegafd, buffer, 5);
        pthread_mutex_unlock(&state->atmegafd_mutex);

        usleep(ATMEGA_INTERVAL);
    }
}

int main(int argc, char *argv[])
{
    int sd, atmegafd, sd_current;
    int8_t buffer[MAX_PACKET];
    pthread_t pulse_thread;
    pthread_t atmega_thread;
    state_t *state = calloc(1, sizeof(state_t));
    char *comp = "from-comp";
    char *atmega = "to-atmega";
    char *help = "help";
    
    if (argc > 1 && !strcmp(argv[1], help)) {
        printf("usage: %s from-comp to-atmega\n\nBy default, neither thread is initialized. Add \"from-comp\" and/or \"to-atmega\" to enable the threads.", argv[0]);
        exit(0);
    }
    
    state->atmegafd = open("/dev/ttyATH0", O_RDWR | O_NOCTTY | O_NDELAY);

    network_connect(state);

    pthread_mutex_init(&state->atmegafd_mutex, NULL);
    pthread_mutex_init(&state->clientfd_mutex, NULL);
    pthread_mutex_init(&state->sockfd_mutex, NULL);
    pthread_mutex_init(&state->last_utime_mutex, NULL);
    
    if ((argc > 1 && !strcmp(argv[1], comp)) || (argc > 2 && !strcmp(argv[2], comp))) {
        printf("starting from-comp thread...\n");
        pthread_create(&pulse_thread, NULL, pulse_monitor, state);
    }
    
    if ((argc > 1 && !strcmp(argv[1], atmega)) || (argc > 2 && !strcmp(argv[2], atmega))) {
        printf("starting to-atmega thread...\n");
        pthread_create(&atmega_thread, NULL, atmega_keep_alive, state);
    }
       
    while (1) {
        int len;
        
        if ((len = recv(state->sockfd, buffer, sizeof(buffer), 0)) < 0) {
            perror("recv");
            break;
        }
        
        pthread_mutex_lock(&state->last_utime_mutex);
        state->last_utime = utime_now();
        pthread_mutex_unlock(&state->last_utime_mutex);

        if (process_message(state, buffer, len) < 0)
            break;
    }

    close(state->sockfd);
    close(state->clientfd);
    close(state->atmegafd);

    free(state);

    return 0;
}
