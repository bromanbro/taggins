#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/xattr.h>

#define READ 0
#define ADD 1
#define REMOVE 2
#define ATTR "user.tags"

void readTags(char *);
void addTags(char *, int, char *[]);
void removeTags(char *, int, char *[]);
void processPath(char *, int, char *[], int);

int main(int argc, char *argv[])
{
  int mode = READ;
  int opt;
  char *path = NULL;
  while ((opt = getopt(argc, argv, ":arf:")) != -1)
  {
    switch (opt)
    {
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

  if (path != NULL)
  {
    processPath(path, argc, argv, mode);
    return 0;
  }

  char *line = NULL;
  size_t len = 0;
  while (getline(&line, &len, stdin) != -1)
  {
    processPath(line, argc, argv, mode);
  }
  free(line);
  return 0;
}

void readTags(char *path)
{
  int size = 200;
  char temp[size];
  int result = getxattr(path, ATTR, temp, size);
  printf("%s ", path);
  if (result != -1)
  {
    printf("%s\n", temp);
  }
  else
  {
    printf("\n");
  }
}

void addTags(char *path, int argc, char *argv[])
{
  int size = 200;
  char temp[size];
  char tempForWrite[size];
  tempForWrite[0] = '\0';
  int result = getxattr(path, ATTR, temp, size);
  int numberOfExistingTags = 0;
  char *existingTags[20];
  if ((result != -1) && (strlen(temp) > 0))
  {
    strcat(tempForWrite, temp);
    strcat(tempForWrite, " ");
    char *p = strtok(temp, " ");
    while (p != NULL)
    {
      existingTags[numberOfExistingTags++] = p;
      p = strtok(NULL, " ");
    }
  }
  for (int i = optind; i < argc; i++)
  {
    int skip = 0;
    for (int j = 0; j < numberOfExistingTags; j++)
    {
      if (strcmp(existingTags[j], argv[i]) == 0)
      {
        skip = 1;
        break;
      }
    }
    if (skip != 1)
    {
      existingTags[numberOfExistingTags++] = argv[i];
      strcat(tempForWrite, argv[i]);
      strcat(tempForWrite, " ");
    }
  }
  tempForWrite[strlen(tempForWrite) - 1] = '\0';
  setxattr(path, ATTR, tempForWrite, strlen(tempForWrite), 0);
  printf("%s %s\n", path, tempForWrite);
}

void removeTags(char *path, int argc, char *argv[])
{
  int size = 200;
  char temp[size];
  char tempForWrite[size];
  tempForWrite[0] = '\0';
  int result = getxattr(path, ATTR, temp, size);
  int numberOfExistingTags = 0;
  char *existingTags[20];
  if (result != -1)
  {
    char *p = strtok(temp, " ");
    while (p != NULL)
    {
      existingTags[numberOfExistingTags++] = p;
      p = strtok(NULL, " ");
    }
  }
  for (int j = 0; j < numberOfExistingTags; j++)
  {
    int skip = 0;
    for (int i = optind; i < argc; i++)
    {
      if (strcmp(existingTags[j], argv[i]) == 0)
      {
        skip = 1;
        break;
      }
    }
    if (skip != 1)
    {
      strcat(tempForWrite, existingTags[j]);
      strcat(tempForWrite, " ");
    }
  }
  tempForWrite[strlen(tempForWrite) - 1] = '\0';
  setxattr(path, ATTR, tempForWrite, strlen(tempForWrite), 0);
  printf("%s %s\n", path, tempForWrite);
}

void processPath(char *path, int argc, char *argv[], int mode)
{
  switch (mode)
  {
  case READ:
    readTags(path);
    break;
  case ADD:
    addTags(path, argc, argv);
    break;
  case REMOVE:
    removeTags(path, argc, argv);
  }
}