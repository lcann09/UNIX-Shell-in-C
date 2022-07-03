#include<stdbool.h> 
#include<stdio.h> 
#include<stdlib.h> 
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<limits.h>
#include<signal.h>

int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);
int sh_pwd(char **args);
int sh_set(char **args);
int sh_unset(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "pwd",
    "set",
    "unset"
};

char *builtin_desc[] = {
    "cd [dir] \n-Changes SEEsh’s working directory to dir or to the HOME directory if dir is unspecified.",
    "help \n-Displays information about builtin commands",
    "exit \n-Causes SEEsh to exit.",
    "pwd \n-Prints the full filename of the current working directory.",
    "set var [value] \n-If environment variable var does not exist it is created. Sets var to value or \"\" if value unspecified. If var and value are both unspecified, lists environment variables.",
    "unset var \n-Destroys the environment variable var"
};

int (*builtin_func[]) (char **) = {
    &sh_cd,
    &sh_help,
    &sh_exit,
    &sh_pwd,
    &sh_set,
    &sh_unset
};
 
int sh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

int sh_cd(char **args)
{
    if (args[1] == NULL){
 	if (chdir(getenv("HOME")) != 0) {
	    perror("SEEsh");
	}
    } else {
	if (chdir(args[1]) != 0) {
	    perror("SEEsh");
	}
    }
    return 1;
}

int sh_help(char **args)
{
    int i;
    printf("Built in commands: \n");

    for (i=0; i<sh_num_builtins(); i++){
	printf("%s\n", builtin_desc[i]);
    }
    return 1;
}

int sh_exit(char **args)
{
    return 0;
}

int sh_pwd()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL){
 	printf("%s\n", cwd);
    } else {
	perror("SEEsh");
    }
    return 1;
}

extern char **environ;

int sh_set(char **args)
{
    if (args[1] == NULL){
	for(char **current = environ; *current; current++){
	    printf("%s\n", *current);
	}
    } else {
	if (args[2] !=NULL){
	    if(setenv(args[1], args[2], 1) != 0) {
	        perror("SEEsh");
	    }
	} else {
	    if(setenv(args[1], "", 1) != 0) {
	        perror("SEEsh");
	    }
	}
    }
    return 1;
}

int sh_unset(char **args)
{
    if (args[1] == NULL){
 	fprintf(stderr, "SEEsh: expected argument to \"unset\"\n");
    } else {
	if (unsetenv(args[1]) != 0) {
	    perror("SEEsh");
	}
    }
    return 1;
}

pid_t pid = -1;

void handle_sigint(int sig){
    kill(pid, SIGKILL);
    printf("\n");
}

int sh_launch(char **args)
{
    pid_t wpid;
    int status;
    pid = fork();

    signal(SIGINT, (void (*)(int))handle_sigint);
    if (pid == 0){
	if (execvp(args[0],args) == -1){
	    perror("SEEsh");
	}
	exit(EXIT_FAILURE);
    } else if (pid < 0){
	perror("SEEsh");
    } else{
	do{
	    wpid = waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    signal(SIGINT, SIG_IGN);
    return 1;
}

int sh_execute(char **args)
{   
    int i;
    if (args[0] == NULL){
	return 1;
    }

    for (i=0; i< sh_num_builtins(); i++){
	if(strcmp(args[0], builtin_str[i]) ==0){
	    return (*builtin_func[i])(args);
	}
    }
    return sh_launch(args);
}

#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"
char **sh_split_line(char *line)
{
    int bufsize = SH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if(!tokens){
	fprintf(stderr, "SEEsh: allocation error\n");
	exit(EXIT_FAILURE);
    }

    token = strtok(line, SH_TOK_DELIM);
    while(token != NULL){
	tokens[position] = token;
	position++;
	if(position >= bufsize){
	    bufsize += SH_TOK_BUFSIZE;
	    tokens = realloc(tokens, bufsize * sizeof(char*));
	    if(!tokens){
		fprintf(stderr, "SEEsh: allocation error\n");
		exit(EXIT_FAILURE);
	    }
	}
        token = strtok(NULL, SH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

char *sh_read_line(void)
{
    char *line = NULL;
    size_t bufsize = 0;
    ssize_t read = getline(&line, &bufsize, stdin);
    if( read==-1){
       exit(0);
    }
    return line;
}

void sh_loop(void)
{
    char *line;
    char **args;
    int status;
    signal(SIGINT, SIG_IGN);
    do {
	printf("? ");
	line = sh_read_line();
	args = sh_split_line(line);
    	status = sh_execute(args);
    	free(line);
    	free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    char *filename = strcat(getenv("HOME"), "/.SEEShrc");
    printf("%s\n",filename);
    FILE *file = fopen(filename, "r");
    char line[512];
    if(file){
        while (fgets(line, sizeof(line), file)){
	    printf("%s",line);
            sh_execute(sh_split_line(line));
        }
	printf("\n");
    }
    fclose(file);

    sh_loop();

    return EXIT_SUCCESS;
}


