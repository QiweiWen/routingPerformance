// ConsoleApplication7.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>
#include "Graph.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdint.h>



struct evnt{
	double time;
	List edges;
};

typedef struct evnt* Event;

int int_comp(void* a, void* b){
    if (*((int*)a) < *((int*)b)){
        return 1;
    }else{
        return 0;
    }
}

void printInt(void* i){
    printf("%d",*(int*)i);
}

static List timetable;

int count_delay(void* e){
   return ((Edge)e)->delay;
}

int count_hop(void* e){
   return 1;
}

//breadth first search
double SHP(Edge e){
	return 1;
}
//least-loaded
double LLP(Edge e){
	return e->numRunning/e->capacity;
}
//least delay
double SDP(Edge e){
	return (double)e->delay;
}


int matchListWithTime(void* A, void* B){
    List A_list = (List)A;
    return ((struct evnt*)A_list->key)->time == *(double*)B;
}

int compareListsOfEvents(void* A,void* B){
    return ((Event)(((List)A)->key))->time < ((Event)(((List)B)->key))->time;
}

void printEvent(void* evnt){
    Event e = (Event) evnt;
    printList(e->edges,printEdge);
    printf("%lf\n",e->time);
}

void printListOfEvents(void* events){
    List eventList = (List)events;
    printList(eventList,printEvent);
}

char* getLineFromFile(char* dest,FILE* source){
	char c = fgetc(source);
	int size = 0;
	while ((c >= 'A' && c <= 'Z') || (c >= '!' && c <= '9') || (c == ' ')){
		if (dest == NULL){
			dest = (char*)malloc(sizeof(char));
			*dest = 0;
		}
		++size;
		dest = (char*)realloc(dest,size+1);
		dest[size - 1] = c;
		dest[size] = 0;
		c = fgetc(source);
	}
	return dest;
}

int main(int argc, char* argv[])
{
    timetable = NULL;
	if (argc != 4){
		fprintf(stderr,"Look, I came here for an argument!\n");
		return 1;
	}
	char* algo = argv[1];
	char* topology = argv[2];
	char* workload = argv[3];
	FILE* topology_file = fopen(topology,"r");
	assert(topology_file != NULL);
	Graph g = newGraph(topology_file);
	fclose(topology_file);
	FILE* workload_file = fopen(workload,"r");
   assert(workload_file != NULL);
	char* newLine = getLineFromFile(NULL,workload_file);
    double start_time = 0;
    char start_node = 0;
    char end_node = 0;
    double duration = 0;
    
    compare_func* h;
    if (!strcmp(algo, "SDP")){
        h = compareDelay;
    }else if (!strcmp(algo, "SHP")){
        h = compareHop;
    } else{
        h = compareLoad;
    }
    int rejected = 0;
    int total = 0;
    List path = NULL;
   double currTime = 0;

    double percent = 0;
   double avg_delay = 0;
   double avg_hop = 0;
	while (newLine != NULL){
   
        total ++;
        sscanf(newLine,"%lf %c %c %lf",&start_time,&start_node,&end_node,&duration);
        currTime = start_time;
         while (timetable != NULL && currTime >= ((Event)((List)timetable->key)->key)->time){
                changePathLoad(g, ((Event)((List)timetable->key)->key)->edges, decrement);
                List tempLink = timetable;
                timetable = pop(timetable);
                destroyList((List)tempLink->key,free);
                free(tempLink);
        }
        path = least_load_path(g, start_node, end_node,h);
       
   
        if (pathValid(g, path)){
            changePathLoad(g, path, increment);
             avg_delay += aggregate(path,count_delay);
        avg_hop += aggregate(path,count_hop);
             double timeToFree = start_time + duration;
              List events = (List)findMatch(timetable, matchListWithTime, &timeToFree);
              Event newTask = (Event)malloc(sizeof(struct evnt));
              newTask->edges = path;
              newTask->time = timeToFree;
              if (!events){
                  events = newList();
                  add_list((void*)newTask, events);
                  timetable = insert(timetable,events,compareListsOfEvents);
              }else{
                  add_list((void*)newTask, events);
              }
        }else{
            rejected++;
        }
        newLine = getLineFromFile(NULL,workload_file);
	}
    percent = total - rejected;
    percent /= total;
    percent *= 100;
    //  printf("%lf\n",percent);
    avg_delay /= (total - rejected);
    avg_hop /= (total - rejected);
    printf("total number of virtual circuit requests: %d\nnumber of successfully routed requests: %d\n\
percentage of successfully routed request: %.2f\nnumber of blocked requests: %d\n\
percentage of blocked requests: %.2f\naverage number of hops per circuit: %.2f\n\
average cumulative propagation delay per circuit: %.2f\n",total,(total - rejected),percent,rejected, 100 - percent, avg_hop, avg_delay);
    destroyGraph(g,free);
}

