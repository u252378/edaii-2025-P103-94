#ifndef SORT_SEARCH_H
#define SORT_SEARCH_H

#include "document.h"

Document *sort_documents_by_relevance(Document *head, const char *keyword);
void print_sorted_documents(const Document *head);
float calculate_relevance(Document *doc, const char *keyword);
void sort_documents_list(DocumentsListNode **headRef);

#endif