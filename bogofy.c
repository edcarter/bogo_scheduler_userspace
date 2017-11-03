#define _GNU_SOURCE
#include <unistd.h>		/* syscall */
#include <sys/syscall.h>	/* STS_xxx definitions */
#include <stdio.h>		/* printf, perror */
#include <stdlib.h>		/* strtol */
#include <linux/types.h>	/* __u32 ... etc */
#include <linux/sched.h>	/* SCHED_FLAG_RESET_ON_FORK */

#define BOGO_POLICY 7

struct sched_attr {
	__u32 size;

	__u32 sched_policy;
	__u64 sched_flags;

	/* SCHED_NORMAL, SCHED_BATCH */
	__s32 sched_nice;

	/* SCHED_FIFO, SCHED_RR */
	__u32 sched_priority;

	/* SCHED_DEADLINE (nsec) */
	__u64 sched_runtime;
	__u64 sched_deadline;
	__u64 sched_period;
};

void usage() {
	printf("%s\n", "usage: bogofy pid");
	exit(EXIT_FAILURE);
}

int sched_setattr(pid_t pid,
		  const struct sched_attr *attr,
		  unsigned int flags) {
	return syscall(SYS_sched_setattr, pid, attr, flags);
}

int sched_getattr(pid_t pid,
		  struct sched_attr *attr,
		  unsigned int size,
		  unsigned int flags) {
	return syscall(SYS_sched_getattr, pid, attr, size, flags);
}

int main(int argc, char **argv) {
	pid_t pid;
	char *ptr;

	if (argc != 2)
		usage();

	pid = (pid_t) strtol(argv[1], &ptr, /*base*/ 10);
	if (!pid) perror("strtol");

	struct sched_attr attr = {
		.size = sizeof(struct sched_attr),
		.sched_policy = BOGO_POLICY,
		.sched_flags = SCHED_FLAG_RESET_ON_FORK,
		.sched_nice = 0,
		.sched_priority = 0,
		.sched_runtime = 0,
		.sched_deadline = 0,
		.sched_period = 0
	};

	if(sched_setattr(pid, &attr, 0)) perror("sched_setattr");
	
	exit(EXIT_SUCCESS);
}
