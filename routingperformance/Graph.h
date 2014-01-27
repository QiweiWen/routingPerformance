#ifndef graph_h
#define graph_h
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

void doNothing(void* bogus);

int increment(int i);

int decrement(int i);


struct edge{
	char oneEnd;
	char anotherEnd;
	int delay;
	int capacity;
	int numRunning;
};

typedef struct edge* Edge;


int matchEdge(void* A, void* B);

int matchEdge_reversed(void* A, void* B);


int compareLoad(void* A, void* B);
int compareDelay(void* A, void* B);
int compareHop(void* A, void* B);

struct graph{
	List adjList;
	int numNodes;
	int numEdges;
};
typedef struct graph* Graph;
typedef struct pair* Pair;
typedef double (cost(Edge));
typedef int (loadChanger(int));
typedef int (compare_func)(void* A, void* B);

Graph newGraph(FILE* input);
Graph route(Graph g,cost h,char start);
List least_load_path(Graph g,char start,char dest,compare_func h);
void destroyGraph(Graph g,void (*foo)(void*));
void printGraph(Graph g);
void printEdge(void*);
Graph newEmptyGraph(void);
void insertEdge(Graph g, Edge edge);
int containsNode(Graph g,char node);
Edge edgeToAdd(cost h,Graph large,Graph small);
void changePathLoad(Graph g,List l,loadChanger lc);
int pathValid(Graph g,List l);
List DFS(Graph g,char dest);
List const getNeighbours(Graph g,char start);
int negative_load(Graph g);
#endif
