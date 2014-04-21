#ifndef UTILS_H
#define UTILS_H

#define PAGE_SIZE 1024
#define LINE_SIZE 256

enum Status {
    SUCCESS,
    FAIL
};

struct Record {
    int order_key;
    int cust_key;
    double total_price;
    int ship_priority;
};

Status split(char *line, Record &rec);

#endif
