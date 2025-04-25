//in this file we will create all functions and data structures of lab 1:

//DOCUMENT STRUCT:
typedef struct {
    int id;
    struct Link* next;
  } Link;
  
  typedef struct {
    int doc_id;
    char* title;
    char* body;
    Link* links;
    float relevance;
  } Document;


//FUNCTION PARSE:
Document *document_desserialize(char *path) {
    FILE* file = fopen(path, "r");
    assert(f != NULL);
    
    Document *doc = (Document*)malloc(sizeof(Document));
    if (!doc) {
        fclose(file);
        return NULL;
    }
    
    char buffer[262144];
    int bufferSize = 262144;
    int bufferIdx = 0;
    char ch;

    //parse id:
    while ((ch = fgetc(f)) != '\n') {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    document->id = atoi(buffer);

    //parse title:
    bufferIdx = 0;
    while ((ch = fgetc(file)) != '\n') {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    doc->title = strdup(buffer); //strdup allocates memory and copies the string

    // parse body
    char linkBuffer[64];
    int linkBufferSize = 64;
    int linkBufferIdx = 0;
    bool parsingLink = false;
    Links *links = LinksInit();
    
    bufferIdx = 0;
    while ((ch = fgetc(f)) != EOF) {
        assert(bufferIdx < bufferSize);
        buffer[bufferIdx++] = ch;
        if (parsingLink) {
            if (ch == ')') { //end of link
                parsingLink = false;
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = '\0';
                int linkId = atoi(linkBuffer);

                //add to links:
                Link* newLink = createLink(linkId);
                if (!newLink) {
                    fclose(file);
                    free(doc->title);
                    free(doc);
                    return NULL;
                }
                if (!head) {
                    head = tail = newLink;
                } else {
                    tail->next = newLink;
                    tail = newLink;
                }

                linkBufferIdx = 0;
            } else if (ch != '(') { // skip first parenthesis of the link
                assert(linkBufferIdx < linkBufferSize);
                linkBuffer[linkBufferIdx++] = ch;
            } 
        } else if (ch == ']') { // found beginning of link id, e.g.: [my link text](123)
          parsingLink = true;
        }
    }
    assert(bufferIdx < bufferSize);
    buffer[bufferIdx++] = '\0';
    
    char *body = (char *)malloc(sizeof(char) * bufferIdx);
    strcpy(body, buffer);

    // TODO
}