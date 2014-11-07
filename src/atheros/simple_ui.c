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

void command_lin_vel(int8_t x, int8_t y)
{

}

void command_ang_vel()
{

}

int main(int argc, char *argv[])
{
    char input[100];
    char command = 0;
    int magnitude, x, y, direction;

    while (command != 'e') {
        printf("-> ");
        scanf("%s", input);
        command = input[0];

        switch (command) {
            case 'f':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);
                break;
            case 'b':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);
                break;
            case 'r':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);
                break;
            case 'l':
                printf("enter magnitude (0-127)\n--> ");
                scanf("%d", &magnitude);
                break;
            case 'v':
                printf("enter x (-128 to 127)\n--> ");
                scanf("%d", &x);
                printf("enter y (-128 to 127)\n--> ");
                scanf("%d", &y);
                break;
            case 't':
                printf("(l)eft or (r)ight:\n--> ");
                scanf("%s", input);
                command = input[0];

                if (command == 'l')
                    direction = 0;
                else
                    direction = 1;

                printf("enter magnitude (0-255)\n--> ");
                scanf("%d", &magnitude);

                break;
            case 'k':
                break;
            case 'e':
                break;
            default:
                print_help();
        }
    }

    return 0;
}
