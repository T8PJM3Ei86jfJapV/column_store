#define PAGE_SIZE 1024

enum Status {
	SUCCESS,
	FAIL
};

int order_page[PAGE_SIZE];
int cus_page[PAGE_SIZE];
double price_page[PAGE_SIZE];
int ship_page[PAGE_SIZE];

struct Record {
	int order_key;
	int cust_key;
	double total_price;
	int ship_priority;
};

int main(int argc, char *argv[])
	Record result;

	// disable stream sync

	// if load
	// load()

	// else
	// retrieve(key, &rec)

	// print result

	return 0;
}


Status load {
	int count;  // number of slots in current page
	// open order.tbl with C++ file stream

	// if it doesn't exist
	return FAIL;

	// open order_key
	// open cust_key
	// open total_price
	// open shipp_riority

	// while !eof 
		// read a line
		// split

		// if count == PAGE_SIZE
			// write the pages
			// order_f.write
			// ...
			count = 0;
		
		// not full
		// append the slots to each pages
		order_page[count] = rec.order_key;
		// ... 

		count++

	// assert: eof
	// if there is some space left
		// pad 0s
		// write the page

	return SUCCESS;
}

/* get a line, return the necessary fields in it
 */
Status split(char*line, Record &rec) {

}

/* given a search key, found the record in the file,
 *  return the record with necessary fields
 */
Status retrieve(int key, Record &rec) {
	// open 4 files

	int page_id = 0;
	// while !eof {
		// read an order_key page
		page_id++;
		for (int count = 0; count < PAGE_SIZE; ++count) {
			if (order_page[count] == 0) {
				return FAIL;
			}

			if (order_page[count] == key) {
				// seek page_id * PAGE_SIZE + count
				// read other fields
				rec.order_key = key;
				// ...
				return SUCCESS;
			}
		}
	}
	return FAIL;
}