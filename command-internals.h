// UCLA CS 111 Lab 1 command internals

enum command_type
  {
    AND_COMMAND,         // A && B
    SEQUENCE_COMMAND,    // A ; B
    OR_COMMAND,          // A || B
    PIPE_COMMAND,        // A | B
    SIMPLE_COMMAND,      // a simple command
    SUBSHELL_COMMAND,    // ( A )
  };
  
  typedef enum token_type
  {
      WORD,
      AND,
      OR,
      PIPE,
      OPEN_P,  // '('
      CLOSE_P, // ')'
      IP_REDIRECT,
      OP_REDIRECT,
      SEMICOLON,
      NEW_LINE,
      END
  }token_t;

// Data associated with a command.
struct command
{
  enum command_type type;

  // Exit status, or -1 if not known (e.g., because it has not exited yet).
  int status;

  // I/O redirections, or 0 if none.
  char *input;
  char *output;

  union
  {
    // for AND_COMMAND, SEQUENCE_COMMAND, OR_COMMAND, PIPE_COMMAND:
    struct command *command[2];

    // for SIMPLE_COMMAND:
    char **word;

    // for SUBSHELL_COMMAND:
    struct command *subshell_command;
  } u;
};

struct command_stream{
    //To read characters
    int (*getbyte) (void*);
    void* arg;
    
    //next character
    char next;
    
    //line number
    int line;
    
    //The current token
    char*   curr_str;
    token_t curr_tkn;
    
    //The next token
    char*   next_str;
    token_t next_tkn;
    
    //Length of a token string
    int token_s;
};
