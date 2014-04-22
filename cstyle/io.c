#include <stdlib.h>
#include <stdio.h>

#include "io.h"

int order_page[PAGE_SIZE];
int cust_page[PAGE_SIZE];
double price_page[PAGE_SIZE];
int ship_page[PAGE_SIZE];

Status load() {
    int count = 0;  // number of slots in current page
    char buffer[LINE_SIZE];
    Record rec;
    int int_zero = 0;
    double double_zero = 0.0;
    int i = 0;
    FILE *order_f = fopen("order_key.dat", "wb+");
    FILE *cust_f= fopen("cust_key.dat", "wb+");
    FILE *price_f = fopen("total_price.dat", "wb+");
    FILE *ship_f = fopen("ship_priority.dat", "wb+");
    FILE *fin = fopen("orders.tbl", "r");
#ifdef TRACK
    int page_id = 0;
#endif

    // if it doesn't exist
    if (fin == NULL || order_f == NULL || cust_f == NULL || 
        price_f == NULL || ship_f == NULL)
        return FAIL;
    // open data files

    while (fgets(buffer, LINE_SIZE, fin) != NULL)  {
        // read a line and split
        if (split(buffer, &rec) == FAIL) {
          printf("An error occurs! spliting fail\n");
            continue;
        }

        if (count == PAGE_SIZE) {
            fwrite(order_page, sizeof(int), PAGE_SIZE, order_f);
            fwrite(cust_page, sizeof(int), PAGE_SIZE, cust_f);
            fwrite(price_page, sizeof(double), PAGE_SIZE, price_f);
            fwrite(ship_page, sizeof(int), PAGE_SIZE, ship_f);
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
    for (i = count; i < PAGE_SIZE; ++i) {
      order_page[i] = cust_page[i] = ship_page[i] = int_zero;
      price_page[i] = double_zero;
    }
    // write the page
    fwrite(order_page, sizeof(int), PAGE_SIZE, order_f);
    fwrite(cust_page, sizeof(int), PAGE_SIZE, cust_f);
    fwrite(price_page, sizeof(double), PAGE_SIZE, price_f);
    fwrite(ship_page, sizeof(int), PAGE_SIZE, ship_f);
    
#ifdef TRACK
    page_id++;
    printf("Writing done. %d pages written.\n", page_id);
#endif
    return SUCCESS;
}

/* given a search key, found the record in the file,
 *  return the record with necessary fields
 */
Status retrieve(char *keystring, Record *rec) {
    int key = atoi(keystring);
    int page_id = 0;
    int count = 0;

    // open data files
    FILE *order_f = fopen("order_key.dat", "rb");
    FILE *cust_f= fopen("cust_key.dat", "rb");
    FILE *price_f = fopen("total_price.dat", "rb");
    FILE *ship_f = fopen("ship_priority.dat", "rb");

    // if it doesn't exist
    if (order_f == NULL || cust_f == NULL || 
        price_f == NULL || ship_f == NULL)
        return FAIL;

    while (!feof(order_f))  {
        // while !eof
        fread(order_page, sizeof(int), PAGE_SIZE, order_f);
        // read an order_key page
        
        for (count = 0; count < PAGE_SIZE; ++count) {
            if (order_page[count] == 0) {
                return FAIL;
            }

            if (order_page[count] == key) {
                int tmp_cust;
                double tmp_price;
                int tmp_ship;
                int offset = page_id * PAGE_SIZE + count;

                // seek page_id * PAGE_SIZE + count
                fseek(cust_f, offset * sizeof(int), SEEK_SET); 
                fseek(price_f, offset * sizeof(double), SEEK_SET); 
                fseek(ship_f, offset * sizeof(int), SEEK_SET); 

                // read other fields
                fread(&tmp_cust, sizeof(int), 1, cust_f);
                fread(&tmp_price, sizeof(double), 1, price_f);
                fread(&tmp_ship, sizeof(int), 1, ship_f);

                rec->order_key = key;
                rec->cust_key = tmp_cust;
                rec->total_price = tmp_price;
                rec->ship_priority = tmp_ship;
#ifdef TRACK
                printf("Record found after iterating over %d pages.\n", page_id); 
#endif
                return SUCCESS;
            }
        }
        ++page_id;
    }
    return FAIL;
}
