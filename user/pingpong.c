#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p[2];
    char buf[10];
    if (pipe(p) < 0)
    {
        fprintf(2, "pipe error");
    }
    int pid = fork();
    if (pid > 0)
    {
        write(p[1], "a", 1);
        close(p[1]);
        sleep(1);
        read(p[0], buf, 1);
        close(p[0]);
        fprintf(2, "%d: received pong\n", getpid());
        exit(0);
    }
    else if (pid == 0)
    {
        read(p[0], buf, 1);
        close(p[0]);
        fprintf(2, "%d: received ping\n", getpid());
        write(p[1], "b", 1);
        close(p[1]);
        exit(0);
    }
    else
    {
        fprintf(2, "fork error\n");
        exit(1);
    }
}