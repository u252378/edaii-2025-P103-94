#ifndef SORT_SEARCH_H
#define SORT_SEARCH_H
#include "document.h"

Document* sort_documents_by_relevance(Document* head);
void print_sorted_documents(const Document* head);

#endif