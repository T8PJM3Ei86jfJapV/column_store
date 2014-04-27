#include <cstdlib>
#include <iostream>
#include <fstream>

#include "io.h"
using std::ifstream;
using std::ofstream;

int order_page[PAGE_SIZE];
int cust_page[PAGE_SIZE];
double price_page[PAGE_SIZE];
int ship_page[PAGE_SIZE];

Status load() {
    int count = 0;  // number of slots in current page
    // open order.tbl with C++ file stream

    ifstream fin;
    fin.open("orders.tbl");
    // if it doesn't exist
    if (!fin)
        return FAIL;

    // open data files
    ofstream order_s("order_key.dat", std::ios::binary | std::ios::app);
    ofstream cust_s("cust_key.dat", std::ios::binary | std::ios::app);
    ofstream price_s("total_price.dat", std::ios::binary | std::ios::app);
    ofstream ship_s("ship_priority.dat", std::ios::binary | std::ios::app);

    char buffer[LINE_SIZE];
    Record rec;
#ifdef TRACK
    int page_id = 0;
#endif
    while (fin.getline(buffer, LINE_SIZE, '\n'))  {
        // read a line and split
        if (split(buffer, rec) == FAIL) {
            std::cout << "An error occurs! spliting fail" << '\n';
            continue;
        }

        if (count == PAGE_SIZE) {
            order_s.write((char *)order_page, sizeof(int) * PAGE_SIZE);
            cust_s.write((char *)cust_page, sizeof(int) * PAGE_SIZE);
            price_s.write((char *)&price_page, sizeof(double) * PAGE_SIZE);
            ship_s.write((char *)&ship_page, sizeof(int) * PAGE_SIZE);
            // write to file
            count = 0;
#ifdef TRACK
            page_id++;
#endif
        }

        order_page[count] = rec.order_key;
        cust_page[count] = rec.cust_key;
        price_page[count] = rec.total_price;
        ship_page[count] = rec.ship_priority;
        ++count;
    }

    // assert: eof
    int int_zero = 0;
    double double_zero = 0.0;

    // padding zeros
    for (int i = count; i < PAGE_SIZE; ++i) {
        order_page[i] = cust_page[i] = ship_page[i] = int_zero;
        price_page[i] = double_zero;
    }

    // write the page
    order_s.write((char *)order_page, sizeof(int) * PAGE_SIZE);
    cust_s.write((char *)cust_page, sizeof(int) * PAGE_SIZE);
    price_s.write((char *)price_page, sizeof(double) * PAGE_SIZE);
    ship_s.write((char *)ship_page, sizeof(int) * PAGE_SIZE);


#ifdef TRACK
    page_id++;
    std::cout << "Writing Done. " << page_id << " pages have be written." << '\n';
#endif
    return SUCCESS;
}

/* given a search key, found the record in the file,
 *  return the record with necessary fields
 */
Status retrieve(int key, Record &rec) {
    // open data files
    ifstream order_s("order_key.dat", std::ios::binary);
    ifstream cust_s("cust_key.dat", std::ios::binary);
    ifstream price_s("total_price.dat", std::ios::binary);
    ifstream ship_s("ship_priority.dat", std::ios::binary);

    // if it doesn't exist
    if (!order_s || !cust_s || !price_s || !ship_s)
        return FAIL;

#ifdef ORDERED
    // seek the last non-zero record;
    order_s.seekg(0, std::ios::end);
    int size = order_s.tellg() / sizeof(int);

#ifdef TRACK
    std::cout << "Size of file: " << size << " records.\n";
#endif

    // read in the last page
    order_s.seekg((size - PAGE_SIZE) * sizeof(int), std::ios::beg);

#ifdef TRACK
    std::cout << "Start searching for last nonzero record.\n"
              << "Seek to " << size - PAGE_SIZE << "\n";
#endif

    int tmp_key = 0, nonzero_offset = 0;
    order_s.read((char *)&tmp_key, sizeof(int));

    while (tmp_key != 0 && order_s.tellg() / sizeof(int) < size) {
#ifdef TRACK
        std::cout << "Current position: " << order_s.tellg() << "\n";
#endif
        if (tmp_key == key) {
            int tmp_cust;
            double tmp_price;
            int tmp_ship;
            int offset = size - PAGE_SIZE + nonzero_offset;
#ifdef TRACK
            std::cout << "Found record in " << offset << " in last page\n";
#endif
            cust_s.seekg(offset * sizeof(int), std::ios::beg);
            price_s.seekg(offset * sizeof(double), std::ios::beg);
            ship_s.seekg(offset * sizeof(int), std::ios::beg);

            // read other fields
            cust_s.read((char *)&tmp_cust, sizeof(int));
            price_s.read((char *)&tmp_price, sizeof(double));
            ship_s.read((char *)&tmp_ship, sizeof(int));

            rec.order_key = key;
            rec.cust_key = tmp_cust;
            rec.total_price = tmp_price;
            rec.ship_priority = tmp_ship;

            order_s.close();
            cust_s.close();
            price_s.close();
            ship_s.close();
            return SUCCESS;
        }

        order_s.read((char *)&tmp_key, sizeof(int));
        nonzero_offset++;
    }

    size = size - PAGE_SIZE + nonzero_offset;

#ifdef TRACK
    std::cout << "Start binary search, last nonzero record in "
              << size - 1 << "\n";
#endif

    // rewind
    order_s.seekg(0, std::ios::beg);

    int low = 0, high = size - 1;
    int middle = (low + high) / 2;

    while (low <= high) {
        middle = (low + high) / 2;

#ifdef TRACK
        std::cout << "Checking middle point in " << middle << "\n";
#endif

        order_s.seekg(middle * sizeof(int), std::ios::beg);
        order_s.read((char *)&tmp_key, sizeof(int));

#ifdef TRACK
        std::cout << "Reading key in " << middle << "\n"
                  << "order_s.tellg() = " << order_s.tellg() << "\n"
                  << "tmp_key = " << tmp_key << "\n";
#endif

        if (tmp_key == 0) {
            return FAIL;
        }

        if (tmp_key == key) {
            int tmp_cust;
            double tmp_price;
            int tmp_ship;

#ifdef TRACK
            std::cout << "Found record in " << middle << "\n";
#endif
            cust_s.seekg(middle * sizeof(int), std::ios::beg);
            price_s.seekg(middle * sizeof(double), std::ios::beg);
            ship_s.seekg(middle * sizeof(int), std::ios::beg);

            // read other fields
            cust_s.read((char *)&tmp_cust, sizeof(int));
            price_s.read((char *)&tmp_price, sizeof(double));
            ship_s.read((char *)&tmp_ship, sizeof(int));

            rec.order_key = key;
            rec.cust_key = tmp_cust;
            rec.total_price = tmp_price;
            rec.ship_priority = tmp_ship;

            order_s.close();
            cust_s.close();
            price_s.close();
            ship_s.close();
            return SUCCESS;
        }

        if (key > tmp_key) {
            low = middle + 1;
        } else {
            high = middle - 1;
        }
    }

/*********************** Linear Search ***************************/
#else
    int page_id = 0;
    while (!order_s.eof())  {
        // while !eof
        
        // read an order_key page
        order_s.read((char *)order_page, sizeof(int) * PAGE_SIZE);

        for (int count = 0; count < PAGE_SIZE; ++count) {
            if (order_page[count] == 0) {
                return FAIL;
            }

            if (order_page[count] == key) {
                int tmp_cust;
                double tmp_price;
                int tmp_ship;
                int offset = page_id * PAGE_SIZE + count;

                cust_s.seekg(offset * sizeof(int), std::ios::beg);
                price_s.seekg(offset * sizeof(double), std::ios::beg);
                ship_s.seekg(offset * sizeof(int), std::ios::beg);

                cust_s.read((char *)&tmp_cust, sizeof(int));
                price_s.read((char *)&tmp_price, sizeof(double));
                ship_s.read((char *)&tmp_ship, sizeof(int));
                // seek page_id * PAGE_SIZE + count
                // read other fields
                rec.order_key = key;
                rec.cust_key = tmp_cust;
                rec.total_price = tmp_price;
                rec.ship_priority = tmp_ship;
                // ...
# ifdef TRACK
                std::cout << "Record found after iterating over "
                          << page_id << " pages\n";
# endif
                order_s.close();
                cust_s.close();
                price_s.close();
                ship_s.close();
                return SUCCESS;
            }
        }
        ++page_id;
    }

#endif

    order_s.close();
    cust_s.close();
    price_s.close();
    ship_s.close();
    return FAIL;
}
