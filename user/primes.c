#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void run(int *p, int current_num)
{
    int pid = fork();
    if (pid > 0)
    {
        return;
    }
    else if (pid == 0)
    {
        char has_right = 0;
        int num;
        int p_new[2];
        int ret;
        fprintf(2, "prime %d\n", current_num);
        while (1)
        {
            close(p[1]);
            ret = read(p[0], &num, sizeof(num));
            if (ret <= 0)
            {
                break;
            }
            if (num % current_num != 0)
            {
                if (has_right)
                {
                    close(p_new[0]);
                    write(p_new[1], &num, sizeof(num));
                }
                else
                {
                    if (pipe(p_new) < 0)
                    {
                        fprintf(2, "pipe error\n");
                        exit(1);
                    }
                    run(p_new, num);
                    has_right = 1;
                }
            }
        }
        close(p_new[1]);
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
    int i = 2;
    int p[2];
    if (pipe(p) < 0)
    {
        fprintf(2, "pipe error\n");
        exit(1);
    }
    run(p, i);
    for (i = 3; i < 36; i++)
    {
        close(p[0]);
        write(p[1], &i, sizeof(i));
    }
    close(p[1]);
    wait(0);
    exit(0);
}
