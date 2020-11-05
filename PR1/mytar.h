#ifndef _MYTAR_H
#define _MYTAR_H

#include <limits.h>

typedef enum{
  CREATE  = 10,
  EXTRACT = 11,
  NONE    = 12,
  ERROR   = 13
} flags;

typedef struct {
  char* name;
  unsigned int size;
} stHeaderEntry;

int createTar(int nFiles, char *fileNames[], char tarName[]);
int extractTar(char tarName[]);


#endif /* _MYTAR_H */
