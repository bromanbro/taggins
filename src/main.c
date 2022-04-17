#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <unistd.h>

#define ATTR "user.tags"
#define MAX_TAGS 50
#define MAX_TAG_SIZE 50
#define BUFFER_SIZE (MAX_TAGS * MAX_TAG_SIZE)

enum {
  READ,
  ADD,
  REMOVE,
  REMOVE_ALL
};

int readTags(char *);
int addTags(char *, int, char *[]);
int removeTags(char *, int, char *[]);
int removeAllTags(char *);
int processPath(char *, int, char *[], int);
int writeOutTags(char *, char *, int);
int readInTags(char *, char *);
void checkExclusive(int);
void printPathWithTags(char *, char *);
void chomp(char *);

int main(int argc, char *argv[])
{
  int mode = READ;
  int opt;
  char *path = NULL;
  while ((opt = getopt(argc, argv, ":ardf:")) != -1) {
    switch (opt) {
    case 'a':
      checkExclusive(mode);
      mode = ADD;
      break;
    case 'r':
      checkExclusive(mode);
      mode = REMOVE;
      break;
    case 'd':
      checkExclusive(mode);
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
    chomp(line);
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
  if (readInTags(path, temp) < 0) {
    return -1;
  }
  printPathWithTags(path, temp);
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
  printPathWithTags(path, tempForWrite);
  return writeOutTags(path, tempForWrite, numberOfExistingTags);
}

int removeTags(char *path, int argc, char *argv[])
{
  int numberOfExistingTags = 0;
  int numberOfTags = 0;
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
      numberOfTags++;
      strcat(tempForWrite, existingTags[j]);
      strcat(tempForWrite, " ");
    }
  }
  tempForWrite[strlen(tempForWrite) - 1] = '\0';
  printPathWithTags(path, tempForWrite);
  return writeOutTags(path, tempForWrite, numberOfTags);
}

int removeAllTags(char *path) {
  int result = removexattr(path, ATTR);
  if (result < 0) {
    if ((errno == ENOTSUP) || (errno == ENOENT)){
      fprintf (stderr, "%s\n", strerror(errno));
      return -1;
    }
  }
  printf("%s - extended attribute removed\n", path);
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
    fprintf (stderr, "%s\n", strerror(errno));
    return -1;
  }
  return 1;
}

int readInTags(char *path, char *buffer) {
  /* We do not always want to hard fail on an error.  It could be that the user.tags xattr
   * Is simply not on the file at this time.  So we just keep processing.
   */
  int result = getxattr(path, ATTR, buffer, BUFFER_SIZE);
  if (result < 0) {
    if ((errno == ENOTSUP) || (errno == ENOENT)){
      fprintf (stderr, "%s\n", strerror(errno));
      return -1;
    }
    buffer[0] = '\0';
    return 0;
  }
  return result;
}

void checkExclusive(int mode)
{
  if (mode != READ) {
    fprintf(stderr, "usage: taggins [-a|-r|-d] [-f filepath] [tag ...]\n");
    exit(EXIT_FAILURE);
  }
}

void printPathWithTags(char *path, char *tags)
{
  printf("%s", path);
  if (strlen(tags) > 0) {
    printf(" tags:{%s}", tags);
  }
  printf("\n");
}

void chomp(char *st)
{
  st[strcspn(st, "\n")] = '\0';
}
