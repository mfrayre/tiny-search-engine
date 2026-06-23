/* 
 * indexio.c --- 
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


//char queue_string[1000];
FILE *f;


// Document structure, for each document associated with a word in HASH "index"
typedef struct page {
	int docID;
	int count;
	//	struct assoc_page_t *next;
} page_t;

// Word structure, for each element in HASH TABLE "index"
typedef struct word_data {
	char *wordy;
	queue_t *webpages;
} word_t;


page_t *make_page2(int docID, int count) {

    page_t *pp;

    if(!(pp = (page_t*)malloc(sizeof(page_t)))) {
        printf("Error with malloc");
        return NULL;
    }

    pp->docID=docID;
    pp->count=count;

    return pp;
}


void iterate_queues(void *e) {

	queue_t *q = (queue_t *) e;
	page_t *p = (page_t *) q;

	int id = p->docID;
	int c = p->count;

	fprintf(f," %d %d", id, c);
	
}


void get_one_line(void *elementp) {

	word_t *w = (word_t *) elementp;
	char *word = w->wordy;
	queue_t *queue = w->webpages;
	
	fprintf(f, "%s", word);
	qapply(queue, iterate_queues);
	
	fprintf(f, "\n");
	
}


void indexsave(hashtable_t *index, char *dirnm) {

	//	queue_string = malloc(5000);
	
	char *fname = dirnm;
	f = fopen(fname, "w");

	if (f == NULL) {
		printf("failed to open file \n");
		return;
	}

	happly(index, get_one_line);

	//	fprintf(f, "%s", queue_string);
	printf("done printing \n");
	fclose(f);
}

void indexload(hashtable_t *in, char *dirnm) {

	//	char *file_path[sizeof(dirnm) + 100);
	//	printf("Loading the saved hashtable_t\n");
	char delimiter[] = " ";
	FILE *fload = fopen(dirnm, "r");

	if (fload == NULL) {
		printf("Issues opening file\n");
		return;
	}

	//	char word_found[100];
	char line[500];

	while (fgets(line, sizeof(line), fload) != NULL) {

	 	word_t *n = malloc(sizeof(word_t)); // FREE'D Later in HAPPLY?

		char *liney = malloc(sizeof(line));
		strcpy(liney, line);

		n->wordy = strtok(liney, delimiter); 
		queue_t *pages = qopen();

		char *id_st;
		char *count_st;
		int id;
		int count;

		id_st = strtok(NULL, delimiter);

		do {
			count_st = strtok(NULL, delimiter);
			id = atoi(id_st);
			count = atoi(count_st);
			page_t *page = make_page2(id, count);
			qput(pages, (void *)page);
			//			printf("%d\n", id);
			id_st = strtok(NULL, delimiter);
			
		} while (id_st !=NULL);
		
		n->webpages = pages;
		int try = hput(in, n, n->wordy, strlen(n->wordy));
		if (try !=0) {
			printf("error \n");
		}
		
	}
	
	fclose(fload);
}	
