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


                A potential improvement that could be made to the reverse index code while maintaining the current implementation of separate chaining is to implement dynamic resizing. 

                This improvement is based on the idea of resizing the array of buckets so that when reaching a certain load factor, then this array will be expanded to have more available buckets on which to store the different linked lists. The origin of this change lies in the conflict that arises from having a fixed number of slots where the different linked lists grow and grow without taking into account the amount of documents that are inserted into these. Therefore when having longer chains, the performance is decreased, as search and insertion operations require traversing more nodes until reaching the desired ones.

                This improvement could be implemented by keeping track of the current load factor of the array, that is by considering a load factor such as (number_keywords / slotsCount). At the same time with also having a predefined threshold to know when it is needed to make use of rehashing, for example having a fixed threshold of 0.75. 

                Moreover, when rehashing the amount of extra space to have, could be defined as the double of the current size of the array slots. This process implementation would involve allocating a new and larger bucket array, and rehashing all existing keys to determine their new positions on the new bucket array. After inserting each key in its corresponding place in the new bucket array, then the memory allocated for the old table is freed.

                The main benefit of this consideration is that it keeps the average chain length bounded, so both reverseIndexPut and reverseIndexGet run in O(1) average time complexity. This is because documents in the linked lists will be evenly distributed between all the buckets, therefore there are not long chains. Although rehashing is an expensive operation in terms of time with O(n) time complexity, it is only done on demand and not on a frequent basis. However rehashing compensates for much faster insertion and searching operations that may be done later, therefore helping the overall efficiency of the program.

                However, dynamic resizing comes at a space cost. This is because it is needed to constantly allocate a larger bucket array and then when this is done, free the old bucket array. Therefore both exist in memory for a short period of time, increasing memory usage. Then after freeing the old bucket array, some space is released, however the new bucket array is already in memory which occupies double the size of the previous one, so the program uses more memory each time rehashing is necessary.

                Furthermore, the time complexity of this implementation is as follows:
                Insert and search for documents in the table: O(1) average time complexity, and O(n) worst time complexity, where n is the number of keywords.
                Rehashing to have a bigger bucket array: O(n), where n is the number of keywords.
                Therefore this implementation requires more time because it is needed to rehash the table in some moments, however after having this rehashing process done then it takes less time to do all the insertion and searching operations.

                In conclusion, making use of this potential implementation, makes it easier to search through the linked lists for a specific document. Additionally, since the table starts with a small size then this improves initialisation speed, and resizing on demand helps in efficiently handling growing amounts of data. However this rehashing process requires extra space in memory.






Use markdown to properly format your report in the repository.









































// Reverse index improvement: describe an improvement of the reverse index to improve search and initialisation speed
// Reverse index improvement: would it require the same, less or more memory overall?
// Reverse index improvement: and what would be the runtime complexity be (Big-O). Would it take the same, less or more time to run?

A potential improvement that could be made to the reverse index code while maintaining the current implementation of separate chaining is to implement dynamic resizing. 

This improvement is based on the idea of resizing the array of buckets so that when reaching a certain load factor, then this array will be expanded to have more available buckets on which to store the different linked lists. The origin of this change lies in the conflict that arises from having a fixed number of slots where the different linked lists grow and grow without taking into account the amount of documents that are inserted into these. Therefore when having longer chains, the performance is decreased, as search and insertion operations require traversing more nodes until reaching the desired ones.

This improvement could be implemented by keeping track of the current load factor of the array, that is by considering a load factor such as (number_keywords / slotsCount). At the same time with also having a predefined threshold to know when it is needed to make use of rehashing, for example having a fixed threshold of 0.75. 

Moreover, when rehashing the amount of extra space to have, could be defined as the double of the current size of the array slots. This process implementation would involve allocating a new and larger bucket array, and rehashing all existing keys to determine their new positions on the new bucket array. After inserting each key in its corresponding place in the new bucket array, then the memory allocated for the old table is freed.

The main benefit of this consideration is that it keeps the average chain length bounded, so both reverseIndexPut and reverseIndexGet run in O(1) average time complexity. This is because documents in the linked lists will be evenly distributed between all the buckets, therefore there are not long chains. Although rehashing is an expensive operation in terms of time with O(n) time complexity, it is only done on demand and not on a frequent basis. However rehashing compensates for much faster insertion and searching operations that may be done later, therefore helping the overall efficiency of the program.

However, dynamic resizing comes at a space cost. This is because it is needed to constantly allocate a larger bucket array and then when this is done, free the old bucket array. Therefore both exist in memory for a short period of time, increasing memory usage. Then after freeing the old bucket array, some space is released, however the new bucket array is already in memory which occupies double the size of the previous one, so the program uses more memory each time rehashing is necessary.

Furthermore, the time complexity of this implementation is as follows:
Insert and search for documents in the table: O(1) average time complexity, and O(n) worst time complexity, where n is the number of keywords.
Rehashing to have a bigger bucket array: O(n), where n is the number of keywords.
Therefore this implementation requires more time because it is needed to rehash the table in some moments, however after having this rehashing process done then it takes less time to do all the insertion and searching operations.

In conclusion, making use of this potential implementation, makes it easier to search through the linked lists for a specific document. Additionally, since the table starts with a small size then this improves initialisation speed, and resizing on demand helps in efficiently handling growing amounts of data. However this rehashing process requires extra space in memory.





