#include <net/if_arp.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void xioctl(int fd, unsigned long request, void *data_p)
{
    int res;

    res = ioctl(fd, request, data_p);

    if (res != 0) {
        printf("ioctl failed.\n");
        exit(1);
    }
}

void ml_network_interface_up(const char *name_p)
{
    struct ifreq ifreq;
    int fd;

    strncpy(&ifreq.ifr_name[0], name_p, sizeof(ifreq.ifr_name));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    xioctl(fd, SIOCGIFFLAGS, &ifreq);
    ifreq.ifr_flags |= IFF_UP;
    xioctl(fd, SIOCSIFFLAGS, &ifreq);
}

void ml_network_interface_down(const char *name_p)
{
    struct ifreq ifreq;
    int fd;

    strncpy(&ifreq.ifr_name[0], name_p, sizeof(ifreq.ifr_name));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    xioctl(fd, SIOCGIFFLAGS, &ifreq);
    ifreq.ifr_flags &= ~IFF_UP;
    xioctl(fd, SIOCSIFFLAGS, &ifreq);
}

int main()
{
    ml_network_interface_down("eth0");
    sleep(10);
    ml_network_interface_up("eth0");

    return (0);
}
