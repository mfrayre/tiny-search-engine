/* 
 * test_pageio.c --- 
 * 
 * Author: Marina Frayre
 * Created: 10-24-2023
 * Version: 1.0
 * 
 * Description: for testing pageio load page
 * 
 */
#include <stdio.h>
#include "pageio.h"
#include "queue.h"
#include "hash.h"
#include "webpage.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	printf("hello\n");
	// int32_t pagesave(webpage_t *pagep, int id, char *dirme);
	// webpage_t *pageload(int id, char *dirnm);
	char *dirname = "../crawler/test_files2/";
	int id = 1;

	//webpage_t *loaded_page = webpage_new("www.blank.com", 0, NULL);
	webpage_t *loaded_page = pageload(id, dirname);

	if (loaded_page != NULL ){
		printf("successfully loaded\n");
		printf("%s\n", webpage_getHTML(loaded_page));
	} else {
		return 1;
	}
	// save html to current directory
	char current_directory[500];
  getcwd(current_directory, sizeof(current_directory)); 
	printf("current directory %s\n", current_directory);
	pagesave(loaded_page, id, current_directory);

	// now load again and compare
	//	webpage_t *loaded_again = pageload(id, current_directory);

	//printf("%s", webpage_getHTML(loaded_again));
	//	int compare = strcmp();
	printf("made it here");
}
