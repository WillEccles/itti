#include "readprocess.h"
#include <cstring>

// borrowed from http://stackoverflow.com/questions/125828/capturing-stdout-from-a-system-command-optimally

pid_t openChildProc(const char *command, int * infp, int * outfp)
{
	int p_stdin[2], p_stdout[2];
	pid_t pid;
	
	if (pipe(p_stdin) == -1)
		return -1;
	
	if (pipe(p_stdout) == -1) {
		close(p_stdin[0]);
		close(p_stdin[1]);
		return -1;
	}
	
	pid = fork();
	
	if (pid < 0) {
		close(p_stdin[0]);
		close(p_stdin[1]);
		close(p_stdout[0]);
		close(p_stdout[1]);
		return pid;
	} else if (pid == 0) {
		close(p_stdin[1]);
		dup2(p_stdin[0], 0);
		close(p_stdout[0]);
		dup2(p_stdout[1], 1);
		dup2(::open("/dev/null", O_RDONLY), 2);
		/// Close all other descriptors for the safety sake.
		for (int i = 3; i < 4096; ++i)
			::close(i);
		
		setsid();
		execl("/bin/sh", "sh", "-c", command, NULL);
		_exit(1);
	}
	
	close(p_stdin[0]);
	close(p_stdout[1]);
	
	if (infp == NULL) {
		close(p_stdin[1]);
	} else {
		*infp = p_stdin[1];
	}
	
	if (outfp == NULL) {
		close(p_stdout[0]);
	} else {
		*outfp = p_stdout[0];
	}
	
	return pid;
}

ssize_t readProcessOut(int procStdOut, char* buffer, size_t buffwidth) {
	if (!procStdOut)
		return -1;
	
	ssize_t bytes_read = read(procStdOut, buffer, buffwidth);
	return bytes_read;
}

std::string commandOutput(const char *command) {
	std::string output;
	int child_stdout = -1;
	pid_t child_pid = openChildProc(command, 0, &child_stdout);
	if (!child_pid)
		return PROCESS_ERROR; // error starting process
	else {
		char buff[5000];
		ssize_t status = readProcessOut(child_stdout, buff, sizeof(buff));
		if (status >= 0)
			output = std::string(buff);
		else
			return OUTPUT_ERROR;
	}
	
	return output;
}
