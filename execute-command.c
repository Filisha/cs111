// UCLA CS 111 Lab 1 command execution

////////////////////////////////////
//Authors:
//  Shubham Joshi   :   504-134-654
//  Derek Foster    :   504-146-063
////////////////////////////////////

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <errno.h>
#include <time.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

void execute_command(command_t);

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
        waitpid(pid, &status, 0);
        c->status = status;
    } else if (pid == 0) {
        pid = fork();
        if (pid > 0) {
            waitpid(pid, &status, 0);
            execute_command(c->u.command[1]);
            _exit(c->u.command[1]->status);
        } else if (pid == 0) {
            execute_command(c->u.command[0]);
            _exit(c->u.command[0]->status);
        } else {
            error(1, 0, "Could not fork");
        }
    } else {
        error(1, 0, "Could not fork");
    }
}

void set_io_redir(command_t c) {
    if (c->input != NULL) {
        int in = open(c->input, O_RDWR);
        if (in < 0) error(1, 0, "Could not open file: %s", c->input);
        if (dup2(in, 0) < 0) error(1, 0,
                "Problem using dup2 with file: %s", c->input);
        if (close(in) < 0) error(1, 0, "Could not close file: %s", c->input);
    }
    if (c->output != NULL) {
        int out = open(c->input, O_CREAT | O_WRONLY | O_TRUNC,
                S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        if (out < 0) error(1, 0, "Could not open file: %s", c->output);
        if (dup2(out, 1) < 0) error(1, 0,
                "Problem using dup2 with file: %s", c->output);
        if (close(out) < 0) error(1, 0, "Could not close file: %s", c->output);
    }
}

void
execute_pipe(command_t c) {
    int status;
    int buf[2];
    pid_t ret, pid_1, pid_2;
    if (pipe(buf) == -1) {
        error(1, 0, "Can not create Pipe");
    }
    pid_1 = fork();
    if (pid_1 > 0) {
        pid_2 = fork();
        if (pid_2 > 0) {
            close(buf[0]);
            close(buf[1]);
            ret = waitpid(-1, &status, 0);
            if (ret == pid_1) {
                c->status = status;
                waitpid(pid_2, &status, 0);
                return;
            } else if (ret == pid_2) {
                waitpid(pid_1, &status, 0);
                c->status = status;
                return;
            }
        } else if (pid_2 == 0) {
            close(buf[0]);
            if (dup2(buf[1], 1) < 0) error(1, errno, "dup2 error");
            execute_command(c->u.command[0]);
            _exit(c->u.command[0]->status);
        } else error(1, 0, "Could not fork");
    } else if (pid_1 == 0) {
        close(buf[1]);
        if (dup2(buf[0], 0) < 0) error(1, errno, "dup2 error");
        execute_command(c->u.command[1]);
        _exit(c->u.command[1]->status);
    } else error(1,0, "Could not fork");
}

void
execute_simple(command_t c){
    int status;
    pid_t pid = fork();
    if(pid > 0){
        waitpid(pid, &status, 0);
        c->status = status;
    } else if(pid == 0){
        set_io_redir(c);
        if(c->u.word[0][0] == ':')
            _exit(0);
        execvp(c->u.word[0], c->u.word);
    } else error(1,0,"Could not fork");
}

void
execute_io(command_t c) {
    if(c->type == SIMPLE_COMMAND) execute_simple(c);
    else if(c->type == SUBSHELL_COMMAND){
        set_io_redir(c);
        execute_command(c->u.subshell_command);
    } else error(1,0,"Error processing I/O command");
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
