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
#define MAX_COMMAND_SIZE 10
#define MAX_DEBUG_STRING_SIZE 1024

#define LISTEN_PORT 12313
#define MAX_PACKET 1500

#define UTIMEOUT 300e3
#define ATMEGA_INTERVAL 300e3

bool debug = false;
int8_t const magic_bytes[] = {0x47, 0x41, 0x4e, 0x53};

typedef struct state state_t;
struct state {
    int64_t last_utime;
    pthread_mutex_t last_utime_mutex;

    FILE *atmegafp;
    pthread_mutex_t atmegafp_mutex;

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

struct command {
    int8_t cmd_size;
    int8_t cmd[MAX_COMMAND_SIZE];
    char debug_msg[MAX_DEBUG_STRING_SIZE];
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

void send_command(state_t *state, struct command cmd)
{
    pthread_mutex_lock(&state->atmegafp_mutex);

    for(int8_t i = 0; i < (int8_t)(sizeof magic_bytes); i++)
        fputc(magic_bytes[i], state->atmegafp);

    for(int8_t i = 0; i < cmd.cmd_size; i++)
        fputc(cmd.cmd[i], state->atmegafp);

    pthread_mutex_unlock(&state->atmegafp_mutex);
}

struct command gen_kill_robot_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 0;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Killed the robot\n", cmd.cmd[0]);

    return cmd;
}

struct command gen_kill_motors_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 1;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Killed the motors\n", cmd.cmd[0]);

    return cmd;
}

struct command gen_honk_horn_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 2;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Honked Horn\n", cmd.cmd[0]);

    return cmd;
}

struct command gen_motor_cmd(int8_t motor, int8_t value, bool control_ang_vels)
{
    struct command cmd;

    control_ang_vels ? (cmd.cmd[0] = 6) : (cmd.cmd[0] = 2);
    cmd.cmd[1] = motor;
    cmd.cmd[2] = value;

    cmd.cmd_size = 3;

    sprintf(cmd.debug_msg, "[atherosd] {%d} M%d: %4d, Control Angular Velocity: %s\n", cmd.cmd[0], motor, value,
            control_ang_vels ? "TRUE" : "FALSE");

    return cmd;
}

struct command gen_velocity_cmd(int8_t v_x, int8_t v_y, int8_t w, bool control_ang_vels)
{
    struct vel_profile vels = inverse_kinematics(v_x, v_y, w);
    struct command cmd;

    control_ang_vels ? (cmd.cmd[0] = 7) : (cmd.cmd[0] = 3);
    cmd.cmd[1] = vels.for_left;
    cmd.cmd[2] = vels.for_right;
    cmd.cmd[3] = vels.rear_right;
    cmd.cmd[4] = vels.rear_left;

    cmd.cmd_size = 5;

    sprintf(cmd.debug_msg, "[atherosd] {%d} M1: %4d, M2: %4d, M3: %4d, M4: %4d, Control Angular Velocity: %s\n", cmd.cmd[0],
            cmd.cmd[1], cmd.cmd[2], cmd.cmd[3], cmd.cmd[4], control_ang_vels ? "TRUE" : "FALSE");

    return cmd;
}

struct command gen_send_motor_velocities_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 4;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Sending Motor Angular Velocities\n", cmd.cmd[0]);

    return cmd;
}

struct command gen_fire_cannon_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 5;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Fired the cannon\n", cmd.cmd[0]);

    return cmd;
}

struct command gen_increase_pressure_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 8;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Increasing pressure\n", cmd.cmd[0]);

    return cmd;
}

struct command gen_send_pressure_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 9;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Sending pressure\n", cmd.cmd[0]);

    return cmd;
}

struct command gen_debug_cmd()
{
    struct command cmd;

    cmd.cmd[0] = 10;
    cmd.cmd_size = 1;
    sprintf(cmd.debug_msg, "[atherosd] {%d} Commanded debug mode for AVR chip. Expect verbose output on /dev/ttyATH0 from the AVR chip.\n", cmd.cmd[0]);

    return cmd;
}

void process_message(state_t *state, int8_t *msg, int len)
{
    if (len < 5) {
        printf("[atherosd] Short message\n");
        return;
    }

    if (msg[0] != magic_bytes[0] ||
        msg[1] != magic_bytes[1] ||
        msg[2] != magic_bytes[2] ||
        msg[3] != magic_bytes[3]) {
        printf("[atherosd] Bad magic bytes\n");
        return;
    }

    int8_t msg_type = msg[4];
    struct command cmd;

    switch (msg_type) {
        case 0:
            cmd = gen_kill_robot_cmd();
            break;

        case 1:
            cmd = gen_kill_motors_cmd();
            break;

        case 2:
            //cmd = gen_motor_cmd(msg[5], msg[6], false);
            cmd = gen_honk_horn_cmd();
            break;

        case 3:
            cmd = gen_velocity_cmd(msg[5], msg[6], msg[7], false);
            break;

        case 4:
            cmd = gen_send_motor_velocities_cmd();
            break;

        case 5:
            cmd = gen_fire_cannon_cmd();
            break;

        case 6:
            cmd = gen_motor_cmd(msg[5], msg[6], true);
            break;

        case 7:
            cmd = gen_velocity_cmd(msg[5], msg[6], msg[7], true);
            break;

        case 8:
            cmd = gen_increase_pressure_cmd();
            break;

        case 9:
            cmd = gen_send_pressure_cmd();
            break;

        case 10:
            cmd = gen_debug_cmd();
            break;

        case 11:
            debug = !debug;
            if(debug)
                printf("[atherosd] Turning on Debug Mode. Verbose output will ensue.\n");
            else
                printf("[atherosd] Turning off Debug Mode.\n");
            return;

    }

    if(debug)
        printf("%s", cmd.debug_msg);

    send_command(state, cmd);
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

    uint8_t messageLengthArray[] = {9, 6};

    while(1)
    {
        int8_t data = fgetc(state->atmegafp);

        messageBuffer[messageBufferIndex] = data;

        if(messageBufferIndex <= 3 && messageBuffer[messageBufferIndex] != magic_bytes[messageBufferIndex])
        {
            memset(messageBuffer, 0, messageBufferIndex + 1);
            messageBufferIndex = 0;
        }

        else if(messageLengthArray[ messageBuffer[4] ] <= messageBufferIndex + 1)
        {
            pthread_mutex_lock(&state->sockfd_mutex);
            uint8_t num_bytes_written = write(state->sockfd, messageBuffer, messageBufferIndex + 1);
            pthread_mutex_unlock(&state->sockfd_mutex);

            if(debug)
                printf("Writing %d bytes to sockdf\n", num_bytes_written);

            memset(messageBuffer, 0, messageBufferIndex + 1);
            messageBufferIndex = 0;
        }

        else
            messageBufferIndex++;
    }
}

int main()
{
    int8_t buffer[MAX_PACKET];
    pthread_t serial_monitor_thread;
    state_t *state = calloc(1, sizeof(state_t));

    state->atmegafp = fopen("/dev/ttyATH0", "a+");

    pthread_mutex_init(&state->atmegafp_mutex, NULL);
    pthread_mutex_init(&state->clientfd_mutex, NULL);
    pthread_mutex_init(&state->sockfd_mutex, NULL);
    pthread_mutex_init(&state->last_utime_mutex, NULL);

    //pthread_create(&serial_monitor_thread, NULL, serial_monitor, state);

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
