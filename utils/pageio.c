/* 
 * pageio.c --- 
 * 
 * Author: Marina Frayre
 * Created: 10-23-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "pageio.h"


int32_t pagesave(webpage_t *pagep, int id, char *dirname) {

	int max_id_len = 32;

    // strip off the trailing slash of dirname, if it exists
    char *lastchar =  &dirname[strlen(dirname) - 1];
		char *new_dirname = malloc(sizeof(char) * strlen(dirname) + 1);
    if (strcmp("/", lastchar) == 0) {
        strcpy(new_dirname, dirname);
		new_dirname[strlen(new_dirname)-1] = 0;
    }
	else {
		strcpy(new_dirname, dirname);
	}
    // getitng html from the webpage
    char *html = webpage_getHTML(pagep);
    int html_len = webpage_getHTMLlen(pagep);
    // get --> url from the webpage
    char *url = webpage_getURL(pagep);
    // get --> depth from the webpage
    int depth = webpage_getDepth(pagep);

    char *fname = malloc(sizeof(char) * strlen(new_dirname) + sizeof(char) * max_id_len + 1);
    sprintf(fname, "%s/%d", new_dirname, id);

	//check if directory exists, if not, create it
	DIR* dir = opendir(new_dirname);
	if (ENOENT == errno){
		//directory = does not exist
		mkdir(new_dirname, 0700);
	}
	closedir(dir);

	// check if it's possible to write to the directory
	if (access(new_dirname, W_OK) != 0) {
		printf("cannot access %s\n", new_dirname);
		chmod(new_dirname, W_OK);
	}

	//make the file and check if everything went right
    FILE *f = fopen(fname, "w");
    if (f == NULL) {
        printf("failed to open file %s\n", fname);
		printf("Error %d \n", errno);
        return -1;
    }

    fprintf(f, "%s\n", url);
    fprintf(f, "%d\n", depth);
    fprintf(f, "%d\n", html_len);
    fprintf(f, "%s\n", html);
    fclose(f);
		free(new_dirname);
		free(fname);
    return 0;
}


/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm) {
	char file_path[sizeof(dirnm)+100];
	sprintf(file_path, "%s%d", dirnm, id);
	printf("Loading the following page: %s\n", file_path);
	
	FILE *file = fopen(file_path, "r");
	if (file == NULL) {
		printf("Issues finding or opening the specified file");
		return NULL;
	}

	char url[1000];
	int depth;
	int html_length;
	
	// get first line, which corresponds to url
	if (fgets(url, sizeof(url), file) != NULL){
		url[strlen(url)-1] = '\0';
		printf("retrieved URL %s\n", url);
	} else {
		return NULL;
	}

	// get second line, wich is the page depth
	if (fscanf(file, "%d", &depth) == 1 ) {
		printf("retrieved depth %d\n", depth);
	} else {
		return NULL;
	}

	if (fscanf(file, "%d", &html_length ) == 1 ) {
		printf("there are %d characters of html\n", html_length);
	} else {
		return NULL;
	}

	char *html = malloc(sizeof(char) * html_length + sizeof(char) *  1);
//	char html[html_length+1];
	int char_count = 0;
	int c;

	// Read the remaining content directly into 'buffer'
	while (char_count < html_length && (c = fgetc(file)) != EOF) {
		html[char_count++] = c;
	}

	//memmove(html, html + 1, strlen(html));
		
	webpage_t *loaded_page;
	loaded_page = webpage_new(url, depth, html);

	//	free(html);
	fclose(file);
	return loaded_page;
	
}
