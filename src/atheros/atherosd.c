#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define HALF_WIDTH	30
#define HALF_LENGTH	28
#define WHEEL_RADIUS 8
#define MAX_COMPUTED_VEL 330
#define MAX_COMMAND 127
#define MAX_BUFFER 1024

#define MAGIC 0x47414e53

#define LISTEN_PORT 12313

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
    char buffer[1000];
    char sock_name[] = "socket";
    int sockfd, atmegafd, sd_current;
    socklen_t addrlen;
    struct sockaddr_in sin;
    struct sockaddr_in pin;

    atmegafd = open("/dev/ttyATH0", O_RDWR | O_NOCTTY | O_NDELAY);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
        
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(LISTEN_PORT);
    
    if (bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    addrlen = sizeof(pin);
    if ((sd_current = accept(sockfd, (struct sockaddr *) &pin, &addrlen)) == -1) {
        perror("accept");
        exit(1);
    }

    if (recv(sd_current, buffer, sizeof(buffer), 0) < 0) {
        perror("recv");
        exit(1);
    }

    printf("got it\n", buffer);

    close(sd_current);
    close(sockfd);

    sleep(1);

    return 0;
}
