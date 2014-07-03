// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

//When allocating memory, do so in blocks
#define BLOCK_SIZE 16

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream(int (*get_next_byte) (void *),
        void *get_next_byte_argument) {
    command_stream_t stream = checked_malloc(sizeof(struct command_stream));
    
    //Set up input
    stream->getbyte = get_next_byte;
    stream->arg     = get_next_byte_argument;
    
    //Set up defaults
    stream->next = -2;
    stream->line = 1;
    
    //Set up token strings
    stream->curr_str = checked_malloc(64 * sizeof(char));
    stream->curr_str[0] = 0;
    stream->next_str = checked_malloc(64 * sizeof(char));
    stream->next_str[0] = 0;
    stream->token_s = 64;
    //When we're finally done, we can return stream
    return stream;
}

command_t
read_command_stream(command_stream_t s) {
    /* FIXME: Replace this with your implementation too.  */
    error(1, 0, "command reading not yet implemented");
    return 0;
}
