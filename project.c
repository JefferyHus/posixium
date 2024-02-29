#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

// define the size of arguments, command
#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 32
#define MAX_ARG_SIZE 64

// command functions
void _touch(char* args);

// command function pointer
typedef void (*command_func_t)(char* args);

// file structure
typedef struct {
  wchar_t *name;
  wchar_t *content;
  float size;
} File;

// command structure
typedef struct {
  char* name;
  command_func_t fn;
} Command;

// command functions implementation
void _touch(char* args) {
  char* filename = strtok(args, " ");
  char* option = strtok(NULL, " ");
  char* value = strtok(NULL, " ");

  printf("filename %s, the option %s, and the value %s", filename, option, value);
}

// list of commands
Command commands[] = {
  {"touch", _touch},
  {NULL, NULL}
};

// command finder function
command_func_t find_command_func(const char* cmd) {
  for(int i = 0; commands[i].name != NULL; i++) {
    if (strcmp(commands[i].name, cmd) == 0) {
      return commands[i].fn;
    }
  }

  return NULL;
}

int main() {
  char input[MAX_INPUT_SIZE];
  char arguments[MAX_INPUT_SIZE];
  int num_args = 0;

  while (1)
  {
    printf("> ");
    fflush(stdout);

    // read the user input into the command buffer
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
      exit(EXIT_FAILURE);
    }

    // tokenize the input using the space
    char* token = strtok(input, " \n");

    // if the exit command is entered, break the loop
    if (strcmp(token, "exit") == 0) {
      break;
    }

    // get the user command
    command_func_t function = find_command_func(token);

    if (function == NULL) {
      fprintf(stderr, "The command you entered is unkown to me!!", NULL, 30);
    } else {
      strncpy(arguments, token + strlen(token) + 1, MAX_INPUT_SIZE - 1);
      arguments[MAX_INPUT_SIZE - 1] = '\0';
      function(arguments);
    }
  }

  return 0;
};