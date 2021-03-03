#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <signal.h> 
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define READ 0
#define WRITE 1

int fg = 0;
pid_t pid1, pid2, pid3;

int getarg(char *cmd, char **argv)
{ //divide input cmd
	int arg = 0;
	while(*cmd) {
		if(*cmd == ' ' || *cmd == '\t')
			*cmd++ = '\0';
		else {
			argv[arg++] = cmd++;
			while(*cmd != '\0' && *cmd != ' ' && *cmd != '\t')
				cmd++;
		}
	}
	argv[arg] = NULL;
	return arg;
}

int selectOption(char *argv)
{ //check shell cmd option
	int option = 0;

	if(argv == '\0') {
		return option;
	}
	
	for(int i = 0; argv[i] != '\0'; i++) {
		if(argv[i] == '&'){
			option = 1;
			return option;
		}

		if(argv[i] == '|'){
			option = 2;
			return option;
		}

		if(argv[i] == '<'){
			option = 3;
			return option;
		}
		
		if(argv[i] == '>'){
			option = 4;
			return option;
		}
	}
	return option;
}

void shellpipe(char **argv)
{ //pipe function
	char *cmd1, *cmd2;
	int fd[2];

	for(int i = 0; argv[i] != '\0'; i++) {
		if(argv[i] == '|'){
			cmd1 = argv[i-1];
			cmd2 = argv[i+1];
		}
	}

	pipe(fd);

	if(fork() == 0) {
		close(fd[READ]);
		dup2(fd[WRITE],1);
		close(fd[WRITE]);
		execlp(cmd1, cmd1, NULL);
		perror("pipe");
	}

	else {
		close(fd[WRITE]);
		dup2(fd[READ],0);
		close(fd[READ]);
		execlp(cmd2, cmd2, NULL);
		perror("pipe");
	}
}


void cmd_ls() 
{ // ls function
	DIR *dp;
	struct dirent *d;
	char *dir;
	int i = 0;
	
	dir = ".";
	
	if((dp = opendir(dir)) == NULL)
		perror(dir);

	while((d = readdir(dp)) != NULL)
		printf("%-3s	", d->d_name);
	
	printf("\n");
	closedir(dp);
}

void cmd_mkdir(char *path)
{ // mkdir function
	if(mkdir(path, 0777) < 0)
		perror("no mkdir");
}

void cmd_rmdir(char *file)
{ // rmdir function
	if(rmdir(file) < 0)
		perror("no rmdir");
}

void cmd_cat(char *file)
{ // cat function
	char buf[1024];
	int files;

	if((files = open(file, O_RDONLY)) == -1) {
		printf("file open error\n");
		exit(1);
	}
	
	while(read(files, buf, 1024) > 0) {
		printf("%s", buf);
	}
}

void do_cmd(int i, char **argv)
{ // do cmd line
	if(!strcmp(argv[i], "cat")){
		if(argv[i+1] == NULL)
			fprintf(stderr, "cmd : cat error\n");		
		else {
			int arg = argv[i+1][0] - 48;
			cmd_cat(arg);
		}
	}

	else if(!strcmp(argv[i], "mkdir")){
		if(argv[i+1] == NULL)
			fprintf(stderr, "cmd : mkdir error\n");
		else
			cmd_mkdir(argv[i+1]);
	}

	else if(!strcmp(argv[i], "rmdir")){
		if(argv[i+1] == NULL)
			fprintf(stderr, "cmd : rmdir error\n");
		else
			cmd_rmdir(argv[i+1]);
	}

	else if(!strcmp(argv[i], "ls")){
		if(argv[i+1] == NULL)
			fprintf(stderr, "cmd : ls error\n");
		else
			cmd_ls();
	}
}

void main()
{
	char buf[1024], *argv[100];
	int arg, amper;
	FILE *fd = NULL;
	int filedes[2];

	printf("Welcome to shell\n");
		
	while(1)
	{
		printf("shell> ");
		fgets(buf, sizeof(buf), stdin);
		arg = getarg(buf, argv);
		int option = 0;

		for(int i = 0; i < arg; i++) { 
			if(!strcmp(argv[i], "exit\n")){ //exit, logout
				printf("close the shell\n");
				exit(1);
				break;
			}

			if(!strcmp(argv[i], "logout\n")){
				printf("close the shell\n");
				exit(1);
				break;
			}
			
			int op = selectOption(argv[i+1]); //check cmd option
			
			if(op == 1) //background and foreground check
				amper = 1;
			else
				amper = 0;

			if((pid1 = fork()) == 0) { //fork(), child 
				if(op == 4) { //redirection output
					fd = creat("./test.txt", 0644);
					close(stdout);
					dup(fd);
					close(fd);
				}

				if(op == 3){ // redirection input
					
				}
			}

			else { //fork(), parent
				if(amper == 0) { //background
					while(fg=1)
						pause();
				}
				else //foreground
					break;
			}

			if(op == 2) { // pipe
				if(pid2 = fork()) == 0) { //grandchild
					close(stdout);
					dup(filedes[1]);
					close(filedes[1]);
					close(filedes[0]);
					execlp(argv[i], argv[i], NULL);	
				}
				else {
					close(stdin);
					dup(filedes[0]);
					close(filedes[0]);
					close(filedes[1]);
					execlp(argv[i+1], argv[i+1], NULL);
				}
			}
		
			else // no pipe
				do_cmd(i,argv);

		}

	}		
}
