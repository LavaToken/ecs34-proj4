# Project X README

Student 1: Kevin Jia (921898728)

Student 2: Partner2 Name (123456987)

## Project Status
This the current status of the project work in progress

## Known Issues
The project fails to function properly in ...

## Code References
* [Different ways to initialize variables](https://stackoverflow.com/questions/22543875/different-ways-to-initialize-variables)
* [Dijkstra's Algorithm Explanation](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)

## Generative AI Use
I used ChatGPT for Prompt 1, and ZZZZZ for Prompt 2....

### Prompt 1
in an implementation of dijkstra's algorithm what are some common test cases.

### Response 1
When testing an implementation of Dijkstra's algorithm, you want cases that check both correctness and edge conditions. Below are common categories of test cases used in assignments, interviews, and production testing.

1. Simple Graph (Basic Correctness)

A small graph where the shortest path is obvious.

Example:

A --1--> B --2--> C
A --4--> C

Expected shortest path from A → C:
A → B → C with cost 3, not the direct edge cost 4.

Purpose:

Confirms the algorithm actually relaxes edges properly.

2. Single Node Graph
A

Expected result:

Distance from A to A = 0

Purpose:

Verifies initialization works correctly.

3. Disconnected Graph
A --1--> B

C --2--> D

Run from A.

Expected:

dist(A)=0

dist(B)=1

dist(C)=∞

dist(D)=∞

Purpose:

Ensures unreachable nodes stay infinity / undefined.

4. Graph With Cycles
A --1--> B
B --1--> C
C --1--> A
B --5--> D

Expected shortest path from A → D:
A → B → D = 6

Purpose:

Confirms algorithm doesn't loop infinitely.

5. Multiple Equal Shortest Paths
A --1--> B --1--> D
A --1--> C --1--> D

Expected:

Shortest distance to D = 2

Purpose:

Ensures algorithm handles ties correctly.

6. Graph Where Direct Path Is Worse
A --10--> B
A --1--> C
C --1--> B

Expected shortest path A → B:
A → C → B = 2

Purpose:

Tests correct edge relaxation ordering.

7. Zero Weight Edges
A --0--> B
B --2--> C
A --5--> C

Expected:

Shortest path A → B → C = 2

Purpose:

Ensures algorithm handles 0 weights correctly.

8. Large Sparse Graph

Example:

10,000 nodes

~10,000 edges

Purpose:

Tests performance with priority queue implementation.

9. Dense Graph

Example:

100 nodes

Almost every node connected to every other node.

Purpose:

Tests performance under many edges.

10. Negative Edge (Invalid Input)
A --(-1)--> B

Expected:

Implementation should reject or warn, because Dijkstra's algorithm does not work with negative weights.

Purpose:

Input validation.

11. Start Node With No Edges
A

B --1--> C

Run from A.

Expected:

Only A reachable.

Purpose:

Tests handling of isolated source node.

12. Self Loop
A --5--> A
A --1--> B

Expected:

Ignore self-loop for shortest path.

dist(B)=1

Purpose:

Ensures algorithm doesn't break on self-edges.

✅ Minimal test suite for most implementations

Basic graph

Disconnected graph

Graph with cycles

Multiple shortest paths

Zero-weight edges

Negative edge detection

💡 If you're writing unit tests for a coding assignment, I can also show a very common 6-graph test suite professors use that catches ~90% of Dijkstra implementation bugs.

### Prompt 2
My other prompt...

### Response 2
The other response...

### Changes 2
The other changes made...
