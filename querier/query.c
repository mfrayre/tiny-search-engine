/* 
 * query.c --- 
 * 
 * Author: Marina Frayre
 * Created: 11-02-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "indexio.h"
#include "hash.h"
#include "queue.h"

int DOC;
int RANK;

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


void print_word(void *el) {
	word_t *w = (word_t *)el;
	printf("%s \n", w->wordy);
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


void remove_extra_spaces(char *str) {
    int i, j;
    int insideWord = 0; // Flag to track if currently inside a word

    for (i = 0, j = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\t') {
            str[j++] = str[i];
            insideWord = 1;
        } else {
            if (insideWord) {
                str[j++] = ' ';
                insideWord = 0;
            }
        }
    }

    str[j] = '\0'; // Add null terminator at the end of the modified string
}


// Normalization function that removes capitals and returns if invalid string
int NormalizeString(char *word) {

	// remove spaces and tabs
	remove_extra_spaces(word);
	char *temp;
	
	if (word == NULL ) {
		return 1;
	}

	// iterate through pointer to characters
	for (temp = word; *temp ; temp++) {
		if (isupper(*temp)){
			//	printf("found uppercase... replacing with lowercase\n");
			*temp = tolower(*temp);
		}

		// return if non alphabetical (excluding spaces)
		if (! isalpha(*temp) && *temp != ' ') {
			//printf("found non-alphabet... deleting\n");
			return 1;
		}
	}
	
	return 0;
}

// strips new line so that ENTER can be detected
void remove_newline(char *word){
	for (int i = 0; word[i] != '\0'; i++) {
		if (word[i] == '\n') {
			word[i] = '\0';
			break;
		}
	}
}

void get_counts(void *p) {
	if (p == NULL ) {
		return;
	}
	page_t *page = (page_t *)p;
	if (page->docID == DOC) {
		printf("%d ", page->count);
		if (page->count < RANK || RANK == 0) {
			RANK = page->count;
		}
	} else {
		printf("count: 0 ");
	}
}


// Finds all words in INDEX that correspond to the string query
void print_compare(hashtable_t *index, char *word, int document) {
	char *temp = word;
	char *hold = strtok(temp, " ");

	do {
		if (!(strcmp(hold, "and")==0  || strcmp(hold, "or") == 0 || strlen(hold) <3)) {
	
				printf("%s: ", hold);

				word_t *temp = (word_t*)hsearch(index, &word_search, hold, strlen(hold)); //pointer to word
				if (temp == NULL) {
					printf("0 ");
				} else {
					page_t *page = (page_t *) temp->webpages;
					if (page !=NULL) {
						DOC = document;
				
						qapply(temp->webpages, get_counts);

					}
				}
			}
		hold = strtok(NULL, " ");
	}while (hold != NULL);
}
	

int main() {

	char input[1000];
	//	keepGoing = true;
	while (1) {
		printf("> ");

		// CHECK IF CNTRL-D PRESSED:
		if (fgets(input, sizeof(input), stdin) == NULL) {
			printf("Exiting \n");
			break;
		}

		// CHECK IF 'ENTER' PRESSED:
		if(input[0] == '\n') {
			continue;
		} else { // ELSE, print INPUT:						

			// remove \n character:
			remove_newline(input);
			int check = NormalizeString(input);

			if (check == 0) { // then there are no 'sketchy' characters
				hashtable_t *index = hopen(1024);
				indexload(index, "step2_query");
				RANK=0;
				print_compare(index, input, 1);
				if (RANK != 0) {
					printf("rank: %d \n", RANK);
				}
			} else {
				printf("[invalid query]\n");
			}				 			
		}
	}
	
	return 0;
}

