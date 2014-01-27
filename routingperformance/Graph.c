#include "list.h"
#include "Graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#define MAX_LINE_LENGTH 12
void doNothing(void* bogus){
	return;
}

int increment(int i){
	return ++i;
}

int decrement(int i){
	return --i;
}

int findList_first(void* A,void* B){
	if (A == NULL) return 0;
	return (void*)((Edge)getKey((List)A))->oneEnd == B;
}

int findList_second(void* A,void* B){
	if (A == NULL) return 0;
	return (void*)((Edge)getKey((List)A))->anotherEnd == B;
}

int matchCharPtr(void* A, void* B){
   return (*(char*)A == *(char*)B);
}

int matchEdge(void* A, void* B){
	return ((Edge)A)->oneEnd == ((Edge)B)->oneEnd && ((Edge)A)->anotherEnd == ((Edge)B)->anotherEnd;
}

int matchEdge_reversed(void* A, void* B){
	return ((Edge)A)->oneEnd == ((Edge)B)->anotherEnd && ((Edge)A)->anotherEnd == ((Edge)B)->oneEnd;
}

int compareLoad(void* A, void* B){
    double A_l =  ((Edge)A)->numRunning;
    double B_l = ((Edge)B)->numRunning;
    A_l /=  ((Edge)A)->capacity;
    B_l /=  ((Edge)B)->capacity;
    return A_l < B_l;
}

int compareDelay(void* A, void* B){
   return ((Edge)A)->delay < ((Edge)B)->delay;
}

int compareHop(void* A, void* B){
   return 1;
}

Graph newGraph(FILE* input){
	char line[MAX_LINE_LENGTH];
	char node1 = 0;
	char node2 = 0;
	int delay = 0;
	int capacity = 0;
	Graph g = newEmptyGraph();
	Edge e = NULL;

	while(fgets(line, MAX_LINE_LENGTH, input) != NULL){
		sscanf(line,"%c %c %d %d",&node1,&node2,&delay,&capacity);
		e = (Edge)malloc(sizeof(struct edge));
		e->oneEnd = node1;
		e->anotherEnd = node2;
		e->capacity = capacity;
		e->delay = delay;
		e->numRunning = 0;
		insertEdge(g,e);
		free(e);
	}
	return g;
}

void printGraph(Graph g){
	List l = g->adjList;
	while (l != NULL){
		printList((List)l->key,printEdge);
		l = l->next;
	}
}

void printEdge(void* e){
	printf("%c,%c,%d,%d,%d//",((Edge)e)->oneEnd,((Edge)e)->anotherEnd
		,((Edge)e)->delay,((Edge)e)->capacity,((Edge)e)->numRunning);
}

Edge edgeToAdd(cost h,Graph large,Graph small){
	List temp = large->adjList;
	Edge minEdge = NULL;
	double minCost = INT_MAX;
	double tempVal = 0;
	while (temp != NULL){
		if (containsNode(small,((Edge)((List)temp->key)->key)->oneEnd)){temp = temp->next; continue;}
		List current = (List)temp->key;
		while (current != NULL){
			if (!containsNode(small,((Edge)current->key)->anotherEnd)){current = current->next; continue;}
			tempVal = h((Edge)current->key);
			if (tempVal < minCost){
				minEdge = (Edge)current->key;
				minCost = tempVal;
			}
			current = current -> next;
		}
		temp = temp->next;
	}
	return minEdge;
}

//dijkstra's algorithm, used for producing MSTs when the routing algorithm is LDP or LHP
Graph route(Graph g,cost h,char start){
	Graph MST = newEmptyGraph();
	List temp = g->adjList;
	while (((Edge)(((List)temp->key)->key))->oneEnd != start){
		temp = temp->next;
	}
	double min = INT_MAX;
	Edge minEdge = NULL;
	temp = (List)temp->key;
	double tempVal = 0;
	while (temp != NULL){
		tempVal = h((Edge)temp->key);
		if (tempVal < min){
			min = tempVal;
			minEdge = (Edge)temp->key;
		}
		temp = temp->next;
	}
	insertEdge(MST,minEdge);
	while (MST->numNodes != g->numNodes){
		Edge newEdge = edgeToAdd(h,g,MST);
		insertEdge(MST,newEdge);
	}
	return MST;
}





Graph newEmptyGraph(void){
	Graph g = (Graph)malloc(sizeof(struct graph));
	g->numEdges = 0;
	g->numNodes = 0;
	g->adjList = newList();
	return g;
}

void insertEdge(Graph g, Edge edge){
	char node1 = edge->oneEnd;
	char node2 = edge->anotherEnd;

	Edge another = (Edge)malloc(sizeof(struct edge));
	another->anotherEnd = edge->anotherEnd;
	another->oneEnd = edge->oneEnd;
	another->capacity = edge->capacity;
	another->delay = edge->delay;
	another->numRunning = edge->numRunning;

	List one = (List)findMatch(g->adjList,findList_first,(void*)node1);

	if (one == NULL){
		one = newList();
		add_list((void*)one,g->adjList);
		g->numNodes++;
	}
	add_list((void*)another,one);
	List two = (List)findMatch(g->adjList,findList_first,(void*)node2);
	if (two == NULL){
		two = newList();
		add_list((void*)two,g->adjList);
		g->numNodes++;
	}

	another = (Edge)malloc(sizeof(struct edge));
	another->anotherEnd = edge->oneEnd;
	another->oneEnd = edge->anotherEnd;
	another->capacity = edge->capacity;
	another->delay = edge->delay;
	another->numRunning = edge->numRunning;
	add_list((void*)another,two);
	g->numEdges += 1;
}

int containsNode(Graph g,char node){
	return (findMatch(g->adjList,findList_first,(void*)node) != NULL);
}

List DFS(Graph g,char dest){
	List stack = NULL;
	char start = ((Edge)((List)g->adjList->key)->key)->oneEnd;
	List neighbours = getNeighbours(g,start);
	List temp = neighbours;
	while (temp != NULL){
		stack = push(stack,temp->key);
		temp = temp->next;
	}
	Edge curr = NULL;
	List path = NULL;
	List tempLink = NULL;
   List linkToFree = NULL;
	while (1){

		curr = (Edge)stack->key;
      linkToFree= stack;
		stack = pop(stack);
      free(linkToFree);
		if (!path){
			path = push(path,curr);
		}else{
			while (path != NULL && ((Edge)path->key)->anotherEnd != curr->oneEnd){
				tempLink = path;
				path = path->next;
				free(tempLink);
			}
			path = push(path,curr);
		}
		if (curr->anotherEnd == dest) break;
		neighbours = getNeighbours(g,curr->anotherEnd);
		temp = neighbours->next;
		while (temp != NULL){
			stack = push(stack,temp->key);
			temp = temp->next;
		}
	}
    destroyList(stack,doNothing);
	return path;
}

List const getNeighbours(Graph g,char start){
	List neighbours = (List)findMatch(g->adjList,findList_first,(void*)start);
	return neighbours;
}


void changePathLoad(Graph g,List l,loadChanger lc){
	List arr[26] = {NULL};
	List start = NULL;
	Edge e = NULL;
	while (l != NULL){
		if (arr[((Edge)l->key)->oneEnd - 'A'] == NULL){
			arr[((Edge)l->key)->oneEnd - 'A'] = 
				(List)findMatch(g->adjList,findList_first,(void*)((Edge)l->key)->oneEnd);
		}
		if (arr[((Edge)l->key)->anotherEnd - 'A'] == NULL){
			arr[((Edge)l->key)->anotherEnd - 'A'] = 
				(List)findMatch(g->adjList,findList_first,(void*)((Edge)l->key)->anotherEnd);
		}
		
		start = arr[((Edge)l->key)->oneEnd - 'A'];
		e = (Edge)findMatch(start,matchEdge,(void*)((Edge)l->key));		
		e->numRunning = lc(e->numRunning);
		start = arr[((Edge)l->key)->anotherEnd - 'A'];
		e = (Edge)findMatch(start,matchEdge_reversed,(void*)((Edge)l->key));	
		e->numRunning = lc(e->numRunning);
		
		l = l->next;
	}
}

int pathValid(Graph g,List l){
	List arr[26] = {NULL};
	while (l != NULL){
		if (arr[((Edge)l->key)->oneEnd - 'A'] == NULL){
			arr[((Edge)l->key)->oneEnd - 'A'] = 
				(List)findMatch(g->adjList,findList_first,(void*)((Edge)l->key)->oneEnd);
		}
		List start = arr[((Edge)l->key)->oneEnd - 'A'];
		Edge e = (Edge)findMatch(start,matchEdge,(void*)((Edge)l->key));
		if (e->numRunning >= e->capacity) return 0;
		l = l->next;
	}
	return 1;
}

void printChar(void* c){
   printf("%c\n",*(char*)c);
}

List least_load_path(Graph g,char start,char dest,compare_func h){
   List seen = NULL;
   seen = push(seen,&start);
   List pQueue = NULL;
	List neighbours = getNeighbours(g,start);
	List temp = neighbours;
	while (temp != NULL){
		pQueue = insert(pQueue, temp->key, compareLoad);
		temp = temp->next;
	}
	Edge curr = NULL;

	List tempLink = NULL;
   char last = 0;
   List linkToFree = NULL;
   Edge ancestry[26] = {NULL};
	while (1){
		curr = (Edge)pQueue->key;
      seen = push(seen,&(curr->anotherEnd));
      linkToFree = pQueue;
		pQueue = pop(pQueue);
      free(linkToFree);
      last = curr->oneEnd;
		
     
		if (curr->anotherEnd == dest) break;
		neighbours = getNeighbours(g,curr->anotherEnd);
      temp = neighbours;
		while (temp != NULL){
         if (!findMatch(seen,matchCharPtr,&(((Edge)temp->key)->anotherEnd))) {
            pQueue = insert(pQueue, temp->key, h);
            //printf("setting ancestry %c to (%c-> %c)\n",((Edge)temp->key)->anotherEnd,curr->oneEnd,curr->anotherEnd);
            ancestry[((Edge)temp->key)->anotherEnd - 'A'] = curr;
         }
			temp = temp->next;
		}
	}
   assert(ancestry[start-'A'] == NULL);
   List path = NULL;
   path = push(path,curr);
   Edge parent = ancestry[curr->anotherEnd - 'A'];
   while (parent != NULL){
      path = push(path,parent);
      parent = ancestry[parent->anotherEnd - 'A'];
   }
   destroyList(pQueue,doNothing);
	return path;
}

int negative_load(Graph g){
   List temp = g->adjList;
   while (temp != NULL){
        List l = (List)temp->key;
        while (l != NULL){
            if (((Edge)l->key) -> numRunning < 0) return 1;
            l = l->next;
         }
        temp = temp->next;
   }
   return 0;
}

void destroyGraph(Graph g,void (*foo)(void*)){
   List temp = g->adjList;
   while (temp != NULL){
      destroyList((List)temp->key,foo);
      temp = temp->next;
   }
   destroyList(g->adjList,free);
   free(g);
}
