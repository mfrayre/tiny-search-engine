/* 
 * test_indexio.c --- 
 * 
 * Author: Marina Frayre
 * Created: 10-29-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */
#include <stdio.h>                                                                
#include "pageio.h"                                                               
#include "queue.h"                                                                
#include "hash.h"                                                                 
#include "webpage.h"                                                              
#include <string.h>                                                               
#include <stdbool.h>                                                              
#include <ctype.h>   
#include "indexio.h"


// Word structure, for each element in HASH TABLE "index"
typedef struct word_data {
	char *wordy;
	queue_t *webpages;
} word_t;


// Document structure, for each document associated with a word in HASH "index"
typedef struct page {
	int docID;
	int count;
} page_t;


void print_word(void *el) {
	word_t *w = (word_t *)el;
	printf("%s \n", w->wordy);
}



void free_pages(void *element) {
	if (element == NULL ) {
		return;
	}
	page_t *p = (page_t *)element;
	if (p != NULL ) {
		free(p);
	}

}

void free_pages_in_queues(void *elementp) {
	if (elementp == NULL) {
		return;
	}
	word_t *w = (word_t *)elementp;
	queue_t *q = w->webpages;

	qapply(q, free_pages);

}

// free all queues in hash table
void free_queues(void *elementp) {
	if (elementp == NULL) {
		return;
	}
	word_t *word_element = (word_t *) elementp;
	queue_t *free_me = word_element->webpages;

	if (free_me != NULL){
		//qapply(free_me, free_pages);
		qclose(free_me);
	}
}

void free_words(void *elementp) {
	if (elementp == NULL ) {
		return;
	}
	word_t *w = (word_t *) elementp;
	free(w);
}

// Function for greating new page_t elements (REMEMBER TO FREE)
page_t *make_page(int docID, int count) {

    page_t *pp;

    if(!(pp = (page_t*)malloc(sizeof(page_t)))) {
        printf("Error with malloc");
        return NULL;
    }

    pp->docID=docID;
    pp->count=count;

    return pp;
}


int countLines(const char *filename) {
    FILE *file = fopen(filename, "r");
		if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int lines = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }

    fclose(file);
    return lines;
}

int main() {

	hashtable_t *new = hopen(1024);
	char *dir = "indexnm_test";
	
	indexload(new, dir);
	
	//	happly(new, print_word);

		// test proper loading
	if (new == NULL) {
	return 1;
	}

	//happly(new, print_word);
	
	// do something with the hash table	
	word_t *new_word2 = malloc(sizeof(word_t)); // FREE'D Later in HAPPLY?
	char *wor = "testme";
	new_word2->wordy = wor;
	queue_t *pages = qopen();
	page_t *page = make_page(1, 47);
	qput(pages, (void *)page);
	new_word2->webpages = pages;
	hput(new, new_word2, new_word2->wordy, strlen(new_word2->wordy));

	// now save the new index
	char *filename = "output_index";
	indexsave(new, filename);

	// compare the two files
	const char *f1 = "indexnm_test";
	const char *f2 = "output_index";

	int line1 = countLines(f1);
	int line2 = countLines(f2);

	int diff = line1 +1;
	if (line2 == diff) {
		printf("successfully added new item to queue, and saved to indexnm\n");
	} else {
		return 1;
	}
	
	// FREE STUFF
	happly(new, free_queues);
	happly(new, free_words);
	
	hclose(new);
	//free(new_word2);
	return 0; 
}
