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

Status load();
Status split(char *line, Record &rec);
Status retrieve(char *keystring, Record &rec);

int main(int argc, char *argv[]) {
    // disable stream sync
    ios::sync_with_stdio(false);

    // if load
    if (strcmp(argv[1], "load") == 0) {
        load();
    } else {  // retrieve
        Record rec;
        retrieve(argv[2], rec);
        cout.precision(2);
        cout << rec.order_key << '|'
             << rec.cust_key << '|'
             << std::fixed << rec.total_price << '|'
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

    // open data files
    ofstream orderStream("order_key.dat", ios::binary | ios::app);
    ofstream custStream("cust_key.dat", ios::binary | ios::app);
    ofstream priceStream("total_price.dat", ios::binary | ios::app);
    ofstream shipStream("ship_priority.dat", ios::binary | ios::app);

    char temp[LINE_SIZE];
    Record rec;

    while (fin.getline(temp, LINE_SIZE, '\n'))  {
        // read a line and split
        if (split(temp, rec) == FAIL) {
            cout << "An error occurs! spliting fail" << endl;
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

/* given a search key, found the record in the file,
 *  return the record with necessary fields
 */
Status retrieve(char *keystring, Record &rec) {
    int key = atoi(keystring);

    // open data files
    ifstream orderStream("order_key.dat", ios::binary);
    ifstream custStream("cust_key.dat", ios::binary);
    ifstream priceStream("total_price.dat", ios::binary);
    ifstream shipStream("ship_priority.dat", ios::binary);

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
                
                custStream.seekg(offset * sizeof(int), ios::beg);
                priceStream.seekg(offset * sizeof(double), ios::beg);
                shipStream.seekg(offset * sizeof(int), ios::beg);

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
