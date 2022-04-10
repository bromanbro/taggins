#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <unistd.h>

#define ATTR "user.tags"
#define MAX_TAGS 50
#define MAX_TAG_SIZE 50
#define BUFFER_SIZE MAX_TAGS * MAX_TAG_SIZE

enum {
  READ,
  ADD,
  REMOVE,
  REMOVE_ALL
};

int displayTags(char *);
int addTags(char *, int, char *[]);
int removeTags(char *, int, char *[]);
int removeAllTags(char *);
int processPath(char *, int, char *[], int);
int writeOutTags(char *, char *, int);
int readInTags(char *, char *);

int main(int argc, char *argv[])
{
  int mode = READ;
  int opt;
  char *path = NULL;
  while ((opt = getopt(argc, argv, ":ardf:")) != -1) {
    switch (opt) {
    case 'a':
      mode = ADD;
      break;
    case 'r':
      mode = REMOVE;
      break;
    case 'd':
      mode = REMOVE_ALL;
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

int displayTags(char *path)
{
  char temp[BUFFER_SIZE];
  printf("%s ", path);
  if (readInTags(path, temp) < 0) {
    printf("\n");
    return -1;
  }
  printf("%s\n", temp);
}

int addTags(char *path, int argc, char *argv[])
{
  char temp[BUFFER_SIZE];
  char tempForWrite[BUFFER_SIZE];
  tempForWrite[0] = '\0';
  int numberOfExistingTags = 0;
  char *existingTags[MAX_TAGS];

  int result = readInTags(path, temp);
  if (result < 0) {
    return -1;
  }
  else if (strlen(temp) > 0) {
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
  tempForWrite[strlen(tempForWrite) - 1] = '\0';
  printf("%s %s\n", path, tempForWrite);
  return writeOutTags(path, tempForWrite, numberOfExistingTags);
}

int removeTags(char *path, int argc, char *argv[])
{
  int numberOfExistingTags = 0;
  char temp[BUFFER_SIZE];
  char tempForWrite[BUFFER_SIZE];
  char *existingTags[MAX_TAGS];

  tempForWrite[0] = '\0';
  int result = readInTags(path, temp);
  if (result < 0) {
    return -1;
  } else if (strlen(temp) > 0) {
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
  printf("%s %s\n", path, tempForWrite);
  return writeOutTags(path, tempForWrite, numberOfExistingTags);
}

int removeAllTags(char *path) {
  if ((removexattr(path, ATTR) < 0) && (errno == ENOTSUP)) {
    fprintf (stderr, "Extended attributes are not available on your filesystem.\n");
    return -1;
  }
  return 1;
}

int processPath(char *path, int argc, char *argv[], int mode)
{
  switch (mode) {
  case READ:
    return displayTags(path);
  case ADD:
    return addTags(path, argc, argv);
  case REMOVE:
    return removeTags(path, argc, argv);
  case REMOVE_ALL:
    return removeAllTags(path);
  }
}

int writeOutTags(char *path, char *value, int numberOfTags)
{
  if (numberOfTags > MAX_TAGS) {
    fprintf(stderr, "This operation would exceed the maximum %i tags.\n", MAX_TAGS);
    return -1;
  } else if (numberOfTags == 0) {
    return removeAllTags(path);
  } else if (setxattr(path, ATTR, value, strlen(value) + 1, 0) < 0) {
    if ((errno == EDQUOT) || (errno == ENOSPC)) {
      fprintf(stderr, "Insufficient space to write tags");
      return -1;
    } else if (errno == ENOTSUP) {
      fprintf (stderr, "Extended attributes are not available on your filesystem.\n");
      return -1;
    }
  }
  return 1;
}

int readInTags(char *path, char *buffer) {
  int result = getxattr(path, ATTR, buffer, BUFFER_SIZE);
  if (result < 0) {
    if (errno == ENOTSUP) {
      fprintf (stderr, "Extended attributes are not available on your filesystem.\n");
      return -1;
    }
    buffer[0] = '\0';
    return 0;
  }
  return result;
}

