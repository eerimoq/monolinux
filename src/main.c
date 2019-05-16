#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

static int smount(const char *source_p,
                  const char *target_p,
                  const char *type_p)
{
    int res;

    res = mount(source_p, target_p, type_p, 0, "");

    if (res != 0) {
        perror("error: mount: ");
    }

    return (res);
}

static int init(void)
{
    int res;

    res = smount("none", "/proc", "proc");

    if (res != 0) {
        return (res);
    }

    res = smount("none", "/sys", "sysfs");

    if (res != 0) {
        return (res);
    }

    return (smount("none", "/sys/kernel/debug", "debugfs"));
}

static void print_uptime(void)
{
    int fd;
    char buf[128];
    ssize_t size;

    fd = open("/proc/uptime", O_RDONLY);

    if (fd < 0) {
        perror("error: ");
    }

    size = read(fd, &buf[0], sizeof(buf) - 1);

    if (size >= 0) {
        buf[size] = '\0';
        printf("Uptime: %s seconds\n", strtok(&buf[0], " "));
    }

    close(fd);
}

int main()
{
    int res;

    res = init();

    if (res != 0) {
        printf("error: Init failed. Aborting.\n");

        return (res);
    }

    while (1) {
        print_uptime();
        sleep(1);
    }

    return (0);
}
