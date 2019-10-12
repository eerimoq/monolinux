#include <unistd.h>
#include <narwhal/narwhal.h>
#include "ml/ml.h"
#include "utils.h"
#include "mocks/mock.h"

TEST_FIXTURE(basic_fixture, void)
{
    ml_init();

    CLEANUP_FIXTURE(basic_fixture)
    {
        mock_finalize();
    }
}

int stdin_pipe(void)
{
    int fds[2];

    ASSERT_EQ(pipe(fds), 0);
    dup2(fds[0], STDIN_FILENO);

    return (fds[1]);
}

void input(int fd, const char *string_p)
{
    ssize_t length;

    length = strlen(string_p);

    ASSERT_EQ(write(fd, string_p, length), length);
}
