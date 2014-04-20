#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#define PAGE_SIZE 1024
#define LINE_SIZE 256
using namespace std;

enum Status {
    SUCCESS,
    FAIL
};

int order_page[PAGE_SIZE];
int cust_page[PAGE_SIZE];
double price_page[PAGE_SIZE];
int ship_page[PAGE_SIZE];

struct Record {
    int order_key;
    int cust_key;
    double total_price;
    int ship_priority;
};

Record rec;
Status load();
Status split(char *line);
Status retrieve(char *keystring);

int main(int argc, char *argv[]) {
    Record result;

    // disable stream sync
    ios::sync_with_stdio(false);

    // if load
    if (strcmp(argv[1], "load") == 0) {
        load();
    }
    // retrieve(key, &rec)
    else {
        retrieve(argv[2]);
        cout << rec.order_key << '|'
             << rec.cust_key << '|'
             << rec.total_price << '|'
             << rec.ship_priority << '\n';
    }

    return 0;
}


Status load() {
    int count = 0;  // number of slots in current page
    // open order.tbl with C++ file stream

    ifstream fin;
    fin.open("orders.tbl");
    // if it doesn't exist
    if (!fin)
        return FAIL;

    // open order_key
    ofstream orderStream;
    orderStream.open("order_key.dat", ios::binary | ios::app);
    // open cust_key
    ofstream custStream;
    custStream.open("cust_key.dat", ios::binary | ios::app);
    // open total_price
    ofstream priceStream;
    priceStream.open("total_price.dat", ios::binary | ios::app);
    // open ship_priority
    ofstream shipStream;
    shipStream.open("ship_priority.dat", ios::binary | ios::app);

    // while !eof
    while (!fin.eof())  {
        // read a line
        // split
        char temp[LINE_SIZE];
        fin.getline(temp, LINE_SIZE, '\n');

        if (split(temp) == FAIL) {
            cout << "An error occurs! spliting fail" << endl;
            continue;
        }
        if (count == PAGE_SIZE) {
            for (int i = 0; i < PAGE_SIZE; ++i) {
                orderStream.write((char *)&order_page[i], sizeof(int));
                custStream.write((char *)&cust_page[i], sizeof(int));
                priceStream.write((char *)&price_page[i], sizeof(double));
                shipStream.write((char *)&ship_page[i], sizeof(int));
            }
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
    for (int i = 0; i < count; ++i) {
        orderStream.write((char *)&order_page[i], sizeof(int));
        custStream.write((char *)&cust_page[i], sizeof(int));
        priceStream.write((char *)&price_page[i], sizeof(double));
        shipStream.write((char *)&ship_page[i], sizeof(int));
    }
    for (int i = count; i < PAGE_SIZE; ++i) {
        orderStream.write((char *)&int_zero, sizeof(int));
        custStream.write((char *)&int_zero, sizeof(int));
        priceStream.write((char *)&double_zero, sizeof(double));
        shipStream.write((char *)&int_zero, sizeof(int));
    }
    // if there is some space left
    // pad 0s
    // write the page

    return SUCCESS;
}

/* get a line, return the necessary fields in it
 */
Status split(char *line) {
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

/* given a search key, found the record in the file,
 *  return the record with necessary fields
 */
Status retrieve(char *keystring) {
    int key = atoi(keystring);
    // open 4 files

    // open order_key
    ifstream orderStream;
    orderStream.open("order_key.dat", ios::binary);
    // open cust_key
    ifstream custStream;
    custStream.open("cust_key.dat", ios::binary);
    // open total_price
    ifstream priceStream;
    priceStream.open("total_price.dat", ios::binary);
    // open ship_priority
    ifstream shipStream;
    shipStream.open("ship_priority.dat", ios::binary);
    // if it doesn't exist
    if (!orderStream || !custStream || !priceStream || !shipStream)
        return FAIL;

    int page_id = 0;
    while (!orderStream.eof())  {
        // while !eof
        for (int i = 0; i < PAGE_SIZE; ++i) {
            orderStream.read((char *)&order_page[i], sizeof(int));
        }
        // read an order_key page
        ++page_id;
        for (int count = 0; count < PAGE_SIZE; ++count) {
            if (order_page[count] == 0) {
                return FAIL;
            }

            if (order_page[count] == key) {
                int tempCustkey;
                double tempTotalprice;
                int tempShippriorty;
                int index = (page_id - 1) * PAGE_SIZE + count;
                for (int i = 0; i <= index; ++i) {
                    custStream.read((char *)&tempCustkey, sizeof(int));
                    priceStream.read((char *)&tempTotalprice, sizeof(double));
                    shipStream.read((char *)&tempShippriorty, sizeof(int));
                }
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
    }
    return FAIL;
}
