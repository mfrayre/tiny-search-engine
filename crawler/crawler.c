/* 
 * crawler.c --- 
 * 
 * Author: Marina Frayre
 * Created: 10-14-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include  "pageio.h"
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

bool searchfn(void* elementp, const void* searchkeyp) {
    webpage_t* data = (webpage_t*)elementp;
    const char* key = (const char*)searchkeyp;
    return (strcmp(webpage_getURL(data), key) == 0);
}

//compare func
bool url_search(void *page_url, const void *search_url) {
	char *p_url = (char *) page_url;
	char *s_url = (char *) search_url;
	if (strcmp(p_url, s_url) == 0) {
		return true;
	}
	return false;
}

void print_webpage_queue(void *elementp) {
	if (elementp == NULL) {
		return;
	}
	webpage_t *page = (webpage_t *)elementp;
	char *url = webpage_getURL(page);
	
	if (url == NULL) {
		return;
	}
	printf("%s\n", url);
	
}

int main(int argc, char *argv[]) {
	
	printf("hello \n");
	//Tests to ensure proper usage of crawler
	//Must be 4 arguments
	if (argc != 4){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}

	char *seed_url = malloc(sizeof(char) * strlen(argv[1]) + 1);
	strcpy(seed_url, argv[1]);
	//char *seed_url = argv[1];
	char *page_dir = argv[2];
	int max_depth= atoi(argv[3]);

	//4th argument must be non-negative
	if (max_depth < 0){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}

	// check that pagedir = valid directory
  struct stat sb;
  if (stat(page_dir, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
		printf("%s is not a valid directory, so making it\n", page_dir);
    mkdir(page_dir, 0700);
	}

	//printf("%s %s %d", seed_url, page_dir, max_depth);
			
	// declare new webpage_t seed
	//	char *url = "https://thayer.github.io/engs50/";
	webpage_t *seed_page = webpage_new(seed_url, 0, NULL);

	if (seed_page == NULL) {
		printf("error - NULL return.");
		exit(1);
	} else {
		printf("successfully created webpage type \n");
	}
	
	// fetch URL to local computer
	if (webpage_fetch(seed_page)) {
		printf("successfully fetched seed page html\n");

	} else {
		printf("failed to fetch \n");
		exit(1);
	}

	// open queue & make hash table of visited pages
	queue_t *url_queue = qopen();
	hashtable_t *url_hashtable = hopen(1000); // is this a good number?

	//put the seed page into hash and the queue
	qput(url_queue, seed_page);
  hput(url_hashtable, seed_url, seed_url, sizeof(seed_url));

	int id = 1; // for directory structure
	int depth; // outside loop; tracks depth
	webpage_t *webpages;

	// iterate through URL's using qget
	while ((webpages = (webpage_t *)qget(url_queue)) != NULL){

		// get depth using webpage_getDepth
		depth = webpage_getDepth(webpages);
		int pos = 0;
		char *q_url = NULL;
    pos = webpage_getNextURL(webpages, pos, &q_url);

		// GO ONLY AS DEEP AS MAXDEPTH
		while (pos > 0 && depth < max_depth) {
			//printf("Found url: %s\n", q_url);

			// ONLY PROCESS INTERNAL URL's
			if (IsInternalURL(q_url)){

				// CHECK TO MAKE SURE NOT ALREADY IN HASH TABLE
        if (hsearch(url_hashtable, &url_search, q_url, strlen(q_url)) == NULL) {

					// create a new webpage at depth + 1
					webpage_t *pg = webpage_new(q_url, depth + 1, NULL);
					
          // ADD TO HASH TABLE
          hput(url_hashtable, q_url, q_url, sizeof(char) * strlen(q_url));
					qput(url_queue, pg);
            
					// save the page under id

					bool real = webpage_fetch(pg);

          if (webpage_getHTMLlen(pg) != 47 && real) { // what is 47??
						pagesave(pg, id, page_dir);
            id++;
					}

					// IF ALREADY IN HASH, free that URL
				} else {
					free(q_url);
				}

				// IF URL IS EXTERNAL, FREE IT
			} else {
				free(q_url);
			}

			// GET NEXT POSITION
      pos = webpage_getNextURL(webpages, pos, &q_url);
		}

		// DELETE WEBPAGE
		webpage_delete(webpages);
    if (depth == max_depth){
			free(q_url);
		}
	}
	
	// free --> seed page
	// close --> queue
	qclose(url_queue);
	// close --> hashtable
	hclose(url_hashtable);
	// exit(EXIT_SUCCESS);

	return 0;
}
