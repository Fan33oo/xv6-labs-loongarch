#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int read_line(char buf[])
{
    int i = 0;
    while (read(0, buf + i, 1))
    {
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            return 1;
        }
        i++;
    }
    buf[i] = '\0';
    return 0;
}

int main(int argc, char *argv[])
{
    char buf[100];
    char *argv_all[MAXARG];
    int i;
    for (i = 0; i < argc - 1; i++)
    {
        argv_all[i] = argv[i + 1];
    }
    int ret = 1;
    while (ret)
    {
        ret = read_line(buf);
        if (ret == 0)
        {
            break;
        }
        argv_all[argc - 1] = buf;
        int pid = fork();
        if (pid < 0)
        {
            fprintf(2, "fork error\n");
            exit(1);
        }
        else if (pid == 0)
        {
            exec(argv[1], argv_all);
            wait(0);
            exit(0);
        }
        else
        {
            wait(0);
            continue;
        }
    }
    exit(0);
}
