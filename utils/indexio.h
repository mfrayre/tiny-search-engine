#pragma once
/* 
 * indexio.h --- 
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

void indexsave(hashtable_t *index, char *dirnm); 


void indexload(hashtable_t *index, char *dirnm);
