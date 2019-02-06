#include <stdio.h>
#include <string.h>
#include <strings.h>
#define STORAGE 255   /* Allows for each arg to be 254 chars plus a null terminator */

int getword(char *w);

/* getword will act as our tokenizer for the shell, returning the number of characters in an arg. It will take args from stdin and
will work closely with Parse in order to act on args accordingly. Meta characters which are going to be detected include, ' ' , 
';' ,'EOF', '\n', '\',  '$', '>', '<', '<<', '|', and '~' . Meta chars ' ' , ';', 'EOF', and '\n' will all act as word terminators
(everything excluding a ' ' will act as a line terminator as well.)ungetc will be needed to redetect them after we have returned. 
'$' and '~' will only be treated as metacharacters if they are detected to be the firstchar of an arg. '\' will be used how it is
in traditional shells, it will negate anything that comes after it(including meta chars) and if a newline is directly after a \,
then we treat that as a space. A leading '$' will treat the arg as an environ var, while a leading '~' will be replaced with the 
path to the current users home directory. '<' and '>' will be used for input and output redirection, while '<<' will be used for
hereis functionality. Finally '|' will be used for pipeline functionality. */
