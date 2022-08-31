#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
get_name(char *path)
{
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

void find(char *path, char *file_name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (strcmp(get_name(buf), ".") != 0 && strcmp(get_name(buf), "..") != 0)
            {
                if (strcmp(get_name(buf), file_name) == 0)
                {
                    printf("%s\n", buf);
                }
                else
                {
                    find(buf, file_name);
                }
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "find: at least pass one argument!\n");
        exit(1);
    }
    else if (argc == 2)
    {
        find(".", argv[1]);
        exit(0);
    }
    else
    {
        int i;
        for (i = 2; i < argc; i++)
        {
            find(argv[1], argv[i]);
        }
        exit(0);
    }
}
