# include <stdio.h>
# include <stdlib.h>
# include "../src/document.h"

// funciton to free link linked list
void free_link_list(Link* head) {
    
    // iterates until there are elements in the link linked list
    while (head != NULL) {
        // sets pointer next to point to next of head
        Link *next = head -> next;
        free(head);
        // then when free the head then the head is set to next value that was saved above
        head = next;
    }
}




// Test 1 (one link in linked list)
int test1() {
    Link* link = create_link(1);
    
    // checking if link was correctly created
    if (link == NULL) {
        printf("Test 1 failed\n");
        return 0;
    }
    if (link -> id != 1) {
        printf("Test 1 failed\n");
        return 0;
    }
    if (link -> next != NULL) {
        printf("Test 1 failed\n");
        return 0;
    }

    // then if test 1 is passed, then an appropiate message is displayed and it is returned 1
    printf("Test 1 passed\n");
    return 1;
}





// Test 2 (two links in linked list)
int test2() {
    Link *link1 = create_link(1);
    Link *link2 = create_link(2);

    // checking if link linked list was created sucessfully
    if (link1 == NULL || link2 == NULL) {
        printf("Test 2 failed\n");
        if (link1 != NULL) {
            free(link1);
        } else if (link2 != NULL) {
            free(link2);
        }
        return 0;
    }

    // link1 next pointer now points to link2
    link1 -> next = link2;

    if (link1 -> next != link2 || link2 -> next != NULL) {
        printf("Test 2 failed\n");
        return 0;
    }

    if (link1 -> id != 1 || link2 -> id != 2) {
        printf("Test 2 failed\n");
        return 0;
    }

    // freeing the link linked list
    free_link_list(link1);

    printf("Test 2 passed\n");
    return 1;
}






// Test 3 (three links in linked list)
int test3() {
    Link *link1 = create_link(1);
    Link *link2 = create_link(2);
    Link *link3 = create_link(3);

    if(link1 == NULL || link2 == NULL || link3 == NULL) {
        printf("Test 3 failed\n");
        if (link1 != NULL) {
            free(link1);
        } else if (link2 != NULL) {
            free(link2);
        } else if (link3 != NULL) {
            free(link3);
        }
        return 0;
    }

    // next pointer of link1 points to link2, and next pointer of link2 points to link3
    link1 -> next = link2;
    link2 -> next = link3;

    if (link1 -> id != 1 || link2 -> id != 2 || link3 -> id != 3) {
        printf("Test 3 failed\n");
        return 0;
    }

    if (link1 -> next != link2 || link2 -> next != link3 || link3 -> next != NULL) {
        printf("Test 3 failed\n");
        return 0;
    }


    free_link_list(link1);

    printf("test 3 passed\n");

    return 1;
}


int main() {
    int result = 0;
    result += test1();
    result += test2();
    result += test3();

    printf("The number of tests passed is %d", result);

    return 0;
}

