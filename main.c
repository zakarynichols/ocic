#define _GNU_SOURCE
#include <errno.h>
#include <limits.h>
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

    char rootfs[PATH_MAX + 8];
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "getcwd: %s\n", strerror(errno));
        return 1;
    }

    snprintf(rootfs, sizeof(rootfs), "%s/rootfs", cwd);

    if (unshare(CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS) < 0) {
        fprintf(stderr, "unshare: %s\n", strerror(errno));
        return 1;
    }

    /*
     * Make mounts private so /proc does not leak to the host.
     *
     * Without this: the host's / is "shared" (propagation), so the child's
     * mount("proc", "/proc", ...) propagates back to the host. The mount
     * persists after container exit and accumulates with each run, blocking
     * rm -rf bundle. MS_PRIVATE stops this; MS_REC applies it recursively.
     *
     * Must be after unshare() and before fork().
     *
     * TO VERIFY THE BUG: comment this out, rebuild, then:
     *   $ gcc -o main main.c && cd bundle && rm -f ./ocic && cp ../main ./ocic
     *   $ echo "Before: $(mount | wc -l)"
     *   $ sudo ./ocic run ocic-test -c 'exit'
     *   $ echo "After:  $(mount | wc -l)"  # increases if bug is present
     *   $ mount | grep bundle              # shows leaked mounts
     * Restore this line to confirm the leak stops.
     */
    mount(NULL, "/", NULL, MS_PRIVATE | MS_REC, NULL);

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

        if (chdir(rootfs) < 0) {
            fprintf(stderr, "chdir: %s\n", strerror(errno));
            _exit(1);
        }

        if (chroot(".") < 0) {
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
