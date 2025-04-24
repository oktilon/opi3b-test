#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include <linux/types.h>

#define GPIO3                       "/dev/gpiochip3"
#define GPIO_BUTTON                 23 // RK_PC7
#define BUTTON_DEBOUNCE_US          50

static struct gpio_v2_line_request  req;
static struct gpio_v2_line_values   values;


/* helper functions for gpio_v2_line_values bits */
static inline void pin_set_bit(__u64 *b, int n) {
	*b |= _BITULL(n);
}

// static inline void pin_change_bit(__u64 *b, int n) {
// 	*b ^= _BITULL(n);
// }

// static inline void pin_clear_bit(__u64 *b, int n) {
// 	*b &= ~_BITULL(n);
// }

static inline int pin_test_bit(__u64 b, int n) {
	return !!(b & _BITULL(n));
}

// static inline void gpiotools_assign_bit(__u64 *b, int n, bool value) {
// 	if (value)
// 		gpiotools_set_bit(b, n);
// 	else
// 		gpiotools_clear_bit(b, n);
// }


int main(int argc, char **argv) {
    int fd, r, ret = 0;

    printf("Opening %s...\n", GPIO3);
    fd = open(GPIO3, O_RDWR);
    printf("Opened %s ret %d\n", GPIO3, fd);
    if (fd < 0) {
        printf("Open error(%d): %m\n", errno);
        return -1;
    }

    memset(&req, 0, sizeof(req));
    req.offsets[0] = GPIO_BUTTON;
    req.num_lines = 1;
    req.config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_FALLING | GPIO_V2_LINE_EVENT_RISING_EDGE;

    pin_set_bit(&req.config.attrs[0].mask, 0);
    req.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
    req.config.attrs[0].attr.debounce_period_us = BUTTON_DEBOUNCE_US;
    req.config.num_attrs = 1;
    strcpy(req.consumer, "Monitor");

    r = ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req);

    if (r < 0) {
        printf("Init Button error(%d): %m\n", errno);
        ret = r;
        goto exit_device_close;
    }
    printf("Init Button done(%d) fd=%d\n", r, req.fd);


    /* Read initial states */
    values.mask = 0;
    values.bits = 0;
    pin_set_bit(&values.mask, 0);

    r = ioctl(req.fd, GPIO_V2_LINE_GET_VALUES_IOCTL, values);
    if (r < 0) {
        printf("Get Button value error(%d): %m\n", errno);
        ret = r;
        goto exit_line_close;
    }

    printf("Initial line value: %d\n",
        pin_test_bit(values.bits, 0));

    while (1) {
        struct gpio_v2_line_event event;

        r = read(req.fd, &event, sizeof(event));
        if (r < 0) {
            if (errno == -EAGAIN) {
                printf("nothing available\n");
                continue;
            } else {
                printf("Failed to read event (%d): %m\n", errno);
                ret = -1;
                break;
            }
        }

        if (r != sizeof(event)) {
            printf("Reading event failed\n");
            ret = -EIO;
            break;
        }

        printf("GPIO EVENT at %llu on line %d (%d|%d) "
            , event.timestamp_ns
            , event.offset
            , event.line_seqno
            , event.seqno);

        switch (event.id) {
        case GPIO_V2_LINE_EVENT_RISING_EDGE:
            printf("rising edge");
            break;
        case GPIO_V2_LINE_EVENT_FALLING_EDGE:
            printf("falling edge");
            break;
        default:
            printf("unknown event");
        }
        printf("\n");

    }

exit_line_close:
    printf("Closing line...\n");
    close(req.fd);
exit_device_close:
    printf("Closing chip...\n");
    close(fd);

    printf("Exit\n");
    return ret;
}
