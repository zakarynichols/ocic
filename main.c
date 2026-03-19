#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    if (unshare(CLONE_NEWUTS) < 0) {
        fprintf(stderr, "unshare: %s\n", strerror(errno));
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork: %s\n", strerror(errno));
        return 1;
    }

    if (pid == 0) {
        if (sethostname("container", 9) < 0) {
            fprintf(stderr, "sethostname: %s\n", strerror(errno));
            _exit(1);
        }
        execl("/bin/sh", "/bin/sh", NULL);
        fprintf(stderr, "execl: %s\n", strerror(errno));
        _exit(1);
    }

    waitpid(pid, NULL, 0);
    return 0;
}
