#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
#define ACTIVE        1
#define ENDED         2

typedef unsigned int  uint32;
typedef int		      int32;
typedef char          int8;
typedef unsigned char uint8;

void getUint32(const char *str, uint32 *value);

uint32 getRandomValueWithinRangeUint32(uint32 min, uint32 max);

int32 getRandomValueWithinRangeInt32(int32 min, int32 max);

uint32 strLength(char *str);

#endif
