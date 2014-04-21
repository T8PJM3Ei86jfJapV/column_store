#include <cstring>
#include <cstdlib>

#include "utils.h"

/* get a line, return the necessary fields in it
 */
Status split(char *line, Record &rec) {
    char *ptr;
    const char *op = "|";
    ptr = strtok(line, op);
    int col = 1;
    while (ptr != NULL) {
        if (col == 1)
            rec.order_key = atoi(ptr);
        if (col == 2)
            rec.cust_key = atoi(ptr);
        if (col == 4)
            rec.total_price = atof(ptr);
        if (col == 8)
            rec.ship_priority = atoi(ptr);
        ++col;
        ptr = strtok(NULL, op);
    }
    return SUCCESS;
}
