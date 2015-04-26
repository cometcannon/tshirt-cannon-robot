/*
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main(int argc, char** argv)
{
    struct termios tio;
    struct termios stdio;
    struct termios old_stdio;
    int tty_fd;

    unsigned char c='D';
    tcgetattr(STDOUT_FILENO,&old_stdio);

    printf("Please start with %s /dev/ttyS1 (for example)\n",argv[0]);
    memset(&stdio,0,sizeof(stdio));
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;
    tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    tty_fd=open(argv[1], O_RDWR | O_NONBLOCK);
    cfsetospeed(&tio,B115200);            // 115200 baud
    cfsetispeed(&tio,B115200);            // 115200 baud

    tcsetattr(tty_fd,TCSANOW,&tio);
    while (c!='q')
    {
        if (read(tty_fd,&c,1)>0)        write(STDOUT_FILENO,&c,1);              // if new data is available on the serial port, print it out
        if (read(STDIN_FILENO,&c,1)>0)  write(tty_fd,&c,1);                     // if new data is available on the console, send it to the serial port
    }

    close(tty_fd);
    tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 1

int main(int argc, char* argv[]) {

    int input_fd, output_fd;
    ssize_t ret_in, ret_out;
    char buffer[BUF_SIZE];
    char string[] = "Where is Gans?\n";

    if(argc != 3)
    {
        printf ("Usage: cp file1 file2");
        return 1;
    }

    input_fd = open (argv [1],  O_RDWR | O_NOCTTY | O_NDELAY);

    if (input_fd == -1)
    {
        perror ("open");
        return 2;
    }

    output_fd = open(argv[2], O_WRONLY | O_CREAT, 0644);

    if(output_fd == -1)
    {
        perror("open");
        return 3;
    }

    write(input_fd, &string, sizeof string);
    close (input_fd);
    input_fd = open (argv [1],  O_RDWR | O_NOCTTY | O_NDELAY);

    while((ret_in = read (input_fd, &buffer, BUF_SIZE)) > 0)
    {
        ret_out = write (output_fd, &buffer, (ssize_t) ret_in);
        write(output_fd, &buffer, (ssize_t) ret_in);

        if(ret_out != ret_in)
        {
            perror("write");
            return 4;
        }
    }

    close (input_fd);
    close (output_fd);

    return (EXIT_SUCCESS);
}

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

int main()
{

    char USB_name[] = "/dev/ttyATH0";
    int USB = open( USB_name, O_RDWR | O_NOCTTY );

    struct termios tty;
    struct termios tty_old;
    memset (&tty, 0, sizeof tty);

    if ( tcgetattr ( USB, &tty ) != 0 )
    {
        printf("Error: Cannot perform tcgetattr on USB\n");
        //std::cout << "Error " << errno << " from tcgetattr: "
            //<< strerror(errno) << std::endl;
    }

    tty_old = tty;

    printf("Output Baud: %lu\n", cfgetospeed(&tty));
    printf("Input Baud: %lu\n", cfgetispeed(&tty));

    //cfsetospeed (&tty, (speed_t)B9600);
    //cfsetispeed (&tty, (speed_t)B9600);

    //tty.c_cflag     &=  ~PARENB;            // Make 8n1
    //tty.c_cflag     &=  ~CSTOPB;
    //tty.c_cflag     &=  ~CSIZE;
    //tty.c_cflag     |=  CS8;

    //tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    //tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    //tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    //cfmakeraw(&tty);

    //tcflush( USB, TCIFLUSH );

    if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
        //std::cout << "Error " << errno << " from tcsetattr" << std::endl;
        printf("Error: Cannot perform tcsetattr on USB\n");

    int8_t cmd_size = 5;
    int8_t cmd[cmd_size];
    cmd[0] = 0x47;
    cmd[1] = 0x41;
    cmd[2] = 0x4e;
    cmd[3] = 0x53;

    cmd[4] = 8;

    int n_written = 0, spot = 0;

    n_written = write(USB, cmd, cmd_size);
    printf("%d of %d bytes written: %u, %u, %u, %u, %u\n", 
            n_written, cmd_size, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);

    int n = 0;
    char buf = '\0';

    char response[1024];
    memset(response, '\0', sizeof response);

    sleep(1);

    n = read( USB, &buf, 1 );

    if (n < 0)
        //std::cout << "Error reading: " << strerror(errno) << std::endl;
        printf("Error reading\n");

    else if (n == 0)
        //std::cout << "Read nothing!" << std::endl;
        printf("Read nothing!\n");

    else
        //std::cout << "Response: " << response << std::endl;
        printf("Response: %s\n", response);

    close(USB_name);
}*/

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <pthread.h>

int main()
{
    FILE *avr = fopen("/dev/ttyATH0", "a+");
    FILE *file = fopen("test.txt", "w");
    int8_t cmd[5];
    int8_t resp;

    cmd[0] = 0x47;
    cmd[1] = 0x41;
    cmd[2] = 0x4e;
    cmd[3] = 0x53;

    cmd[4] = 8;

    if(avr != NULL)
    {
        fputc(cmd[0], avr);
        fputc(cmd[1], avr);
        fputc(cmd[2], avr);
        fputc(cmd[3], avr);
        fputc(cmd[4], avr);
        fputc('\n', avr);

    }
    else
        printf("Couldn't open file\n");

    for(int i = 0; i < 100; i++)
    {
        resp = fgetc(avr);
        fputc(resp, file);
    }

    fclose(file);
    fclose(avr);
}
