#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#define GPIO3                       "/dev/gpiochip3"
#define GPIO_BUTTON                 23 // RK_PC7

static struct gpiohandle_request    btn = {0};

int main(int argc, char **argv) {
    int fd, r;

    printf("Opening %s...\n", GPIO3);
    fd = open(GPIO3, O_RDWR);
    printf("Opened %s ret %d\n", GPIO3, fd);
    if (fd < 0) {
        printf("Open error(%d): %m\n", errno);
        return -1;
    }

    /* Setup Button GPIO to input */
    btn.flags = GPIOHANDLE_REQUEST_INPUT;
    strcpy(btn.consumer_label, "Button");
    memset(btn.default_values, 0, sizeof(btn.default_values));
    btn.lines = 1;
    btn.lineoffsets[0] = GPIO_BUTTON;

    if ((r = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &btn)) < 0) {
        printf("Init GPIO%d (Button) error(%d): %m", GPIO_BUTTON, errno);
    } else {
        printf("Init GPIO%d (Button) ok(%d)", GPIO_BUTTON, r);
    }

    printf("Closing...\n");
    close(fd);

    printf("Exit\n");
    return 0;
}
