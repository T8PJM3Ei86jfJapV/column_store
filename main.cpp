#include <iostream>
#include <cstring>

#include "utils.h"
#include "io.h"

int main(int argc, char *argv[]) {
    // disable stream sync
  std::ios::sync_with_stdio(false);

    // if load
    if (strcmp(argv[1], "load") == 0) {
        load();
    } else {  // retrieve
        Record rec;
        
        if (retrieve(argv[2], rec) == SUCCESS) {
          std::cout.precision(2);
          std::cout << rec.order_key << '|'
                 << rec.cust_key << '|'
                 << std::fixed << rec.total_price << '|'
                 << rec.ship_priority << '\n';
        } else {
          std::cout << "Record not found." << '\n';
        }
    }

    return 0;
}
