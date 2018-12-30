#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include "getword.h"
#include <libgen.h>
#include <pwd.h>
#define MAXITEMS 100
/*
Parse will be called in main right after our prompt is issued in order to get everything in the right place before we attempt execution.Parse
will also work with getword in order to set the proper flags for our main to catch, as well as fill our buffer with commands to be executed. 
Possible input and output files will also be set in parse if we catch a redirection from getword. Any simple syntax errors will be caught in parse,
by incrementing the flags past one.
 */
int parse();
/*
sighandler is used in order to protect ourselves from the killpg command
 */
void sighandler();
/* 
pipeFunction will be used in main when we detect a pipeline from the user. Our pipeline commands will be executed if possible and we will return
back to main in order to indicate success. 
*/
int pipeFunction();

/* 
 Pipe Function which will be used in main when multiple pipelines(under 10) are detected. 
*/
int multiplePipes();
