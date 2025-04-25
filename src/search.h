#ifndef SEARCH_H //we include guard to prevent multiple inclusion of this header file.
#define SEARCH_H

#include "document.h"
#include "query.h"

Document* search_documents(const Document* all_docs, const Query* query, int max_results);

#endif