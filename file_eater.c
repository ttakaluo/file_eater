#include <stdio.h>
#include <unistd.h>	// fork, getpid, setsid
#include <sys/types.h>	// pid_t
#include <sys/stat.h>	// umask
#include <stdlib.h>	// exit
#include <syslog.h>	// syslog

#define LOG_DAEMON_NAME	"The Test Daemon"
#define LOG_FACILITY LOG_DAEMON
#define LOG_PRIORITY LOG_ERR

// make a process a daemon

// syslog is not opened in the daemonize
// routine. Stderr is used for error reporting
// until file descs are closed in the end.

void daemonize(void) {
	pid_t pid;
	
	pid = fork();
	if (pid < 0) {
		perror("on fork while daemonizing");
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		// if we're the parent, we will terminate now
		// so that the child will be moved under init
		printf("This is parent process, exiting.\n");
		exit(EXIT_SUCCESS);
	}

	//Child continues here
		

	// create a new session and put us into it

	pid = getpid();
	printf("The child is currently at PID %d.\n", pid);

	if (setsid() < 0) {
		printf("This is child process, exiting.\n");
		perror("on session creation while daemonizing");
		exit(EXIT_FAILURE);
	}

	pid = getpid();
	printf("The child was changed to PID %d.\n", pid);

	// change the directory into root.
	if (chdir("/") < 0) {
		perror("on root dir change while daemonizing");
		exit(EXIT_FAILURE);
	}
	
	// reset umask
	umask(0);

	// close the normal file descs.
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int check_for_file(void) {

	FILE *fp;

	fp = fopen("/home/ttakaluo/c_programming/file_eater/eat.me", "r");
	if (fp == NULL) {
		return 0;
	}
	else {
	syslog(LOG_PRIORITY, "Found a file\n");
		return 1;
	}
}

int main(int argc, char** argv) {
	
	pid_t pid, ppid;
	
	int fileExist = 0;
	int counter = 0;

	pid = getpid();
	ppid = getppid();

	// opening logging facility
	// we want the system logger (syslogd) to add
	// the PID to each message.
	openlog(LOG_DAEMON_NAME, LOG_PID, LOG_FACILITY);
	syslog(LOG_PRIORITY, "main[pid=%d, ppid=%d]: going daemon\n", pid, ppid);
	daemonize();
	
	syslog(LOG_PRIORITY, "main[pid=%d, ppid=%d]: scanning for file:\n", pid, ppid);

	while (1) {
		syslog(LOG_PRIORITY, "main[pid=%d, ppid=%d]: Scanning....\n", pid, ppid);
		fileExist = check_for_file();
		sleep(5);

		if (fileExist == 1) {
			unlink("/home/ttakaluo/c_programming/file_eater/eat.me");
			fileExist = 0;
			syslog(LOG_PRIORITY, "main[pid=%d, ppid=%d]: File destroyed!\n", pid, ppid);
			counter++;
		}
		if (counter > 5){
			break;
		}
	}
		
	syslog(LOG_PRIORITY, "stopping\n");
	return EXIT_SUCCESS;
}

