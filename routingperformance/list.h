#ifndef _LIST_H
#define _LIST_H
typedef struct _list* List;
struct _list{
	void* key;
	List next;

};
typedef int (counter(void*));
typedef int (comparator)(void*,void*);
List newList(void);
void add_list(void*,List);
List delete_list(void*,List,void (*foo)(void*));
void* get(int,List);
void* findMatch(List,int (*foo)(void*,void*),void* target);
void destroyList(List,void (*foo)(void*));
void* getKey(List l);
List pop(List l);
void printList(List l,void (*foo)(void*));
List push (List l,void* newItem);
List insert(List l,void* newContent,comparator c);
int aggregate(List l, counter h);
#endif
