#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define HALF_WIDTH	1
#define HALF_LENGTH	1
#define MAX_COMPUTED_VEL (4*127)
#define MAX_COMMAND 127

struct vel_profile {
    int8_t for_left;
    int8_t for_right;
    int8_t rev_left;
    int8_t rev_right;
};

struct vel_profile inverse_kinematics(int8_t v_x, int8_t v_y, int8_t w)
{
    struct vel_profile vels;
    int velocity;

    velocity = v_x - v_y - (HALF_WIDTH + HALF_LENGTH) * w;
    vels.for_left = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    velocity = v_x + v_y + (HALF_WIDTH + HALF_LENGTH) * w;
    vels.for_right = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    velocity = v_x + v_y - (HALF_WIDTH + HALF_LENGTH) * w;
    vels.rev_right = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    velocity = v_x - v_y + (HALF_WIDTH + HALF_LENGTH) * w;
    vels.rev_left = MAX_COMMAND * velocity / MAX_COMPUTED_VEL;

    return vels;
}

int process_message(int atmegafd, int8_t *msg)
{
    int8_t msg_type = msg[0];
    struct vel_profile vels;
    int8_t *buffer;

    switch (msg_type) {
        case 0:
            buffer = calloc(1, sizeof msg_type);
            buffer[0] = 0;
            write(atmegafd, buffer, 1);
            free(buffer);

            return -1;
        case 1:
            vels = inverse_kinematics(msg[1], msg[2], msg[3]);

            buffer = calloc(5, sizeof msg_type);
            buffer[0] = 2;
            buffer[1] = vels.for_left;
            buffer[2] = vels.for_right;
            buffer[3] = vels.rev_right;
            buffer[4] = vels.rev_left;
            write(atmegafd, buffer, 5);
            free(buffer);

            break;
        case 2:
            buffer = calloc(3, sizeof msg_type);
            buffer[0] = 1;
            buffer[1] = msg[1];
            buffer[2] = msg[2];
            write(atmegafd, buffer, 3);
            free(buffer);

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
        char buffer[100];

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
