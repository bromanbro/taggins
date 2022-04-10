#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <unistd.h>

#define ATTR "user.tags"
#define MAX_TAGS 3
#define MAX_TAG_SIZE 50
#define BUFFER_SIZE MAX_TAGS * MAX_TAG_SIZE

enum {
  READ,
  ADD,
  REMOVE
};

int readTags(char *);
int addTags(char *, int, char *[]);
int removeTags(char *, int, char *[]);
int processPath(char *, int, char *[], int);

int main(int argc, char *argv[])
{
  int mode = READ;
  int opt;
  char *path = NULL;
  while ((opt = getopt(argc, argv, ":arf:")) != -1) {
    switch (opt) {
    case 'a':
      mode = ADD;
      break;
    case 'r':
      mode = REMOVE;
      break;
    case 'f':
      path = optarg;
      break;
    }
  }

  if (path != NULL) {
    if (processPath(path, argc, argv, mode) < 0) {
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }

  char *line = NULL;
  size_t len = 0;
  while (getline(&line, &len, stdin) != -1) {
    if (processPath(line, argc, argv, mode) < 0) {
      free(line);
      exit(EXIT_FAILURE);
    }
  }
  free(line);
  exit(EXIT_SUCCESS);
}

int readTags(char *path)
{
  char temp[BUFFER_SIZE];
  printf("%s ", path);
  int result = getxattr(path, ATTR, temp, BUFFER_SIZE);
  if (result < 0) {
    if (errno == ENOTSUP) {
      fprintf (stderr, "Extended attributes are not available on your filesystem.\n");
      return -1;
    }
    printf("\n");
  } else {
    printf("%s\n", temp);
  }
  return 1;
}

int addTags(char *path, int argc, char *argv[])
{
  char temp[BUFFER_SIZE];
  char tempForWrite[BUFFER_SIZE];
  tempForWrite[0] = '\0';
  int result = getxattr(path, ATTR, temp, BUFFER_SIZE);
  if (result < 0) {
    if (errno == ENOTSUP) {
      fprintf (stderr, "Extended attributes are not available on your filesystem.\n");
      return -1;
    }
  }
  int numberOfExistingTags = 0;
  char *existingTags[MAX_TAGS];
  if ((result != -1) && (strlen(temp) > 0)) {
    strcat(tempForWrite, temp);
    strcat(tempForWrite, " ");
    char *p = strtok(temp, " ");
    while (p != NULL) {
      existingTags[numberOfExistingTags++] = p;
      p = strtok(NULL, " ");
    }
  }
  for (int i = optind; i < argc; i++) {
    int skip = 0;
    for (int j = 0; j < numberOfExistingTags; j++) {
      if (strcmp(existingTags[j], argv[i]) == 0) {
        skip = 1;
        break;
      }
    }
    if (skip != 1) {
      existingTags[numberOfExistingTags++] = argv[i];
      strcat(tempForWrite, argv[i]);
      strcat(tempForWrite, " ");
    }
  }
  if (numberOfExistingTags > MAX_TAGS) {
    fprintf(stderr, "This operation would exceed the maximum %i tags.\n", MAX_TAGS);
    return -1;
  }
  tempForWrite[strlen(tempForWrite) - 1] = '\0';
  setxattr(path, ATTR, tempForWrite, strlen(tempForWrite) + 1, 0);
  printf("%s %s\n", path, tempForWrite);
  return 1;
}

int removeTags(char *path, int argc, char *argv[])
{
  int numberOfExistingTags = 0;
  char temp[BUFFER_SIZE];
  char tempForWrite[BUFFER_SIZE];
  char *existingTags[MAX_TAGS];

  tempForWrite[0] = '\0';
  int result = getxattr(path, ATTR, temp, BUFFER_SIZE);
  if (result < 0) {
    if (errno == ENOTSUP) {
      fprintf (stderr, "Extended attributes are not available on your filesystem.\n");
      return -1;
    }
  }
  if (result != -1) {
    char *p = strtok(temp, " ");
    while (p != NULL) {
      existingTags[numberOfExistingTags++] = p;
      p = strtok(NULL, " ");
    }
  }
  for (int j = 0; j < numberOfExistingTags; j++) {
    int skip = 0;
    for (int i = optind; i < argc; i++) {
      if (strcmp(existingTags[j], argv[i]) == 0) {
        skip = 1;
        break;
      }
    }
    if (skip != 1) {
      strcat(tempForWrite, existingTags[j]);
      strcat(tempForWrite, " ");
    }
  }
  tempForWrite[strlen(tempForWrite) - 1] = '\0';
  setxattr(path, ATTR, tempForWrite, strlen(tempForWrite) + 1, 0);
  printf("%s %s\n", path, tempForWrite);
  return 1;
}

int processPath(char *path, int argc, char *argv[], int mode)
{
  switch (mode) {
  case READ:
    return readTags(path);
  case ADD:
    return addTags(path, argc, argv);
  case REMOVE:
    return removeTags(path, argc, argv);
  }
}
