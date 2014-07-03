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

token_t next(command_stream_t s) {
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
            while ((curr = read(s))) {
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
            next[pos] = '\0';
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
                next[pos] = '\0';
                s->next_tkn = AND;
                break;
            }
        }
        
        //Differentiate between PIPE (|) and OR (||) commands
        else if (curr == '|') {
            next[pos] = curr;
            pos++;
            if ((curr = read(s)) == '|') {
                next[pos] = curr;
                pos++;
                next[pos] = '\0';
                s->next_tkn = OR;
                break;
            } else {
                //Read a pipe, push the last character back
                s->next = curr;
                next[pos] = '\0';
                s->next_tkn = PIPE;
                break;
            }
        }
        
        //Open and close parenthesis
        else if (curr == '(') {
            next[pos] = curr;
            pos++;
            next[pos] = '\0';
            s->next_tkn = OPEN_P;
            break;
        } else if (curr == ')') {
            next[pos] = curr;
            pos++;
            next[pos] = '\0';
            s->next_tkn = CLOSE_P;
            break;
        }
        
        //IP/OP redirect
        else if (curr == '<') {
            next[pos] = curr;
            pos++;
            next[pos] = '\0';
            s->next_tkn = IP_REDIRECT;
            break;
        } else if (curr == '>') {
            next[pos] = curr;
            pos++;
            next[pos] = '\0';
            s->next_tkn = OP_REDIRECT;
            break;
        }
        
        //Semicolons
        else if (curr == ';') {
            next[pos] = curr;
            pos++;
            next[pos] = '\0';
            s->next_tkn = SEMICOLON;
            break;
        }
        
        //Newlines
        else if (curr == '\n') {
            s->line++;
            //Consume any subsequent newlines
            while ((curr == read(s)) == '\n') s->line++;

            if (curr == EOF) {
                s->next_tkn = END;
                break;
            }

            s->next = curr; //undo read of next line
            next[pos] = ' ';
            pos++;
            next[pos] = '\0';
            s->next_tkn = NEW_LINE;
            break;
        }
        
        //Any unrecognized character
        else {
            error(1, 0, "%d: Unrecognized or out of place character", s->line);
            break;
        }
    }
    return s->curr_tkn;
}

command_t parse_piped_command(command_stream_t s){
    return NULL;
}

command_t parse_and_or_command(command_stream_t s) {
    command_t first_c = parse_piped_command(s);
    
    while(PEEK(s) == AND || PEEK(s) == OR) {
        token_t curr = next(s);
    }
    
    return NULL;
}

command_t parse_complete_command(command_stream_t s){
    command_t first_c = parse_and_or_command(s);
    
    while(PEEK(s) == NEW_LINE || PEEK(s) == SEMICOLON) {
        next(s);
        if(PEEK(s) == END) break;
        command_t sequence_b = parse_and_or_command(s);
        command_t sequence_c = checked_malloc(sizeof(struct command));
        sequence_c->type = SEQUENCE_COMMAND;
        sequence_c->status = -1;
        sequence_c->input = 0;
        sequence_c->output = 0;
        sequence_c->u.command[0] = first_c;
        sequence_c->u.command[1] = sequence_b;
        first_c = sequence_c;
    }
    return first_c;
}

command_t parse_subshell_command(command_stream_t s){
    
    //Consume open parenthesis
    if(next(s) != OPEN_P) {
        error(1,0,"%d: Could not read script, expected \'(\'", s->line);
    }
    
    //Parse inner command
    command_t inner_c = parse_complete_command(s);
    
    //Consume newlines before close parenthesis
    while(PEEK(s) == NEW_LINE) next(s);
    
    //Consume close parenthesis
    if(next(s) != CLOSE_P) {
        error(1,0,"%d: Could not read script, expected \')\'", s->line);
    }
    
    //Now create the parsed command
    command_t command = checked_malloc(sizeof(struct command));
    command->type   = SUBSHELL_COMMAND;
    command->status = -1;
    command->input  = 0;
    command->output = 0;
    command->u.subshell_command = inner_c;
    
    return command;
}

//Parse commands
command_t parse(command_stream_t s) {
    while(PEEK(s) == NEW_LINE){
        next(s);
    }
    
    //Is it a subshell command?
    if(PEEK(s) == OPEN_P) {
        command_t subshell_c = parse_subshell_command(s);
        if(PEEK(s) == IP_REDIRECT) {
            //Consume the <
            next(s);
            if(next(s) != WORD) {
                error(1,0,"%d: Could not read script, expected input argument",
                        s->line);
            }
            subshell_c->input = checked_malloc(strlen(s->curr_str ) + 1);
            strcpy(subshell_c->input, s->curr_str);
        }
        if(PEEK(s) == OP_REDIRECT) {
            //Consume the >
            next(s);
            if(next(s) != WORD) {
                error(1,0,"%d: Could not read script, expected input argument",
                        s->line);
            }
            subshell_c->output = checked_malloc(strlen(s->curr_str ) + 1);
            strcpy(subshell_c->output, s->curr_str);
        }
        return subshell_c;
    }
    return NULL;
}

command_t
read_command_stream(command_stream_t s) {
    if (PEEK(s) == END)
        return NULL;
    next(s);
    return 0;
}
