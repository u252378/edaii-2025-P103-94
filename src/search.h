Document *load_documents_from_folder(const char *folder_path); // loads documents
void free_documents(Document *docs); // frees the memory
void print_document_details(const Document *doc); // prints the details of a document
#endif // DOCUMENT_H
