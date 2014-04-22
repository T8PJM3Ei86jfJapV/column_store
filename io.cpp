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

    int page_id = 0;
    while (!order_s.eof())  {
        // while !eof
        order_s.read((char *)order_page, sizeof(int) * PAGE_SIZE);
        // read an order_key page
        
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
#ifdef TRACK
                std::cout << "Record found after iterating over " 
                    << page_id << " pages\n";
#endif
                order_s.close();
                cust_s.close();
                price_s.close();
                ship_s.close();
                return SUCCESS;
            }
        }
        ++page_id;
    }
    
    order_s.close();
    cust_s.close();
    price_s.close();
    ship_s.close();
    return FAIL;
}
