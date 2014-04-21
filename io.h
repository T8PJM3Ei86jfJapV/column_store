#include "utils.h"

#ifndef IO_H
#define IO_H

int order_page[PAGE_SIZE];
int cust_page[PAGE_SIZE];
double price_page[PAGE_SIZE];
int ship_page[PAGE_SIZE];

Status load();
Status retrieve(char *keystring, Record &rec);

#endif
