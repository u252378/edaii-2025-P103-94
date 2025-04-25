#ifndef SEARCH_H
#define SEARCH_H

#include "document.h"
#include "query.h"

Document* search_documents(const Document* all_docs, const Query* query, int max_results);

#endif