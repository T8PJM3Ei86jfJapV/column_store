#include <string.h>
#include <stdio.h>
#include "io.h"

int main(int argc, char *argv[]) {
    // if load
    if (strcmp(argv[1], "load") == 0) {
        load();
    } else {  // retrieve
        Record rec;

        if (retrieve(argv[2], &rec) == SUCCESS) {
          printf("%d|%d|%.2f|%d\n", rec.order_key, rec.cust_key, 
              rec.total_price, rec.ship_priority);
        } else {
          printf("Record not found.\n");
        }
    }

    return 0;
}
