#include <stdio.h>
#include <stdint.h>
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

int main(int argc, char *argv[])
{
    char sock_name[] = "socket";
    int sockfd;
    struct sockaddr_un name;

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

        struct vel_profile vels = inverse_kinematics(buffer[1], buffer[2], buffer[3]);
        printf("forward left: %d\n", vels.for_left);
        printf("forward right: %d\n", vels.for_right);
        printf("reverse right: %d\n", vels.rev_right);
        printf("reverse left: %d\n", vels.rev_left);

        if (!vels.for_left && !vels.for_right && !vels.rev_left && !vels.rev_right)
            break;
    }

    close(client_sockfd);
    close(sockfd);
    unlink(sock_name);

    return 0;
}
