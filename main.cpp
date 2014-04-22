#include <iostream>
#include <cstring>

#include "io.h"

int main(int argc, char *argv[]) {
    // disable stream sync
  std::ios::sync_with_stdio(false);

    // if load
    if (strcmp(argv[1], "load") == 0) {
        load();
    } else {  // retrieve
        Record rec;
        int key;
        while (std::cin >> key) {
            if (retrieve(key, rec) == SUCCESS) {
                std::cout.precision(2);
                std::cout << rec.order_key << ' '
                    << rec.cust_key << ' '
                    << std::fixed << rec.total_price << ' '
                    << rec.ship_priority << '\n';
            }
        }
    }

    return 0;
}
