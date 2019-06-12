#ifndef UTILS_H
#define UTILS_H

DECLARE_FIXTURE(basic_fixture, void);

int stdin_pipe(void);

void input(int fd, const char *string_p);

#endif
