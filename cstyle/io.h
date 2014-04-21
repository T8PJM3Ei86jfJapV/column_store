#include "utils.h"

#ifndef IO_H
#define IO_H

#define BUFFER_SIZE 1024

Status load();
Status retrieve(char *keystring, Record *rec);

#endif
