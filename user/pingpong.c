#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p1[2];
    int p2[2];
    char buf[10];
    if (pipe(p1) < 0)
    {
        fprintf(2, "pipe error\n");
        exit(1);
    }
    if (pipe(p2) < 0)
    {
        fprintf(2, "pipe error\n");
        exit(1);
    }
    int pid = fork();
    if (pid > 0)
    {
        close(p1[0]);
        write(p1[1], "ping", strlen("ping"));
        close(p1[1]);
        close(p2[1]);
        read(p2[0], buf, strlen("pong"));
        close(p2[0]);
        fprintf(2, "%d: received %s\n", getpid(), buf);
        exit(0);
    }
    else if (pid == 0)
    {
        close(p1[1]);
        read(p1[0], buf, strlen("ping"));
        close(p1[0]);
        fprintf(2, "%d: received %s\n", getpid(), buf);
        close(p2[0]);
        write(p2[1], "pong", strlen("pong"));
        close(p2[1]);
        exit(0);
    }
    else
    {
        fprintf(2, "fork error\n");
        exit(1);
    }
}