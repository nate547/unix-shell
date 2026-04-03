// ########################################
//          Dynamic Input Reading
// ########################################

// Pseudo Code |
//             V

// Allocate initial buffer
// Used = 0
/*
do {
    (read buffer + size,
    define max size as total size,
    update used var by how many chars read)
    if \n found -> break
    else -> realloc buffer for more space
} while (1)
*/

#include <stdio.h> // printf, fgets, perror
#include <stdlib.h> // malloc/realloc, free, exit
#include <string.h> // strlen
#include <unistd.h> // fork, execvp
#include <sys/types.h> // pid_t
#include <sys/wait.h> // wait

char *read_line(void) {
// This is our initialization of the buffer
    size_t bufsize = 64;
    size_t used = 0;
    char *buffer = malloc(bufsize);
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    

    while (1) {
        if (fgets(buffer + used, bufsize - used, stdin) == NULL) {
            free(buffer);
            return NULL; // EOF or error
        }
        
// Update our used variable to allocate more space
        used += strlen(buffer + used); 
        
// If buffer contains \n we know we don't have to read more input so no realloc required
        if (buffer[used - 1] == '\n') {
            buffer[used - 1] = '\0';
            return buffer;
        }
        
// Handles the actual reallocation
        bufsize *= 2;
        char *new_buf = realloc(buffer, bufsize);
        if (!new_buf) {
            free(buffer);
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        buffer = new_buf;
    }
}

// ########################################
//              Command Parser
// ########################################

// Pseudo Code |
//             V

// create empty list argv
// set i = 0

/* 
function parse_line(line) {
    while (i < length of line) {
        skip any space with pointer
        if we reach a '\n' {
            break
        }
        
        mark start of word = i
        
        move i forward until we hit a space or end of line
        replace space with null term if ' '
        
        add pointer to argv
    }
    
    add null term to argv
    return argv
}
*/

char **parse_line (char *line) {
    const size_t MAX_ARGS = 32;
    char **argv = malloc(sizeof(char*) * MAX_ARGS);
    int arg_index = 0;
    int i = 0;
    
    while (line[i] != '\0') {
// Skips leading spaces
        while (line[i] == ' ') i++;
        
// Breaks while if end is reached
        if (line[i] == '\0') break;
        
// Mark start of word
        int start = i;
        
// Move i to the end of word
        while (line[i] != ' ' && line [i] != '\0') i++;
        
// Replace space with null term
        if (line[i] == ' ') {
            line[i] = '\0';
            i++;
        }
      
// Store pointer to start of word  
        argv[arg_index++] = &line[start];
    }
    
// Null term argv
    argv[arg_index] = NULL;
    return argv;
}
    
// ########################################
//                  MAIN
// ########################################

int main() {
// Shell runtime loop
    while (1) {
        printf("> ");
        fflush(stdout);

// read_line call
        char *line = read_line();
        if (!line) break;

// Call parse_line to tokenize args
        char **argv = parse_line(line);
        if (argv[0] == NULL) {
            free(line);
            free(argv);
            continue;
        }

// Command execution
        pid_t pid = fork();
// Is process current?
        if (pid == 0) {
            execvp(argv[0], argv);
            perror("execvp");
            exit(EXIT_FAILURE);
// If not queue process until current
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("fork");
        }

// Free memory
        free(line);
        free(argv);
    }
    printf("\nExiting Shell. \n");
    return 0;  
}
