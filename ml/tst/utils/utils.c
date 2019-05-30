#include <unistd.h>
#include <unicorn/unicorn.h>
#include "utils.h"

int stdin_pipe(void)
{
    int fds[2];

    ASSERT_EQ(pipe(fds), 0);
    dup2(fds[0], STDIN_FILENO);

    return (fds[1]);
}

void input(int fd, const char *string_p)
{
    size_t length;

    length = strlen(string_p);

    ASSERT_EQ(write(fd, string_p, length), length);
}
