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
    ofstream orderStream("order_key.dat", std::ios::binary | std::ios::app);
    ofstream custStream("cust_key.dat", std::ios::binary | std::ios::app);
    ofstream priceStream("total_price.dat", std::ios::binary | std::ios::app);
    ofstream shipStream("ship_priority.dat", std::ios::binary | std::ios::app);

    char temp[LINE_SIZE];
    Record rec;

    while (fin.getline(temp, LINE_SIZE, '\n'))  {
        // read a line and split
        if (split(temp, rec) == FAIL) {
          std::cout << "An error occurs! spliting fail" << '\n';
            continue;
        }

        if (count == PAGE_SIZE) {
            orderStream.write((char *)order_page, sizeof(int) * PAGE_SIZE);
            custStream.write((char *)cust_page, sizeof(int) * PAGE_SIZE);
            priceStream.write((char *)&price_page, sizeof(double) * PAGE_SIZE);
            shipStream.write((char *)&ship_page, sizeof(int) * PAGE_SIZE);
            // write to file
            count = 0;
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

    // write the page
    orderStream.write((char *)order_page, sizeof(int) * count);
    custStream.write((char *)cust_page, sizeof(int) * count);
    priceStream.write((char *)price_page, sizeof(double) * count);
    shipStream.write((char *)ship_page, sizeof(int) * count);

    for (int i = count; i < PAGE_SIZE; ++i) {
        orderStream.write((char *)&int_zero, sizeof(int));
        custStream.write((char *)&int_zero, sizeof(int));
        priceStream.write((char *)&double_zero, sizeof(double));
        shipStream.write((char *)&int_zero, sizeof(int));
    }

    return SUCCESS;
}

/* given a search key, found the record in the file,
 *  return the record with necessary fields
 */
Status retrieve(char *keystring, Record &rec) {
    int key = atoi(keystring);

    // open data files
    ifstream orderStream("order_key.dat", std::ios::binary);
    ifstream custStream("cust_key.dat", std::ios::binary);
    ifstream priceStream("total_price.dat", std::ios::binary);
    ifstream shipStream("ship_priority.dat", std::ios::binary);

    // if it doesn't exist
    if (!orderStream || !custStream || !priceStream || !shipStream)
        return FAIL;

    int page_id = 0;
    while (!orderStream.eof())  {
        // while !eof
        orderStream.read((char *)order_page, sizeof(int) * PAGE_SIZE);
        // read an order_key page
        
        for (int count = 0; count < PAGE_SIZE; ++count) {
            if (order_page[count] == 0) {
                return FAIL;
            }

            if (order_page[count] == key) {
                int tempCustkey;
                double tempTotalprice;
                int tempShippriorty;
                int offset = page_id * PAGE_SIZE + count;
                
                custStream.seekg(offset * sizeof(int), std::ios::beg);
                priceStream.seekg(offset * sizeof(double), std::ios::beg);
                shipStream.seekg(offset * sizeof(int), std::ios::beg);

                custStream.read((char *)&tempCustkey, sizeof(int));
                priceStream.read((char *)&tempTotalprice, sizeof(double));
                shipStream.read((char *)&tempShippriorty, sizeof(int));
                // seek page_id * PAGE_SIZE + count
                // read other fields
                rec.order_key = key;
                rec.cust_key = tempCustkey;
                rec.total_price = tempTotalprice;
                rec.ship_priority = tempShippriorty;
                // ...
                return SUCCESS;
            }
        }
        ++page_id;
    }
    return FAIL;
}
