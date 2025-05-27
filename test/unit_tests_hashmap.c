# include <stdio.h>
# include <stdlib.h>
# include "../src/hashmap.h"


// Test 1 (insert and retrieve a document into the hashmap)
int insert_retrive_into_from_hashmap() {

    // create a hashmap with 1 slots
    HashMap* hashmap = createHashMap(1);

    // allocate space for one document that will be inserted in the hashmap created above
    Document* document_test_insert = malloc(sizeof(Document));
    if (document_test_insert == NULL) { // handling memory allocation fail
        printf("Memory allocation failed\n");
        freeHashMap(hashmap);
        return 0;
    }

    // inserting document created for the test under the name "document_1" into the hashmap created for the test
    insertToHashMap(hashmap, "document_1", document_test_insert);

    // retrieve document under the name "document_1" from hashmap
    Document* document_test_retrieve = getFromHashMap(hashmap, "document_1");

    // checking document inserted and retrieved is the same, and if so then the test was successfull
    if (document_test_insert == document_test_retrieve) {
        printf("Test 1 passed\n");
        // then after test, the hashmap and document created are free
        freeHashMap(hashmap);
        free(document_test_insert);
        return 1;
    } else {
        printf("Test 1 failed\n");
        // then after test, the hashmap and document created are free
        freeHashMap(hashmap);
        free(document_test_insert);
        return 0;
    }
}





// Test 2 (overwrite existing document under the same name (that is key))
int overwrite_key_hashmap() {

    //creating a hashmap with 2 slots
    HashMap* hashmap = createHashMap(2);

    // allocating memory for two documents
    Document* document1 = malloc(sizeof(Document));
    Document* document2 = malloc(sizeof(Document));

    if(document1 == NULL || document2 == NULL) { // handling memory allocation fail
        printf("Memory allocation failed\n");
        if (document1 != NULL) {
            free(document1);
        }
        if(document2 != NULL) {
            free(document2);
        }
        freeHashMap(hashmap);
        return 0;
    }

    /*inserting both documents 1 and 2 into hashmap, under the name of "key" both, 
    so then when inserting the document2 then the stored pointer for this key is updated to document2*/
    insertToHashMap(hashmap, "key", document1);
    insertToHashMap(hashmap, "key", document2);

    // then when retrieving the document under the name "key" then it should get document2
    Document* document_retrieve = getFromHashMap(hashmap, "key");

    if (document_retrieve == document2) {
        printf("Test 2 passed\n");
        // then after te test, the hashmap and both documents are free
        freeHashMap(hashmap);
        free(document1);
        free(document2);
        return 1;
    } else {
        printf("Test 2 failed\n");
        // then after te test, the hashmap and both documents are free
        freeHashMap(hashmap);
        free(document1);
        free(document2);
        return 0;
    }
}






// Test 3 (collision chain handling test)
int collision_chain() {

    // create hashmap with 1 slots
    HashMap* hashmap = createHashMap(1);

    // allocating memory for three documents
    Document* doc1 = malloc(sizeof(Document));
    Document* doc2 = malloc(sizeof(Document));
    Document* doc3 = malloc(sizeof(Document));

    if(doc1 == NULL || doc2 == NULL ||doc3 == NULL) { // handling memory allocation fail
        printf("Memory allocation failed\n");
        if (doc1 != NULL) {
            free(doc1);
        }
        if(doc2 != NULL) {
            free(doc2);
        }
        if (doc3 != NULL) {
            free(doc3);
        }
        freeHashMap(hashmap);
        return 0;
    }

    /*insert the documents doc1, doc2, doc3 into hashmap,
    and since there is only one slot then there will be collisions,
    and these are handled by having a linked list in the bucket
    */
    insertToHashMap(hashmap, "key1", doc1);
    insertToHashMap(hashmap, "key2", doc2);
    insertToHashMap(hashmap, "key3", doc3);

    // then retrieve each document from hashmap to check that they were correctly stored in the hashmap
    Document* retrieve_doc1 = getFromHashMap(hashmap, "key1");
    if (retrieve_doc1 != doc1) {
        printf("Test 3 failed\n");
        // then after the test the hashmap and the three documents are free
        freeHashMap(hashmap);
        free(doc1);
        free(doc2);
        free(doc3);
        return 0;
    }

    Document* retrieve_doc2 = getFromHashMap(hashmap, "key2");
    if (retrieve_doc2 != doc2) {
        printf("Test 3 failed\n");
        // then after the test the hashmap and the three documents are free
        freeHashMap(hashmap);
        free(doc1);
        free(doc2);
        free(doc3);
        return 0;
    }

    Document* retrieve_doc3 = getFromHashMap(hashmap, "key3");
    if (retrieve_doc3 != doc3) {
        printf("Test 3 failed\n");
        // then after the test the hashmap and the three documents are free
        freeHashMap(hashmap);
        free(doc1);
        free(doc2);
        free(doc3);
        return 0;
    }

    // then after the test the hashmap and the three documents are free
    freeHashMap(hashmap);
    free(doc1);
    free(doc2);
    free(doc3);

    printf("test 3 passed\n");
    return 1;
}



int main() {
    int result = 0;
    result += insert_retrive_into_from_hashmap();
    result += overwrite_key_hashmap();
    result += collision_chain();
    printf("The number of test passed were %d\n", result);
}