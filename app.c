#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#define GPIO3                       "/dev/gpiochip3"
#define GPIO_BUTTON                 23 // RK_PC7


int main(int argc, char **argv) {
    int fd;

    printf("Opening %s...\n", GPIO3);
    fd = open(GPIO3, O_RDWR);
    printf("Opened %s ret %d\n", GPIO3, fd);
    if (fd < 0) {
        printf("Open error(%d): %m\n", errno);
        return -1;
    }

    /* Setup Button GPIO to input */
    // r = pin_init_gpio(fd, GPIO_BUTTON, GPIOHANDLE_REQUEST_INPUT, "Button", &btn);
    // if (r) goto error;

    printf("Closing...\n");
    close(fd);

    printf("Exit\n");
    return 0;
}
