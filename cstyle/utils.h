#ifndef UTILS_H
#define UTILS_H

#define PAGE_SIZE 1024
#define LINE_SIZE 256

typedef enum {
    SUCCESS,
    FAIL
} Status;

typedef struct {
    int order_key;
    int cust_key;
    double total_price;
    int ship_priority;
} Record;

Status split(char *line, Record *rec);

#endif
