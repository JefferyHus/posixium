#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <sys/stat.h>

// define the size of arguments, command
#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 32
#define MAX_ARG_SIZE 64

char *sanitize_string(const char *str) {
  const char* whitelist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.";
  char* output = malloc(strlen(str) + 1);
  char* pointer = output;

  for (const char *c = str; *c != '\0'; c++) {
    if (strchr(whitelist, *c) != NULL) {
      *pointer = *c;
      pointer++;
    }
  }

  *pointer = '\0';

  return output;
}

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
  HANDLE file;
  SYSTEMTIME sysTime;
  FILETIME fileTime;

  char* filename = strtok(args, " ");
  char* option = strtok(NULL, " ");
  char* value = strtok(NULL, " ");

  // sanitize the filename, option and value
  filename = sanitize_string(filename);
  option = sanitize_string(option);
  value = sanitize_string(value);

  // create the file
  struct _stat fileinfo;
  DWORD disposition = CREATE_NEW;

  if (_stat(filename, &fileinfo) == 0) {
    disposition = OPEN_EXISTING;
  }

  file = CreateFile(filename, GENERIC_WRITE, 0, NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE) {
      fprintf(stderr, "Error: Unable to create file %s\n", filename);
      return;
  }

  if (option != NULL && strcmp(option, "--content") == 0) {
    DWORD bytesWritten;
    if (disposition == OPEN_EXISTING) {
      SetFilePointer(file, 0, NULL, FILE_END);
    }
    if (!WriteFile(file, value, strlen(value), &bytesWritten, NULL)) {
      fprintf(stderr, "Error: Unable to write to file %s\n", filename);
      return;
    }
    WriteFile(file, "\n", 1, &bytesWritten, NULL);
  }

  GetSystemTime(&sysTime);
  SystemTimeToFileTime(&sysTime, &fileTime);
  SetFileTime(file, &fileTime, &fileTime, &fileTime);

  CloseHandle(file);
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