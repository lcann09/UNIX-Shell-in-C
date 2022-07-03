# UNIX Shell in C

A minimal interactive UNIX shell called SEEsh made in C based on Operating Systems course project. 

The shell interprets six shell built-in commands: cd, help, exit, pwd, set, unset.
• cd [dir]: change SEEsh’s working directory to dir, or to the HOME directory if dir is omitted.
• pwd: print the full filename of the current working directory.
• help: display information about builtin commands.
• exit: SEEsh exits.
• set var [value]: If environment variable var does not exist, then SEEsh creates it. SEEsh sets the value of var to value, or to the empty string if value is omitted. If both var and value are omitted then display all environment variables and values. Note: Initially, SEEsh inherits environment variables from its parent. 
• unset var: destroy the environment variable var.

If the command is not an SEEsh built-in, then SEEsh considers the command-name to be the name of a file that contains executable binary code. SEEsh uses the PATH environment variable to locate the binary, fork a child process and pass the filename, along with its arguments, to the execvp system call. If the attempt to execute the file fails, then SEEsh prints an error message indicating the reason for the failure.
