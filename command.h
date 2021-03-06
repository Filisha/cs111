// UCLA CS 111 Lab 1 command interface

////////////////////////////////////
//Authors:
//  Shubham Joshi   :   504-134-654
//  Derek Foster    :   504-146-063
////////////////////////////////////

typedef struct command *command_t;
typedef struct command_stream *command_stream_t;

typedef int bool;
#define TRUE 1
#define FALSE 0

/* Create a command stream from LABEL, GETBYTE, and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.*/
void execute_command (command_t);

/*Execute in time travel mode*/
void execute_command_t(command_t);

/* Return the exit status of a command, which must have previously been executed.
   Wait for the command, if it is not already finished.  */
int command_status (command_t);
