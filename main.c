#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "run") != 0) {
        fprintf(stderr, "Usage: %s run <container-id>\n", argv[0]);
        return 1;
    }

    char rootfs[512];
    char cwd[512];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "getcwd: %s\n", strerror(errno));
        return 1;
    }

    snprintf(rootfs, sizeof(rootfs), "%s/rootfs", cwd);

    if (unshare(CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS) < 0) {
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

        if (mount(rootfs, rootfs, "", MS_BIND, NULL) < 0) {
            fprintf(stderr, "mount: %s\n", strerror(errno));
            _exit(1);
        }

        if (chroot(rootfs) < 0) {
            fprintf(stderr, "chroot: %s\n", strerror(errno));
            _exit(1);
        }

        if (chdir("/") < 0) {
            fprintf(stderr, "chdir: %s\n", strerror(errno));
            _exit(1);
        }

        mount("proc", "/proc", "proc", 0, NULL);
        execl("/bin/sh", "/bin/sh", NULL);
        fprintf(stderr, "execl: %s\n", strerror(errno));
        _exit(1);
    }

    waitpid(pid, NULL, 0);
    return 0;
}
