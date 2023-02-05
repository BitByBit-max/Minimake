#include <err.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        err(1, "wrong number of arguments");
    }
    int pid = fork();
    if (pid == -1)
    {
        err(1, "error in the fork");
    }
    if (pid == 0)
    {
        execl("/bin/sh", "supershell", "-c", argv[1], NULL);
    }
    int wstatus;
    int child_pid = waitpid(pid, &wstatus, 0);
    if (child_pid == -1)
    {
        err(1, "wait child failed");
    }
    printf("process exit status: %d\n", WEXITSTATUS(wstatus));
    return 0;
}
