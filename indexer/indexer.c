/* 
 * indexer.c --- 
 * 
 * Author: Marina Frayre
 * Created: 10-24-2023
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
#include <dirent.h>

// Global counter for words
int COUNTER = 0;
int global_ID = 0; 

// Document structure, for each document associated with a word in HASH "index"
typedef struct page {
	int docID;
	int count;
} page_t;

// Word structure, for each element in HASH TABLE "index"
typedef struct word_data {
	char *wordy;
	queue_t *webpages;
} word_t;

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

// Compare Function for if word exists already in "index" HASH
bool word_search(void *word1, const void *word2) {
	word_t *to_compare  = (word_t *)word1;
	char *w1 = to_compare->wordy;
	char *w2 = (char *) word2;
	if (strcmp(w1, w2) == 0) {
		return true;
	}
	return false;
}


// Compare Function for if word exists already in "index" HASH
bool page_id_search(void *element1, const void *element2) {
	page_t *to_compare  = (page_t *)element1;
	int ID1 = to_compare->docID;
	int *ID2 = (int *) element2;
	if (ID1 == *ID2) {
		return true;
	}
	return false;
}

// Global count of all words, including duplicates, from HASH
void get_count(void *elementp) {
	int id = global_ID;
	if (elementp == NULL){
		return;
	}
	word_t *word_element = (word_t *)elementp;
	queue_t *documents = word_element->webpages;

	page_t *to_compare = make_page(id, 0);
	page_t *found_page;

	if ((found_page =(page_t *)qsearch(documents, page_id_search, to_compare)) != NULL ) {
		//printf("count %d \n", found_page->count);
		COUNTER += found_page->count;
	}
	free(to_compare);
}

void free_pages(void *element) {
	if (element == NULL ) {
		return;
	}
	page_t *p = (page_t *)element;
	free(p);

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
	qclose(free_me);
}

void free_words(void *elementp) {
	if (elementp == NULL ) {
		return;
	}
	word_t *w = (word_t *) elementp;
	char *tree = w->wordy;
	free(tree);
	free(w);
}

// Normalization function that removes capitals and returns if invalid word
int NormalizeWord(char *word) {

	char *temp;
	
	if (word == NULL ) {
		return 1;
	}

	if (strlen(word) < 3) {
		//printf("shorter than 3.\n");
		return 1;
	}

	// iterate through pointer to characters
	for (temp = word; *temp ; temp++) {
		if (isupper(*temp)){
			//	printf("found uppercase... replacing with lowercase\n");
			*temp = tolower(*temp);
		}

		if (! isalpha(*temp)) {
			//printf("found non-alphabet... deleting\n");
			return 1;
		}
	}
	
	return 0;
}

bool process_pages(int id, char *dirname, hashtable_t *index) {
	// Use pageload to import document
	webpage_t *loaded_page = pageload(id, dirname);

	if (loaded_page != NULL) {
		printf("successfully loaded \n");
	} else {
		return false;
	}

	// get all HTML, and initialize word string

	char *html =  webpage_getHTML(loaded_page);

	// open file for non normalized words
	FILE *f = fopen("test_words.txt", "w");
	if (f == NULL) {
		printf("error creating file\n");
	}

	// open file for normalized words
	FILE *f2 = fopen("test_normalize.txt", "w");

	// create hash table "index" to store word and their webpages

	int pos = 0;
	do {
		// keep getting position of next word
		char *word = malloc(sizeof(char) * 100);
		pos = webpage_getNextWord(loaded_page, pos, &word);
 
		// print that word to non-normalized File
		fprintf(f, "%s\n", word);

		// If word is "normalizeable", save to normalized file:
		if (NormalizeWord(word) == 0) {
			fprintf(f2, "%s\n", word);

			// If word is not in hashtable, add it and make a queue
			word_t *temp = (word_t*)hsearch(index, &word_search, word, strlen(word)); //pointer to word
			if (temp == NULL){
				//printf("found new word \n");

				word_t *new_word = malloc(sizeof(word_t)); // FREE'D Later in HAPPLY?
        new_word->wordy = word;

				// make queue and put into new_word
				queue_t *pages = qopen();
				page_t *page = make_page(id, 1); // free'd later in happly iterates through queues, which contain pages 
				qput(pages, (void *)page);
				new_word->webpages=pages;
						
        hput(index, new_word, new_word->wordy, strlen(new_word->wordy));
			}

			// If word is in hashtable, then temp points to word_t,  increment the count by one
			else{

				// see if there is already an associated webpage queue with it
				// if so, adjust counter. If not, add to end of queue.
				queue_t *temp_queue = temp->webpages;
				page_t *id_page = make_page(id, 1); // FREE'D either below or later in HAPPLY
				if (qsearch(temp_queue, page_id_search, id_page) != NULL){
					//printf("word already has associated webpage %d. Adjusting Counter\n", id);
					page_t *temp_page = (page_t*)qsearch(temp_queue, page_id_search, id_page);
					temp_page->count++;
					free(id_page);
				} else { // add to end of queue
					//printf("page not found in queue, so adding \n");
					qput(temp_queue, (void *)id_page);
				}
			}
		}
		
	} while (pos < strlen(html));
	//free(word);
	fclose(f2);
	fclose(f);

	webpage_delete(loaded_page);

	return true;
}



int main(int argc, char *argv[]){
	// DIRECTORY OF PAGES
	//char *dirname = "../crawler/test_files2/";
	//	int id = 1;
	//	int id = atoi(argv[1]);

	char *dir_open = malloc(sizeof(char) * strlen(argv[1]) + 1);
 	char *file_save = malloc(sizeof(char) * strlen(argv[2]) + 1);
	strcpy(dir_open, argv[1]);
	strcpy(file_save, argv[2]);

	// get COUNT 
	int file_count = 0;
	DIR * dirp = opendir(dir_open);
	struct dirent * entry;
	
	if (dirp == NULL) {
		perror("opendir");
    exit(EXIT_FAILURE);
	}
 
	while ((entry = readdir(dirp)) != NULL) {
		//		if (entry->d_type == DT_REG) { 
			file_count++;
			//}
	}

	closedir(dirp);
	printf("%d \n", file_count);
	int id = file_count;
	
	hashtable_t *index = hopen(1024);

	do {
		process_pages(id, dir_open, index);
		global_ID = id;
		happly(index, get_count);
		printf("number of words, aggregated with happly: %d\n", COUNTER);
		COUNTER = 0;
		id--;
		
	} while (id > 0);

	// SAVE INDEX TO FILE
	indexsave(index, file_save);

	// FREE STUFF 
	happly(index, free_queues);
	happly(index, free_words);
	hclose(index);
	free(file_save);
	free(dir_open);
	// char testme[] = is better than char *test = 
	//char testme[] = "#ASDFo";

	return 0;
}

