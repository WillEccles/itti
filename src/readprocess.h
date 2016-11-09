#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

#define PROCESS_ERROR std::string("Error starting process.")
#define OUTPUT_ERROR std::string("Error reading from process stdout.")

// open a process "command" and eat its output
// command: the command to run
// infp: the handle with which to associate process's stdin (pass 0 if input shouldn't be stored). initially set int in which to store as -1.
// outfp: the handle with which to associate the process's stdout. the int in which it will be stored should be set to -1 initially.
pid_t openChildProc(const char *command, int *infp, int *outfp);

// read the output of a process opened with openChildProc.
// returns -1 if error, 0 if no output, and otherwise the number of bytes read.
// procStdOut: the handle associated with the process's stdout.
// buffer: the buffer in which to store the output read
ssize_t readProcessOut(int procStdOut, char* buffer, int buffwidth);

// returns a string containing the ouput of a given command
std::string commandOutput(const char *command);
