#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include "unicode_subs.h"

#define EUS_CODE_STARTFRAME (0x81) // Start byte. Followed by, x-width (1 byte), y-width (1 byte)
#define EUS_CODE_ENDFRAME   (0x82) // End of image; next byte should be EUS_CODE_STARTFRAME again


enum {
    idle, wait_x_width, wait_y_width, data, frame_full, error
} rx_state;

int print_dark_border(int x, int y);
int print_light_block(int x, int y);
int print_center_dot(int x, int y);
int print_big_dot(int x, int y);
int print_ASCII(int x, int y, const char c);
int print_string(int x, int y, const char* str);
void make_frame_border(int x_size, int y_size);

void print_help(void);
int open_uart(const char *device);

int main(int argc, char *argv[])
{
    // SETUP / OPEN UART

    uint8_t buf;
    
    // opening / setting file descriptors

    if (argc<2)
	{
		fprintf(stderr, "ERROR: Missing arguments.\n");
        print_help();
		return -1;
	}

    if ( (strcmp(argv[1],"--help")==0) || (strcmp(argv[1],"-h")==0) )
    {
        print_help();
        return 0;
    }

    if (argv[1][0]=='-') {
		fprintf(stderr, "ERROR: Unknown argument.\n");
        print_help();
		return -1;
    }


    int fd_UL = open_uart(argv[1]);




    // SETUP NCURSES
    int x_size, y_size, x_count, y_count, count, maxcount;

    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    clear();

    mvaddstr(1, 1, "Waiting for screen data stream...\n");
    mvaddstr(3, 1, "     (end with Ctrl-C)");
    refresh();
    sleep(2);

    // SETUP TIME / FRAME RATE
    struct timespec ts, old_ts;
    old_ts.tv_sec = 0;
    old_ts.tv_sec = 0;
    struct tm *tm_info;

    // RX loop

    while(1) {

        // blocking 1 byte read
        while(read(fd_UL, &buf, 1) != 1);

        if (buf==EUS_CODE_STARTFRAME) {
            rx_state = wait_x_width;
            x_count = 0;
            y_count = 0;
            count = 0;
            continue;
        }


        if (buf==EUS_CODE_ENDFRAME) {
            rx_state = idle;
            refresh();
            continue;
        }

        switch(rx_state) {

            int i;

            case wait_x_width:
                x_size = buf;
                if (x_size==0)
                    rx_state = error;
                else
                    rx_state = wait_y_width;
                break;

            case wait_y_width:
                y_size = buf;
                if (y_size==0)
                    rx_state = error;
                else {
                    clear();
                    make_frame_border(x_size, y_size);
                    rx_state = data;
                    break;
                }

            case data:
                for (i=0; i<uc_subs_elements; i++) { // find Unicode substitutions
                    if (buf==unicode_subs[i].ASCII) {
                        mvaddstr(y_count+1, x_count+1, unicode_subs[i].UTF8);
                        break;
                    }
                }
                if (i==uc_subs_elements) // no Unicode found
                    print_ASCII(x_count,y_count,buf);

                x_count++;
                if (x_count==x_size) {
                    x_count = 0;
                    y_count++;
                    if (y_count==y_size) {
                        rx_state = frame_full;

                        clock_gettime(CLOCK_REALTIME, &ts);
                        tm_info = localtime(&ts.tv_sec);
                        long msecs = ts.tv_nsec / 1000000;
                        char timebuf[64] = "";
                        // if (ts.tv_sec != old_ts.tv_sec) {
                        //     long timediff = (ts.tv_nsec - old_ts.tv_nsec)
                        //     + (ts.tv_sec - old_ts.tv_sec)*1000000000;
                        //     double framerate = 1000000000.0 / timediff;
                        //     snprintf(timebuf, 64, "Current frame received @ %02d:%02d:%02d,%03ld"
                        //         "  - %5.1f frames/s \n",
                        //         tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, msecs, framerate);
                        // }
                        // else
                        snprintf(timebuf, 64, "Current frame received @ %02d:%02d:%02d,%03ld\n",
                                    tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, msecs);
                        old_ts = ts;
                        mvaddstr(y_size+2, 0, timebuf);
                    }
                }
                break;
     
            default:
        }
    }

    return 0;
}



int print_dark_border(int x, int y) {

        mvaddstr(y+1, x+1, "\u2592");
        return 0; // done

    return -1; // OUT_OF_FRAME
}

int print_light_block(int x, int y) {
        mvaddstr(y+1, x+1, "\u2588");
        return 0; // done
}

int print_center_dot(int x, int y) {
    mvaddstr(y+1, x+1, "\u00b7");
    return 0; // done
}

int print_big_dot(int x, int y) {
    mvaddstr(y+1, x+1, "\u2022");
    return 0; // done
}

int print_ASCII(int x, int y, const char c) {
        mvaddch(y+1, x+1, c);
        return 0; // done
}

int print_string(int x, int y, const char* str) {

    int i = 0;
    int retval = 0;

    while(str[i]!='\0' && retval==0)
        retval = print_ASCII(x+i,y, str[i++]);
    
    return retval;
}


void make_frame_border(int x_size, int y_size)
{
    // top line
    for(int x=-1; x<=x_size; x++)
        print_dark_border(x,-1);

    // bottom line
    for(int x=-1; x<=x_size; x++)
        print_dark_border(x,y_size);

    // left line
    for(int y=0; y<y_size; y++)
        print_dark_border(-1,y);

    // right_line
    for(int y=0; y<y_size; y++)
        print_dark_border(x_size,y);
}


void print_help(void)
{   printf("\U0001f609 \u2022 \n");
    printf("\n  Usage:\n\n");
    printf("  ./UARTscreen.elf  DEVICE      Open UART interface DEVICE,\n");
    printf("                                for example /dev/ttyUSB0\n");
    printf("  ./UARTscreen.elf  --help      Print this message\n\n");


}


int open_uart(const char *device)
{
    struct termios tio; 
	int fd = open(device, O_RDONLY | O_NOCTTY );

	if (fd == -1)
	{
		fprintf(stderr, "ERROR: Unable to open UART device '%s'\n", device);
        print_help();
		exit(-1);
	}

    // close once to be able to cleanly re-open.
    close(fd);
    
    fd = open(device, O_RDONLY | O_NOCTTY | O_NDELAY);

	if (fd == -1)
	{
		fprintf(stderr, "ERROR: Unable to open UART device '%s'\n", device);
        print_help();
		exit(-1);
	}
    else
    {
        printf("\n Opened UART %s \n\n", device);
        sleep(1);
    }

    // blocking for now
    fcntl(fd, F_SETFL, 0);

    // set 230400 Baud, 8N1 Mode
    tcgetattr(fd, &tio);
    cfsetispeed(&tio, B230400);
    cfsetospeed(&tio, B230400);
    tio.c_cflag &= ~PARENB;      // No parity
    tio.c_cflag &= ~CSTOPB;      // 1 stop bit
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;          // 8 data bits
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_iflag &= ~(IXON | IXOFF | IXANY);
    tcsetattr(fd, TCSANOW, &tio);

    tcflush(fd, TCIOFLUSH);

	// set interface non-blocking
	fcntl(fd, F_SETFL, O_NONBLOCK); // make non-blocking

    return fd;
}