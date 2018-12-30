/*
	Author: Anthony Reese
	Program: p2.c
	REDID : 817839276	
	Professor: Dr. John Carroll
	Class: CS570 - Operating Systems
	Due date: 10/10/18 

*/

#include "p2.h"
char s[STORAGE];
int wordCount;
char *newargv[MAXITEMS];
int c;
int rightFlag;
int leftFlag;
int pipeFlag;
char *input;
char *output;
char *hereIsWord;
int backGroundFlag;
int letterCount;
extern int backSlashFlag;
extern int environFlag;
extern int userLookFlag;
char argv[MAXITEMS * STORAGE];
int inputFile;
int outputFile;
int pipeIndex[10];
int hereIsFlag;
char *prompt;	
int printDifferent;
char *updatePrompt;
char *showenv;
char cwd[256];
int varFlag;
char *environvar; 
char badVar[254];
char badUserVar[254];
int badUserFlag;
	int main(){
		FILE *file;
		size_t size;	
		int kidpid,pid;	
		int returnVal;
		int fd;	
		char *hereIsLine;
		size_t bufsize = STORAGE; 
		size_t characters;	
		setpgid(0,0);
		signal(SIGTERM,sighandler);
		prompt = ":570: ";	
		for(;;){
			outputFile = -1;	
			inputFile = -1;	
			showenv = NULL;	
			fflush(stdout);
			fflush(stderr);	
			if(printDifferent == 1)
				printf("%s",updatePrompt);	
			printf("%s",prompt);
			returnVal = parse();
		/*
		In these next 5 if statements, we're checking for a EOF, if that is detected then we break, if there's no input we reissue theprompt to the user,
		if any flag is greater then one(pipeFlag  '|' needs to be greater than 10), meaning there is more than one '>','<',  '<<', then we issue an error to the user and reissue the prompt.
 
*/		
			if(returnVal == -255)
				break;
			if(wordCount == 0)
				continue;

			if(pipeFlag > 10){
				(void)fprintf(stderr,"ERROR: Only 10 pipelines are supported.\n");
				continue;
			}
				
			if(rightFlag > 1){
				(void)fprintf(stderr,"ERROR: Ambiguous output redirection, use only one '>'.\n");
				continue;
			}
		
			if(leftFlag > 1){
				(void)fprintf(stderr,"ERROR: Ambiguous input redirection, use only one '<'.\n");
				continue;
			}

			if(hereIsFlag > 1){
				(void)fprintf(stderr,"ERROR: Ambiguous input redirection, use only one '<<'.\n");
				continue;
			}
		
		/* We issue an error if there is a '<<' and '<' on the same line as '<<' is already used for input redirection */	
			if(hereIsFlag > 0 && leftFlag > 0){
				(void)fprintf(stderr,"ERROR: hereis functionality and input redirection should be used seperately.\n");
				continue;

			} 	
	/* These next two if statements will check to see if the variables after either a '$' or '~' could not be accessed. */
			if(varFlag == 1){
				(void)fprintf(stderr,"'%s': Undefined Variable.\n",badVar);
				continue;
			}

			if(badUserFlag == 1){
				(void)fprintf(stderr,"'%s': Username lookup has failed.\n",badUserVar);
				continue;
			}

	/* Here we set up our hereIs file in order to be used for input redirection, we open a file(doing error checks) and then copy into the file from stdin until the terminator is found */	
			if(hereIsFlag == 1){
				if(hereIsWord == NULL){
					(void)fprintf(stderr,"ERROR: No hereis terminator provided!\n");
					hereIsFlag =0;
					continue;
				}
				file = fopen(hereIsWord,"w+");
				if(file == NULL){
					(void)fprintf(stderr,"ERROR: Failed to create '%s' hereis file.\n",hereIsWord);
					continue;
				}

				else{
					while((getline(&hereIsLine,&size,stdin)) != -1){
						strtok(hereIsLine,"\n");
						if(strcmp(hereIsLine,hereIsWord) == 0)
							break;
						else{
							fputs(hereIsLine,file);
							fputs("\n",file);
						}
					}	
					fclose(file);
					input= hereIsWord;	
					strcpy(input,hereIsWord);
					leftFlag =1;	
				}	
			}	


		/*
		In these next 2 if statements we're checking for valid input or output redirection. If there was a redirection detected, but no name was given for redirection
		then we print an error and reissue the prompt. If there is input redirection and we cannot access the file, or there is output redirection and the file exists,
		or there is output redirection and the file failed to create, then we issue the corresponding error message and reissue the prompt.
 */	
			if(leftFlag == 1){
				if(input == NULL){
					(void)fprintf(stderr,"ERROR: No file was chosen for input redirection.\n");
					continue;
				}	
				inputFile = open(input,O_RDONLY);
				if(inputFile == -1){	
					(void)fprintf(stderr,"ERROR: Failed to open '%s'.\n",input);
					input = NULL;	
					leftFlag = 0;	
					continue;	
				}
			}					
			
			if(rightFlag == 1){
				if(output == NULL){
					(void)fprintf(stderr,"ERROR: No file was chosen for output redirection.\n");
					rightFlag = 0;	
					continue;
				}	
				if((access(output,F_OK)) == 0){
					(void)fprintf(stderr,"ERROR: You cannot overwrite '%s'.\n",output);
					output = NULL;
					rightFlag = 0;	
					continue;
				}
				outputFile = open(output,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);
				if(outputFile == -1){
					(void)fprintf(stderr,"ERROR: Failed to create '%s'.\n",output);
					output = NULL;	
					continue;	
				}
			}
		/*
		If we get here then either we have a 'cd', which is the next if statement, we have a pipe, or we need to simply try and execute. If a cd is detected, it will
		try to change the directory if there is 1 argument passed in or no arguments. If there's none we cd to our home directory, if not then we try to cd into the 
		given directory. If either cd fails, then the corresponding error message is sent to the user and we reissue the prompt. After changing directories, we use getcwd()
		in order to update our prompt with the appropriate name. 
*/
			if(strcmp(newargv[0],"cd") == 0){
				if(wordCount > 2){
					(void)fprintf(stderr,"ERROR: 'cd' expects no more than one argument.\n");
				}
				else if(wordCount == 2){
					if(chdir(newargv[1]) == -1){
						(void)fprintf(stderr,"ERROR: '%s' Invalid directory.\n", newargv[1]);
					}
					printDifferent = 1;	
					getcwd(cwd,sizeof(cwd));
					updatePrompt = basename(cwd);
				}
				else{
					if(chdir(getenv("HOME")) == -1){
						(void)fprintf(stderr,"ERROR: Invalid home directory.\n");
					}
					printDifferent = 1;	
					getcwd(cwd,sizeof(cwd));	
					updatePrompt = basename(cwd);	
				}
			}	
			else if(strcmp(newargv[0],"environ") == 0){
				if(wordCount > 3)
					(void)fprintf(stderr,"ERROR: 'environ' expects no more than two arguments.\n");
				else if(wordCount == 3){
					setenv(newargv[1],newargv[2],1);	
				}
				else{
					if(wordCount == 2){
						showenv = getenv(newargv[1]);
						if(showenv != NULL)	
							printf("%s\n",showenv);
						else{
							printf("(null)\n");
						}	
					}		
				}
			}	
		/*
		If we have a pipeFlag and our pipeIndex hasn't changed, then we know there is no second child, so we send an error message to the user and reissue the 
		prompt. If not then we call our pipeFunction which will handle our redirection and execution of both children(see comments for that function, for specifics).
		If pipeFunction returns 0, then everything went right and we can reissue the prompt. 
*/	
			
			else if(pipeFlag == 1){
				if(pipeIndex[0] == 0){
					(void)fprintf(stderr,"ERROR: Invalid input for pipeline, two children are expected.\n");
					continue;
				}	
				if((pipeFunction()) == 0){
					leftFlag = 0;
					rightFlag =0;
					pipeFlag =0;	
					continue;
				}
			}

		/*
 		If we have between 2 and 10 pipelines, we go to our multiplePipeline function and execute there, if 0 is returned then we were succesful and we reissue the prompt.
		If our first pipeIndex is 0, then we know that each pipe does not have two commands and we issue an error.
 */

			else if(pipeFlag > 1 && pipeFlag < 11){
				if(pipeIndex[0] == 0){
					(void)fprintf(stderr,"ERROR: Invalid input for pipeline. Two children are expected for each pipe.\n");
				}
				
				if(multiplePipes() == 0){
					pipeFlag = 0;	
					continue;
				}	
			}

		/*
		If we get here, we try and execute commands correspondingly, after creating a child process and if there is any redirection then it is handled in here.
		If the command is detected to be a background process, then the parent does not wait for the child to finish trying to execute.
 */		
			else{
				fflush(stdout);
				fflush(stderr);
				kidpid = fork();	
				if(kidpid == -1){
					(void)fprintf(stderr,"ERROR: Failed forking process.\n");
					exit(1);
				}
		/*
		Here we know that the fork was succesful and we are in our child process. If there is any redirection it is executed in here and if we have failure then
		the corresponding message is sent and we exit from the child. If there was not input redirection AND we are detected to be a background process, then input
		is redirected to /dev/null to prevent the child from reading from the parent. After all of the redirection is finished, we try to execute the commands which
		are pointed at by our newargv[]. If execution fails then we send an error to the user and exit from the child. 
 */		
				else if(kidpid == 0){
					if(outputFile != -1){
						if((dup2(outputFile,STDOUT_FILENO)) == -1){
							(void)fprintf(stderr,"ERROR: '%s' Output redirection has failed.\n",outputFile);
							exit(2);
						}			
							close(outputFile);
							outputFile = -1;	
							rightFlag = 0;
					}
					if(inputFile != -1){
						if((dup2(inputFile,STDIN_FILENO)) == -1){
							(void)fprintf(stderr,"ERROR: '%s' Input redirection has failed.\n",inputFile);
							exit(3);
						}	
							close(inputFile);
							inputFile == -1;
							leftFlag = 0;	
					}
					else{
						if(backGroundFlag == 1){	
							if((fd = open("/dev/null",O_RDONLY)) == -1){
								(void)fprintf(stderr,"ERROR: failed to open '/dev/null'.\n");
								exit(14);
							}	
							if((dup2(fd,STDIN_FILENO)) == -1){
								(void)fprintf(stderr,"ERROR: '/dev/null' input redirection failed.\n");
								exit(13);
							}
						}	
					}	
					if(execvp(newargv[0],newargv) == -1){
						(void)fprintf(stderr,"ERROR: '%s' Execution has failed.\n",newargv[0]);
						exit(4);	
					}	
				}
		/*
		Here we know that we are in the parent process, if a the command was detected to be a background process, the parent does not wait and reissues another 
		prompt after printing out the process id and first command of the child. If the command was not a background process, then we wait for the child which
		is trying to execute. 
*/		
				else{
					if(backGroundFlag == 1){
						printf("%s [%d]\n",newargv[0],kidpid);
						backGroundFlag == 0;	
						continue;	
					}
					else{
							do{
								pid = wait(NULL);
							}while(pid != kidpid);	
							remove(hereIsWord);	
					}	
				}	
			}	
		}
		killpg(getpgrp(),SIGTERM);
		printf("p2 terminated.\n");
		exit(0);	
	}

	/*
	Parse works together with getword in order to determine if we need to redirect input/output, if we have a pipeline, and if there is a background process.
	If none of these are detected then we insert our word into argv and make newargv[] point at each corresponding 'valid'(for now, we still need to try and execute)
	word. If an EOF, new line, or ';' is detected then we break from our loop and insert a null terminator into our newargv[]. Furthermore, if a '<','>','|', '&', or 
	'\n' is detected to be directly after a '\', then we ignore that character and move on. newargv[] and argv are both incremented in order to skip the previous word
	which was placed into argv and pointed at by newargv. 

	p4 Additions - when parse encounters a word with a leading "$" which was detected by getword, we use getenv() on the word and replace the result(if valid) with the result.
			the word is then placed into the array.
			When a '~' is discovered to be the first character of a word, we use getpwnam() on the word in order to access the given passwd struct for valid users.
			pw->pw_dir is then used to return the correct field of our username lookup. The result of this will replace our initial word and be placed into the array.		 
			The same logic and I/O redirection is used for hereis functionality. If we encounter '<<' we increase the hereIsFlag and continue. If the hereIsFlag is 1
			and our hereIsVariable hasn't been set yet, then we take the next word to be used in our hereIs functionality.  
 
*/

	int parse(){
		struct passwd *pw;	
		char *index = argv;	
		int i;
		char *userPasswd; 
		input = NULL;
		output = NULL;
		hereIsWord = NULL;	
		wordCount = 0;
		rightFlag = 0;
		leftFlag = 0;
		pipeFlag = 0;
		backGroundFlag = 0;
		backSlashFlag = 0;
		pipeIndex[0] = 0;
		hereIsFlag = 0;	
		environFlag = 0;	
		varFlag = 0;	
		badUserFlag =0;
		for(;;){
			letterCount = getword(s);
			if(letterCount == 0 || letterCount == -255){
				if(letterCount == 0 && backSlashFlag == 1)
					continue;
				else
					break;
			}	
			if(backGroundFlag == 1){
				backGroundFlag =0;	
				*index = '&';	
				index += 1 ;
				*index ='\0';	
				newargv[wordCount++]= index -1;
				index +=2;	
			}	
			if(letterCount == 1 && *s == '&' && backSlashFlag == 0){
					backGroundFlag = 1;
					continue;			
			}

			if(wordCount == MAXITEMS - 1){
				(void)fprintf(stderr,"ERROR: Too many arguments");
				break;
			}	
			if(environFlag == 1){
				environvar = NULL;
				environvar = getenv(s);
				if(environvar == NULL){
					varFlag =1;	
					strcpy(badVar,s);		
				}
				else{
					strcpy(s,environvar);
					letterCount = strlen(s);	
				}
			}
			if(userLookFlag == 1){
				pw = getpwnam(s);	
				if(pw == NULL){
					badUserFlag =1; 
					strcpy(badUserVar,s);
				}	
				else{	
					strcpy(s,pw->pw_dir);
					letterCount = strlen(s);	
				}
			}	
			if(letterCount == 2 && backSlashFlag == 0){
				if(strcmp(s,"<<") == 0){
					hereIsFlag++;
					continue;
				}
			}	
			if(letterCount == 1 && *s == '|' && backSlashFlag == 0){
				pipeFlag++;
				newargv[wordCount++]=NULL;
				if(pipeFlag < 11)	
					pipeIndex[pipeFlag -1] = wordCount; 	
			}

			else if(letterCount == 1 && *s == '>' && backSlashFlag == 0){
				rightFlag++;
				continue;	
			}

			else if(letterCount == 1 && *s == '<' && backSlashFlag == 0){
				leftFlag++;
				continue;	
			}
			else if((leftFlag == 1 && input == NULL)){
				strcpy(index,s);
				input = index;
				index += letterCount + 1;	
				continue;
			}
			else if(rightFlag == 1 && output == NULL) {
				strcpy(index,s);
				output = index;
				index += letterCount +1;	
				continue;
			}	
			else if(hereIsFlag == 1 && hereIsWord == NULL){
				strcpy(index,s);
				hereIsWord = index;
				index += letterCount +1;
				continue;	
			}	
				
			else{
				strcpy(index,s);	
				newargv[wordCount++] = index ;	
			}	

		index += letterCount +1;	
		}

		newargv[wordCount] = '\0';
		return letterCount;
	}

		/*
		sighandler is here in order to protect ourselves from being killed by killpg when it is called towards the end of main.		 
*/
	void sighandler(){};
	

		/*
		Here we set up our vertical pipe line. First we fork a child process, and that initial child process is waited on by the parent. Then our pipe is
		set up by the child process and we fork again, creating a grandchild process. The child process will then wait on the grandchild to try and execute
		before carrying on. In the grandchild, we deal with the first child of the pipeline. Here we also check for any input file redirection and handle
		it correspondingly. If there is no input redirection and we are detected to be a background process, then input is redirected to '/dev/null', in order
		to avoid reading from the grandchild process's parent, the child process. After these checks are done, we do our pipe redirection and then we try
		and execute. If any errors are detected in the grandchild, we print the corresponding error message and exit, we also close both file descriptors in order
		to avoid a possible deadlock. In our grand child, our pipe redirection is done in order to read from the pipe line.
	
		When the child process is done waiting for the grand child process, we deal with the second child of the pipeline. In here, we will check for any output 
		redirection and handle it accordingly. After this we handle our pipe redirection, in order to write into the pipe line. Once all of this is finished,
		we try to execute the second child of our pipeline. If any error occurs in the child process, then the corresponding error message is sent to the user
		and we exit.

		Finally, when the original parent gets the signal that his child process has finished, we return 0 to main to signal a success. Something that could
		have been done differently.... Is calling this function in main after our original fork, if a pipeline was detected. This would make this function a 
		tad shorter and we would not have to fork twice in here, because we would already be the child process.
*/

	int pipeFunction(){
		int pid, kidpid,grandkidpid;
		int fds[2];
		int fd;
		fflush(stdout);
		fflush(stderr);	
		kidpid = fork();
		if(kidpid == -1)
			return -1;
		else if(kidpid == 0){
			if((pipe(fds)) == -1){
				(void)fprintf(stderr,"ERROR: Fork proccess failed.\n");
				exit(17); 
			}	
			fflush(stdout);
			fflush(stderr);	
			grandkidpid = fork();
			if(grandkidpid == -1){
				(void)fprintf(stderr,"ERROR: Fork proccess failed.\n");
				exit(16);
			}	
			else if(grandkidpid == 0){
				if(inputFile!= -1){
					if((dup2(inputFile,STDIN_FILENO) == -1)){
						(void)fprintf(stderr,"ERROR: Input redirection has failed.\n");
						exit(3);
						}
					inputFile = -1;
					input = NULL;
					close(inputFile);	
				}
				else{
					if(backGroundFlag == 1){
						if((fd = open("/dev/null",O_RDONLY)) == -1){
							(void)fprintf(stderr,"ERROR: '/dev/null' failed to open.\n");
							exit(12);
						}
						if((dup2(fd,STDIN_FILENO)) == -1){
							(void)fprintf(stderr,"ERROR: Input redirection has failed.\n");
							exit(3);
						}
					}
				}	
				if((dup2(fds[1],STDOUT_FILENO)) == -1){
					(void)fprintf(stderr,"ERROR: Pipe output redirection has failed.\n");
					exit(9);				
				}
				close(fds[0]);
				close(fds[1]);
				if((execvp(newargv[0],newargv)) == -1){
					(void)fprintf(stderr,"ERROR: '%s' Execution has failed.\n",newargv[0]);
					exit(4);
				}	
			}
				
		
			else{
				if(backGroundFlag == 0){
						pid_t pid;
						do{
							pid = wait(NULL);		
						}while(pid != grandkidpid);
						if(outputFile != -1){
							if((dup2(outputFile,STDOUT_FILENO)) == -1){
								(void)fprintf(stderr,"ERROR: Output redirection has failed.\n");
								exit(2);	
							}
							outputFile = -1;
							output = NULL;
							close(outputFile);	
						}	
						if((dup2(fds[0],STDIN_FILENO)) == -1){
							(void)fprintf(stderr,"ERROR: Pipe input redirection has failed.\n");
							exit (10);
						}	
						close(fds[0]);
						close(fds[1]);
						if((execvp(newargv[pipeIndex[0]],(newargv + pipeIndex[0]))) == -1){
							(void)fprintf(stderr,"ERROR: '%s' Execution has failed.\n",newargv[pipeIndex[0]]);
							exit(4);
						}
				}	
				
				else{
					printf("[%s],%d.\n",newargv[0],pid);	
					if(outputFile != -1){
						if((dup2(outputFile,STDOUT_FILENO)) == -1){
							(void)fprintf(stderr,"ERROR: Output redirection has failed.\n");
							outputFile = -1;
							output = NULL;	
							exit(2);
						}
						outputFile = -1;
						output = NULL;
						close(outputFile);
					}
					if((dup2(fds[0],STDIN_FILENO)) == -1){
						(void)fprintf(stderr,"ERROR: Pipe input redirection has failed.\n");
						exit(10);
					}
					close(fds[0]);
					close(fds[1]);
					if((execvp(newargv[pipeIndex[0]],newargv+pipeIndex[0])) == -1){
						(void)fprintf(stderr,"ERROR: '%s' Execution has failed.\n",newargv[pipeIndex[0]]);
						exit(4);
					}
				}	
			}
		}
		else{
			pid_t pid;	
			do{
				pid = wait(NULL);
			}while(pid != kidpid);	
			remove(hereIsWord);	
			return 0;
		}
	}

	/*
		We use this function when we have in between 2 and 10 pipelines. If there is one our pipeFunction takes care of it, more than 10 pipelines 
		aren't supported. First we fork and our p2 will wait for all of the processes created by various pipelines to finish, before returning to main.
		In our child we create our pipe at our first spot in our fd array which was initialized by the size of pipeFlag(number of pipes). Our child will 
		handle the very last child in the argument, redirecting output(if needed) and reading from the created pipeline. Our grandchild  will handle the rest of the children.
		(from the child second last to the right to the child on the left.For the amount of pipelines - 1, we want to create a new pipe and then fork in order to create another 
		process. If the given grandchild is the parent, it
		will handle the I/O redirection. Meaning that it will write to it's inherited pipe, and read from the pipeline that it created. In each child that was created,
		we check if we're the last child(meaning the first child in the given command). If we're detected to be the last child, we exec in the child and the loop is finished.
		Before doing this, we redirect output to the write end of the last created pipe, and redirect input if needed. If not, we continue the loop, creating a pipe for the next child to write to.
	*/  
 
	int multiplePipes(){
		int pipeNum = 1;
		int pid,kidpid,grandkidpid,greatgkpid;
		int fd[pipeFlag * 2];	
		int grandKidCommands = pipeFlag-2;	
		int i;	
		int dev;
		fflush(stdout);
		fflush(stderr);	
		kidpid = fork();
		if(kidpid == -1){
			(void)fprintf(stderr,"ERROR: Failed forking process.\n");
			exit(1);
		}
		else if(kidpid == 0){
			pipe(fd);			
			fflush(stderr);
			fflush(stdout);
			grandkidpid = fork();
			if(grandkidpid == -1){
				(void)fprintf(stderr,"ERROR: Failed forking process.\n");
				exit(1);
			}
			else if(grandkidpid == 0){
				for(i=0 ; i < pipeFlag -1; i++){
					pipe(fd + (pipeNum*2));
					fflush(stderr);
					fflush(stdout);
					greatgkpid = fork();
					if(greatgkpid == -1){
						(void)fprintf(stderr,"ERROR: Failed forking process.\n");
						exit(1);
					}
					else if(greatgkpid == 0){
						if ((pipeNum + 1) == pipeFlag){
							if(inputFile != -1){
								if((dup2(inputFile,STDIN_FILENO)) == -1 ){
									(void)fprintf(stderr,"ERROR: Input redirection has failed.\n");
									input = NULL;
									inputFile = - 1;
 									exit(5);	
								}
								input = NULL;
								close(inputFile);		
							}
							else{
								if(backGroundFlag == 1){
									if((dev = open("/dev/null",O_RDONLY)) == -1){
										(void)fprintf(stderr,"ERROR: '/dev/null' failed to open.\n");
										exit(12);
									}
								
									if((dup2(dev,STDIN_FILENO)) == -1){
										(void)fprintf(stderr,"ERROR: Input redirection has failed.\n");
										exit(3);
									}	
								}		
							}	
							if((dup2(fd[pipeNum*2 + 1],STDOUT_FILENO)) == -1){
								(void)fprintf(stderr,"ERROR: Leftmost pipe output redirection has failed.\n");	
								exit(6);	
							}	
							for(i=0; i <= pipeNum*2 +1; i++){
								close(fd[i]);
							}	
							if((execvp(newargv[0],newargv)) == -1){
								(void)fprintf(stderr,"ERROR: '%s' Execution has failed.\n",newargv[0]);
								exit(2);
							}
						}
						pipeNum++;	
					}	
					else{
						
						if((dup2(fd[pipeNum *2],STDIN_FILENO)) == -1){
							(void)fprintf(stderr,"ERROR: Pipe input redirection failure.\n");
							exit(7);
						}
						if((dup2(fd[(pipeNum*2)-1],STDOUT_FILENO)) == -1){
							(void)fprintf(stderr,"ERROR: Pipe output redirection failure.\n");
							exit(8);
						}
						for(i = 0; i <= pipeNum*2 +1; i++){
							close(fd[i]);
						}	
						if((execvp(newargv[pipeIndex[pipeFlag - pipeNum-1]],newargv + pipeIndex[pipeFlag - pipeNum - 1])) == -1){
							(void)fprintf(stderr,"ERROR:'%s' Execution has failed.\n", newargv[pipeIndex[pipeFlag - pipeNum - 1]]);
							exit(1);
						}	
					}	
				}	
			}
			else{
				if(outputFile != -1){
					if((dup2(outputFile,STDOUT_FILENO)) == -1){
						(void)fprintf(stderr,"ERROR: Output redirection has failed.\n");
						output = NULL;
						outputFile = -1;	
						exit(3);	
					}
					output = NULL;
					close(outputFile);	
				}	
				if((dup2(fd[0],STDIN_FILENO)) == -1){
					(void)fprintf(stderr,"ERROR: Rightmost pipe input redirection has failed.\n");	
					exit(4);
				}
				close(fd[0]);
				close(fd[1]);	
				if(execvp(newargv[pipeIndex[pipeFlag -1]], newargv + pipeIndex[pipeFlag -1]) == -1){
					(void)fprintf(stderr,"ERROR: '%s' Execution has failed.\n",newargv[pipeIndex[pipeFlag-1]]);
					exit(4);
				}	
			}	
		}

		else{
			if(backGroundFlag == 1){
				printf("%s [%d}\n",newargv[0],kidpid);
				backGroundFlag = 0;
				return 0;
			}
			else{
				do{
					pid = wait(NULL);
				}while(pid != kidpid);	
				remove(hereIsWord);	
				return 0;
			}		
		}
	}
