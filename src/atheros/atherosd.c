#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#define HALF_WIDTH	30
#define HALF_LENGTH	28
#define WHEEL_RADIUS 8
#define MAX_COMPUTED_VEL 330
#define MAX_COMMAND 127
#define MAX_BUFFER 1024

#define MAGIC 0x47414e53

struct vel_profile {
    int8_t for_left;
    int8_t for_right;
    int8_t rear_left;
    int8_t rear_right;
};

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

int process_message(int atmegafd, int8_t *msg)
{
    int8_t msg_type = msg[0];
    struct vel_profile vels;
    int8_t buffer[MAX_BUFFER];

    buffer[0] = MAGIC >> 24 & 0xff;
    buffer[1] = MAGIC >> 16 & 0xff;
    buffer[2] = MAGIC >> 8 & 0xff;
    buffer[3] = MAGIC >> 0 & 0xff;

    switch (msg_type) {
        case 0:
            buffer[4] = 0;
            write(atmegafd, buffer, 5);

            return -1;
        case 1:
            vels = inverse_kinematics(msg[1], msg[2], msg[3]);

            buffer[4] = 2;
            buffer[5] = vels.for_left;
            buffer[6] = vels.for_right;
            buffer[7] = vels.rear_right;
            buffer[8] = vels.rear_left;
            write(atmegafd, buffer, 9);

            break;
        case 2:
            buffer[4] = 1;
            buffer[5] = msg[1];
            buffer[6] = msg[2];
            write(atmegafd, buffer, 7);

            break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char sock_name[] = "socket";
    int sockfd, atmegafd;
    struct sockaddr_un name;

    atmegafd = open("/dev/ttyATH0", O_RDWR | O_NOCTTY | O_NDELAY);

    sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, sock_name);
    bind(sockfd, &name, SUN_LEN(&name));
    listen(sockfd, 5);

    struct sockaddr_un client_name;
    socklen_t client_name_len;
    int client_sockfd;

    client_sockfd = accept(sockfd, &client_name, &client_name_len);

    while (1) {
        int len;
        int8_t buffer[100];

        if (read(client_sockfd, buffer, 100) < 0)
            return 1;

        if (process_message(atmegafd, buffer) < 0)
            break;
    }

    close(client_sockfd);
    close(sockfd);
    unlink(sock_name);

    return 0;
}
