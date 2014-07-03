// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

//An invalid character to use as a null value
#define NO_CHAR (-5)

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream(int (*get_next_byte) (void *),
        void *get_next_byte_argument) {
    command_stream_t stream = checked_malloc(sizeof (struct command_stream));

    //Set up input
    stream->getbyte = get_next_byte;
    stream->arg = get_next_byte_argument;

    //Set up defaults
    stream->next = NO_CHAR;
    stream->line = 1;

    //Set up token strings
    stream->curr_str = checked_malloc(64 * sizeof (char));
    stream->curr_str[0] = 0;
    stream->next_str = checked_malloc(64 * sizeof (char));
    stream->next_str[0] = 0;
    stream->token_s = 64;
    //When we're finally done, we can return stream
    return stream;
}



char read(command_stream_t s) {
    char curr;
    //If the stream has a character, consume it
    if (s->next != NO_CHAR) {
        curr = s->next;
        s->next = NO_CHAR;
    }
    //Else directly read it in from getbyte
    else
        curr = s->getbyte(s->arg);
    return curr;
}

command_t next(command_stream_t s) {
    char* next = s->next_str;

    //Move the next token to the current one
    strcpy(s->curr_str, next);
    s->curr_str = s->next_str;
    s->next_str[0] = 0;

    //Data read in
    char curr;
    int pos = 0;

    while (TRUE) {
        //Read the next character
        curr = read(s);

        if(curr == EOF || curr == NO_CHAR){
            s->next_tkn = END;
        }
        //Ignore leading whitespace
        else if (curr == ' ' || curr == '\t') continue;
        else if (curr == '#'){
            //ignore comments
            while((curr = read(s)) != '\n') continue;
            s->next = curr;//do not ignore the newline character
        }
    }
}

//Read the next token
//But do not pull it from the stream
#define PEEK(s) (s->next_tkn)
command_t
read_command_stream(command_stream_t s) {
    if (PEEK(s) == END)
        return NULL;
    return 0;
}
