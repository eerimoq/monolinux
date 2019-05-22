#include <net/if_arp.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

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
    int netfd;

    strncpy(&ifreq.ifr_name[0], name_p, sizeof(ifreq.ifr_name));

    netfd = socket(AF_INET, SOCK_DGRAM, 0);
    xioctl(netfd, SIOCGIFFLAGS, &ifreq);
    ifreq.ifr_flags |= IFF_UP;
    xioctl(netfd, SIOCSIFFLAGS, &ifreq);
    close(netfd);
}

void ml_network_interface_down(const char *name_p)
{
    struct ifreq ifreq;
    int netfd;

    strncpy(&ifreq.ifr_name[0], name_p, sizeof(ifreq.ifr_name));

    netfd = socket(AF_INET, SOCK_DGRAM, 0);
    xioctl(netfd, SIOCGIFFLAGS, &ifreq);
    ifreq.ifr_flags &= ~IFF_UP;
    xioctl(netfd, SIOCSIFFLAGS, &ifreq);
    close(netfd);
}

void ml_network_interface_set_ipv4_address(const char *name_p,
                                           const char *address_p)
{
    struct ifreq ifreq;
    struct sockaddr_in sai;
    int netfd;

    netfd = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(&ifreq.ifr_name[0], name_p, sizeof(ifreq.ifr_name));
    memset(&sai, 0, sizeof(sai));
    sai.sin_family = AF_INET;
    sai.sin_port = 0;
    sai.sin_addr.s_addr = inet_addr(address_p);
    memcpy(&ifreq.ifr_addr, &sai, sizeof(ifreq.ifr_addr));
    xioctl(netfd, SIOCSIFADDR, &ifreq);
    close(netfd);
}

/* void ml_network_interface_set_ipv4_netmask(const char *name_p) */
/* { */
/*     netmask = htonl(0); */
/*     memcpy(&si->sin_addr, &netmask, 4); */
/*     xioctl(netfd, SIOCSIFNETMASK, &ifreq); */
/* } */

int main()
{
    /* ml_network_interface_down("eth0"); */
    /* sleep(10); */
    /* ml_network_interface_up("eth0"); */
    ml_network_interface_set_ipv4_address("eth0", "4.5.6.7");

    return (0);
}
