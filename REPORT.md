# Report: Building a search engine like Google

> [!NOTE]  
> Complete the report in this file before delivering the project.
> The lab guide has a list of the questions you must answer.

You must write a report in “REPORT.md” which contains the following (and only the following)
items:
- C4 component diagram showing the data structures (graphs, indexes, caches, ...), files you
read/write and their relation. Clearly distinguish components which are volatile (stored in
memory) vs persistent (files in disk) using two different colors.
- Table with 3 columns (description, Big-O, justification) with one row for each of the following
(in this exact order):
– Runtime complexity analysis of parsing a document into the struct (including adding
the links to the list)
– Runtime complexity analysis of parsing a query into the struct
– Runtime complexity analysis of counting the neighbours in the graph
– Runtime complexity analysis of counting the neighbours of a document in the graph
– Runtime complexity analysis of finding the documents that contain a keyword in the
reverse-index
– Runtime complexity analysis of finding the documents that match all keywords in the
query
– Runtime complexity analysis of sorting the documents according to the relevance score
- Plot the search time with/without the reverse index for the different-sized datasets and discuss
the results (2 sentences)
- Plot the initialization time for different hashmap slot count settings and discuss the results
(2 sentences)
- Plot the search time for different hashmap slot count settings and discuss the results (2
sentences)
- Describe an improvement of the reverse index to improve search and initialization speed.
Would it require the same, less or more memory overall? And what would the runtime
complexity be (Big-O)? Would it take the same, less or more time to run?
Use markdown to properly format your report in the repository.