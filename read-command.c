// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdio.h>

//When allocating memory, do so in blocks
#define BLOCK_SIZE 16

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream(int (*get_next_byte) (void *),
        void *get_next_byte_argument) {
    /* FIXME: Replace this with your implementation.  You may need to
       add auxiliary functions and otherwise modify the source code.
       You can also use external functions defined in the GNU C Library.
    error (1, 0, "command reading not yet implemented");
    return 0;*/

    size_t buffer_s = 1024; //Hold these many characters
    int buffer_pos = 0;

    char* input_buf = checked_malloc(buffer_s * sizeof (char));
    memset(input_buf, '\0', buffer_s * sizeof (char));

    command_stream_t stream = checked_malloc(sizeof (struct command_stream));
    stream->stream_s = 0;
    stream->pos = 0;
    stream->commands = checked_malloc(BLOCK_SIZE * sizeof (command_t));
    char curr;
    while((curr = get_next_byte(get_next_byte_argument)) != EOF)
    {
        //safely add character to buffer
        if(buffer_pos == buffer_s){
            buffer_s += 1024;
            checked_realloc(input_buf, buffer_s);
        }
        input_buf[buffer_pos] = curr;
    }
    
    //DO NOT DELETE!
    free(input_buf);
    return 0;
}

command_t
read_command_stream(command_stream_t s) {
    /* FIXME: Replace this with your implementation too.  */
    error(1, 0, "command reading not yet implemented");
    return 0;
}
