# Tiny Search Engine

A search engine built from scratch in C, developed for Dartmouth's COSC50/ENGS50 (Software Design and Implementation). The engine crawls web pages, builds an inverted index of word frequencies, and answers ranked search queries with AND/OR operator support.

## Components

### Crawler
Fetches web pages starting from a seed URL, following internal links up to a specified depth. Each page is saved to a local directory with a unique numeric ID.

```
./crawler <seedurl> <pagedir> <maxdepth>
```

Example:
```
./crawler https://thayer.github.io/engs50/ ./pages 2
```

### Indexer
Reads the pages saved by the crawler and builds an inverted index: a hash table mapping each word to the documents it appears in and how many times.

```
./indexer <pagedir> <indexfile>
```

### Querier
Loads the index and answers search queries interactively. Supports AND/OR boolean operators (AND has higher precedence). Results are printed in ranked order.

```
./query <pagedir> <indexfile> [-q]
```

The optional `-q` flag reads queries from stdin (useful for batch testing):
```
./query ./pages index.txt -q < queries.txt
```

Example query session:
```
> dartmouth college
rank: 5: doc: 3 : https://thayer.github.io/engs50/...
rank: 2: doc: 1 : https://thayer.github.io/engs50/

> dartmouth or mit
rank: 9: doc: 2 : https://thayer.github.io/engs50/...
```

## Building

Each component has its own Makefile. Build the shared utility library first, then each component:

```bash
cd utils && make
cd ../crawler && make
cd ../indexer && make
cd ../querier && make
```

## Project Structure

```
.
├── crawler/       # Web crawler
├── indexer/       # Inverted index builder
├── querier/       # Query processor and ranker
├── utils/         # Shared library: queue, hash table, page I/O, index I/O, webpage fetching
├── lib/           # Compiled utility library (libutils.a)
└── test/          # Module-level tests for pageio and indexio
```

## Key Concepts

- **Crawler**: BFS over internal URLs using a queue of pages and a hash table of visited URLs to avoid duplicates
- **Indexer**: Normalizes words (lowercase, alphabetic only, length ≥ 3) and builds a hash table of word → `{docID, count}` lists
- **Querier**: Parses query grammar (`andsequence [or andsequence]...`), ranks by minimum AND-count summed across OR-groups, and retrieves page URLs from the crawler directory

## Collaborators

Built for Dartmouth COSC50/ENGS50 (Fall 2023).
Primary author: **Marina Frayre**
Collaborators: **Derek Pickell**, **Avery** (lab partners)

Course-provided modules: `webpage.c/h` (Stephen Taylor), `pageio.h` interface (Stephen Taylor)
