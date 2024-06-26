# If you find a bug, or have suggestions, please open an [issue](https://github.com/zWolv/CPP-Things/issues).

# C++ Implementations of datastructures
I find datastructures interesting, and wanted to learn about the ins and outs of them. So I decided to just implement them myself to learn about them, learning some C++ in the process. I do try to make the implementations as generic as possible.

## General info
None of the datastructures will work with raw pointers. They will accept `std::unqiue_ptr` or `std::shared_ptr`. I do intend to make all my datastructures compatible compatible with objects or structs that manage their own memory dynamically using pointers. All arithmetic types (following `std::is_arithmetic<T>`) will be contiguous in memory if possible with the datastructure. Other types will not be contiguous in memory.


## Datastructures I have written *some* tests for.
* List (Dynamic Array)
* Linked List
* Stack
* Doubly Linked List
  
## Datastructures I am 99% sure actually work fully
None.

## Datastructures Implemented (may contain bugs)
* Queue

## Datastructures In Progress
* HashMap

## Datastructures Planned
I will implement these eventually, in no particular order.
* Priority Queue - Implementation with a Max Heap
* Tree
* Binary Tree's
  * Binary Search Tree
    * Red-Black BST
    * Regular BST
  * Heap
    * Min Heap
    * Max Heap
* HashSet

