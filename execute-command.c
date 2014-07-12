// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <time.h>
#include <sys/unistd.h>
#include <sys/wait.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

void execute_command(command_t, int);

int
command_status(command_t c) {
    return c->status;
}

void
execute_and(command_t c) {
    execute_command(c->u.command[0]); //Execute A
    if (c->u.command[0]->status == 0) { //If A succeeded
        execute_command(c->u.command[1]); //Execute B
        c->status = c->u.command[1]->status; //Return status of B
    } else { //If A failed
        c->status = c->u.command[0]->status; //Return status of A
    }
}

void
execute_or(command_t c) {
    execute_command(c->u.command[0]); //Execute A
    if (c->u.command[0]->status != 0) { //If A failed
        execute_command(c->u.command[1]); //Execute B
        c->status = c->u.command[1]->status; //Return status of B
    } else { //If A succeeded
        c->status = c->u.command[0]->status; //Return status of A
    }
}

void
execute_sequence(command_t c) {
    int status;
    pid_t pid = fork();
    if (pid > 0) {
        waitpid(pid, &status, c);
        c->status = status;
    } else if (pid == 0) {
        pid = fork();
        if (pid > 0) {
            waitpid(pid, &status, c);
            execute_command(c->u.command[1]);
            _exit(c->u.command[1]->status);
        } else if (pid == 0) {
            execute_command(c->u.command[0]);
            _exit(c->u.command[0]->status);
        } else {
            error(1,0, "Could not fork");
        }
    } else {
        error(1,0,"Could not fork");
    }
}

void
execute_pipe(command_t c) {
}

void
execute_io(command_t c) {
}

void
execute_command(command_t c) {
    switch (c->type) {
        case AND_COMMAND: execute_and(c);
            break;
        case OR_COMMAND: execute_or(c);
            break;
        case SEQUENCE_COMMAND: execute_sequence(c);
            break;
        case PIPE_COMMAND: execute_pipe(c);
            break;
        case SIMPLE_COMMAND:
        case SUBSHELL_COMMAND:execute_io(c);
            break;
        default: error(1, 0, "Invalid command type");
    }
}
