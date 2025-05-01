#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
// unit test 1
void test_single_keyword() {
    Document* doc1 = create_document(1, "Doc 1", "This document talks about cats.");
    Document* doc2 = create_document(2, "Doc 2", "No animals here.");
    doc1->next = doc2;

    char* query[] = { "cat" }; //define the keyword
    int matched = 0;
    //we go trough the list of documents
    Document* current = doc1;
    while (current) {
        int matches = 1;
        for (int i = 0; i < 1; i++) { //we make a loop trough
            if (!contains_substring(current->body, query[i])) {
                matches = 0;//if it doesn't containt it false
                break;
            }
        }
        if (matches) matched++; //if it matches we count
        current = current->next; //pass to the next
    }

    if (matched == 1) printf("Test 1 PASS\n"); //if matches the test has passed
    else printf("Test 1 FAIL\n");

    free_documents(doc1);//free the memory
}
//unit test 2
void test_multiple_keyword() {
    Document* doc1 = create_document(1, "Doc 1", "Dogs and cats are friendly.");
    Document* doc2 = create_document(2, "Doc 2", "I only have dogs.");
    doc1->next = doc2;

    char* query[] = { "dogs", "cats" }; // serch for both keywords
    int matched = 0;

    Document* current = doc1;
    while (current) {
        int matches = 1;
        for (int i = 0; i < 2; i++) {//loop to search for the keywords
            if (!contains_substring(current->body, query[i])) { //if it doesn´t match
                matches = 0;//false
                break;
            }
        }
        if (matches) matched++; //if it matches we count
        current = current->next;//we pass to the next
    }

    if (matched == 1) printf("Test 2 PASS\n"); //if matches the test has passed
    else printf("Test 2 FAIL\n");

    free_documents(doc1);//free memory
}

//unit test 3 
void test_no_match() {
    Document* doc1 = create_document(1, "Doc 1", "This document is about birds.");
    Document* doc2 = create_document(2, "Doc 2", "I love fish.");
    doc1->next = doc2;

    char* query[] = { "cat" }; //search for a random word that doesn´t appear
    int matched = 0;

    Document* current = doc1;
    while (current) {
        int matches = 1;
        for (int i = 0; i < 1; i++) { //loop trough to find
            if (!contains_substring(current->body, query[i])) { //if not found
                matches = 0;
                break;
            }
        }
        if (matches) matched++;//if it matches we count
        current = current->next;
    }

    if (matched == 0) printf("Test 3 PASS\n");
    else printf("Test 3 FAIL\n");

    free_documents(doc1);//free memory
}
