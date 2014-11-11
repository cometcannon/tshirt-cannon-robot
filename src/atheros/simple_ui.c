#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

void print_help()
{
    printf("available commands:\n\n");
    printf("(f)orward\n");
    printf("(b)ackward\n");
    printf("(l)eft\n");
    printf("(r)ight\n");
    printf("(v)ector\n");
    printf("(t)urn\n");
    printf("(k)ill\n");
    printf("(e)xit\n\n");
}

void command_velocity(int sockfd, int8_t x, int8_t y, int8_t w)
{
    int8_t buffer[4];

    buffer[0] = 0;
    buffer[1] = x;
    buffer[2] = y;
    buffer[3] = w;

    write(sockfd, buffer, 4);
}

int main(int argc, char *argv[])
{
    char input[100];
    char command = 0;
    char sock_name = "socket";
    int sockfd;
    struct sockaddr_un name;
    int magnitude;
    int x;
    int y;
    int w;
    int direction;

    sockfd = socket(PF_LOCAL, SOCK_STREAM, 0);
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, socket_name);
    connect(sockfd, &name, SUN_LEN(&name));

    while (command != 'e') {
        printf("-> ");
        scanf("%s", input);
        command = input[0];

        switch (command) {
            case 'f':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);

                command_velocity(magnitude, 0, 0);
                break;
            case 'b':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);

                command_velocity(-1 * magnitude, 0, 0);
                break;
            case 'r':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);

                command_velocity(0, -1 * magnitude, 0);
                break;
            case 'l':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);

                command_velocity(0, magnitude, 0);
                break;
            case 'v':
                printf("enter x (-128 to 127)\n--> ");
                scanf("%d", &x);
                printf("enter y (-128 to 127)\n--> ");
                scanf("%d", &y);
                printf("enter w (-128 to 127)\n--> ");
                scanf("%d", &w);

                command_velocity(x, y, w);
                break;
            case 't':
                printf("(l)eft or (r)ight:\n--> ");
                scanf("%s", input);
                command = input[0];

                if (command == 'l')
                    direction = 0;
                else
                    direction = 1;

                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);

                w = direction ? -1 * magnitude : magnitude;
                command_velocity(0, 0, w);
                break;
            case 'k':
                command_velocity(0, 0, 0);
                break;
            case 'e':
                break;
            default:
                print_help();
        }
    }

    close(sockfd);

    return 0;
}
