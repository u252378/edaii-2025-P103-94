//in this file we will create the functions: document_desserialize and load_documents_from_folder
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "document.h"
#include <dirent.h>  //to handle directory operations like opendir, readdir, closedir.  
#include <sys/stat.h>

//helper to create a new Link node
Link* create_link(int id) {
    Link* newLink = malloc(sizeof(Link)); //allocates memory for a new Link node.
    if (!newLink) return NULL; //return NULL if memory allocation fails. 
    newLink->id = id; //set the link's ID
    newLink->next = NULL; //initializes the next pointer to NULL (end of the list).
    return newLink;
}

//FUNCTION PARSE:
//parse a single document file:
Document *document_desserialize(char *path) { 
    FILE* file = fopen(path, "r"); //open the document in read mode
    if (!file) return NULL; //check that it has opened correctly
    
    Document *doc = (Document*)malloc(sizeof(Document)); //allocate memory for a new document   
    if (!doc) { // if the memory allocation fails we close the file and return NULL
        fclose(file);
        return NULL;
    }
    
    char buffer[262144]; //creation of a buffer
    int bufferSize = 262144;
    int bufferIdx = 0; //index for entering the buffer
    char ch; //to read characters from the file

    //parse id:
    while ((ch = fgetc(file)) != '\n') { //this will read characters until newline
        assert(bufferIdx < bufferSize); //we do this in order to prevent overflow
        buffer[bufferIdx++] = ch; //stores char in the buffer
    }
    assert(bufferIdx < bufferSize); //buffer safety check
    buffer[bufferIdx++] = '\0'; //null terminate string 
    doc->doc_id = atoi(buffer); //converts buffer to integer and store as document ID

    //parse title:
    bufferIdx = 0; //in order to prepare to read the next line (title) we reset the buffer index
    while ((ch = fgetc(file)) != '\n') { //read characters from the file until a new line is found
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    doc->title = strdup(buffer); //same as in id, but it allocates memory and copies the title string into doc->title

    //parse body
    char linkBuffer[64]; //buffer to store link ID strings
    int linkBufferSize = 64;
    int linkBufferIdx = 0; //index for link buffer
    bool parsingLink = false; //flag for link parsing state
    /*Link *link = LinksInit();*/ //what is this for?????
    Link *head = NULL, *tail = NULL; //head and tail of linked list of links
    
    bufferIdx = 0;
    while ((ch = fgetc(file)) != EOF) { //reads until EOF (end of file)
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch; //adds character to body buffer
        if (parsingLink) { //parsing a link ID
            if (ch == ')') { //end of link (ends with ")")
                parsingLink = false;
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = '\0'; //null terminate link ID string
                int linkId = atoi(linkBuffer); //converts to int

                //add to links:
                Link* newLink = create_link(linkId);
                if (!newLink) { //in case creation failed, it will clean up and exit
                    fclose(file);
                    free(doc->title);
                    free(doc);
                    return NULL;
                }
                if (!head) { //first link in the list
                    head = tail = newLink;
                } else { //append to list
                    tail->next = newLink;
                    tail = newLink;
                }

                linkBufferIdx = 0; //reset for next link
            } else if (ch != '(') { //skip first parenthesis of the link
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = ch; //store link character
            } 
        } else if (ch == ']') { //found beginning of link id. ex: [my link text](123)
          parsingLink = true;
        }
    }
    assert(bufferIdx < bufferSize); //this ensures the buffer is not full
    buffer[bufferIdx++] = '\0'; //null terminates the body
    
    //copy body content from buffer:
    char *body = (char *)malloc(sizeof(char) * bufferIdx); //allocates memory for the body.
    strcpy(body, buffer); //copy the buffer content into the allocated body memory.

    //assign body and links to document
    doc->body = body;
    doc->links = head;
    doc->relevance = 0.0; //default relevance value

    fclose(file); //close the file
    return doc; //return the filled Document struct
}

//load all documents from a folder:
Document* load_documents_from_folder(const char* folder_path) {
    DIR* dir = opendir(folder_path); //open the directory specified by folder_path. 
    if (!dir) return NULL; //check if the directory can be open, if not return NULL.

    struct dirent* entry; //declare a pointer to hold the directory entries
    Document* head = NULL;
    Document* tail = NULL;

    while ((entry = readdir(dir)) != NULL) { //loop through each entry
        char fullpath[1024]; //array to store ful path of the file.
        snprintf(fullpath, sizeof(fullpath), "%s/%s", folder_path, entry->d_name); //create full file path.
        struct stat path_stat;
        if (stat(fullpath, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
        Document* doc = document_desserialize(fullpath); //desserialize the doc.
        if (doc) {
            if (!head) head = tail = doc; //if list is empty, set head and tail to the new document.
            else { //if list is NOT empty, append the document to the list
                tail->next = doc; //set the current tail's next to the new doc.
                tail = doc; //update the tail to point to the new document.
            }
        }
    }
}
    closedir(dir);
    return head;
}

//print details of a document:
void print_document_details(const Document* doc) {
    printf("\n=== Document ID %d ===\n", doc->doc_id);
    printf("Title: %s\n", doc->title);
    printf("Body:\n%s\n", doc->body);
    printf("Links:\n");

    const Link* link = doc->links; //pointer to traverse the links.
    while (link) {
        printf("  -> Document ID %d\n", link->id); //print each link's ID.
        link = link->next; //move to the next link in the list. 
    }
}

//free all allocated memory for a list of documents:
void free_documents(Document* head) {
    while (head) {
        Document* next = head->next; //save next doc in the list.

        free(head->title);
        free(head->body);

        Link* link = head->links;
        while (link) {
            Link* tmp = link->next; //save next link.
            free(link);
            link = tmp; //move to next link.
        }

        free(head);
        head = next; //move to next doc in the list.
    }
}