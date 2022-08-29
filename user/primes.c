#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void run(int p0, int current_num)
{
    int pid = fork();
    if (pid > 0)
    {
        return;
    }
    else if (pid == 0)
    {
        fprintf(2, "%d", current_num);
        int has_right = 0;
        int num;
        int p[2];
        while (read(p0, &num, 1))
        {
            if (has_right)
            {
                if (num % current_num != 0)
                {
                    close(p[0]);
                    write(p[1], (char *)&num, 1);
                    close(p[1]);
                }
            }
            else
            {
                if (num % current_num != 0)
                {
                    if (pipe(p) < 0)
                    {
                        fprintf(2, "pipe error");
                        exit(1);
                    }
                    close(p[0]);
                    write(p[1], (char *)&num, 1);
                    close(p[1]);
                    run(p[0], num);
                    has_right = 1;
                }
            }
        }
        wait(0);
        exit(0);
    }
    else
    {
        fprintf(2, "fork error\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int i;
    int p[2];
    if (pipe(p) < 0)
    {
        fprintf(2, "pipe error");
        exit(1);
    }

    for (i = 2; i < 36; i++)
    {
        close(p[0]);
        write(p[1], (char *)&i, 1);
        close(p[1]);
        if (i == 2)
        {
            run(p[0], i);
        }
    }
    wait(0);
    exit(0);
}