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

//Read the next token
//But do not pull it from the stream
#define PEEK(s) (s->next_tkn)

//read a character from the stream

char read(command_stream_t s) {
    char curr;
    //If the stream has a character, consume it
    if (s->next != NO_CHAR) {
        curr = s->next;
        s->next = NO_CHAR;
    }//Else directly read it in from getbyte
    else
        curr = s->getbyte(s->arg);
    return curr;
}

void reallocate(command_stream_t s) {
    s->token_s += 64;

    s->curr_str = checked_realloc(s->curr_str, s->token_s * sizeof (char));
    s->next_str = checked_realloc(s->next_str, s->token_s * sizeof (char));
}

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

bool iswordcharacter(char c) {
    bool isnum = '0' <= c && c <= '9';
    bool isloweralpha = 'a' <= c && c <= 'z';
    bool isupperalpha = 'A' <= c && c <= 'Z';
    bool ispunct = c == '!' || c == '+' || c == '%' || c == '^' || c == '.' ||
            c == '-' || c == ',' || c == '@' || c == '/' ||
            c == ':' || c == '_';

    return isnum || isloweralpha || isupperalpha || ispunct;
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

        //Break at end of file
        if (curr == EOF || curr == NO_CHAR) {
            s->next_tkn = END;
            break;
        }
        
        //Ignore leading whitespace
        else if (curr == ' ' || curr == '\t') continue;

        //Ignore comments
        else if (curr == '#') {
            while ((curr = read(s)) != '\n') continue;
            s->next = curr; //do not ignore the newline character
        }
        
        //Read a word
        else if (iswordcharacter(curr)) {
            next[pos] = curr;
            pos++;
            //Consume characters until token ends
            while (char = read(s)) {
                if (!iswordcharacter(curr)) {
                    s->next = curr; //do not skip the current character
                    break;
                }
                if (pos >= s->token_s) {
                    reallocate(s);
                    next = s->next_str;
                }
                next[pos] = curr;
                pos++;
            }
            //Reallocate if necessary, and add the terminal 0
            if (pos >= s->token_s) {
                reallocate(s);
                next = s->next_str;
            }
            next[pos] = 0;
            s->next_tkn = WORD;
            break;
        }
        
        //Read an AND (&&) command
        else if (curr == '&') {
            next[pos] = curr;
            pos++;
            if ((curr = read(s)) != '&') {
                error(1, 0, "%d: Found invalid character, lone &");
                break;
            } else {
                next[pos] = curr;
                pos++;
                next[pos] = 0;
                s->next_tkn = AND;
                break;
            }
        }
    }
}

command_t
read_command_stream(command_stream_t s) {
    if (PEEK(s) == END)
        return NULL;
    return 0;
}
