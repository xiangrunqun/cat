#include <stdio.h>
#include <stdlib.h>


typedef struct LinkList {
	int data;
	LinkList *next;
} List;

/*
 * init an empty list...
 */
List* makeEmpty();
/**
 * find x in the list, and return index.if not found then return -1
 */ 
List* findByElement(int element, List *ptrl);
/**
 * find element by index...
 */ 
List* findElementByIndex(int x, List *ptrl);
/**
 * insert element before index...	
 */
List* insert(int ele, int i, List *ptrl);
/**
 * delete element by index...	i between 1 and +++
 */
List* deleteElement(int i, List *ptrl);
/**
 * return length of list...	
 */
int length(List *ptrl);


int main(int argc, char const *argv[])
{
	List *ptrl;
	//int len = length(ptrl);
	//printf("len is %d\n", len);
	ptrl = makeEmpty();
	printf("the init ptrl addr is:%p\n", ptrl);

	int index = 3;
	ptrl = insert( 3, 1, ptrl );
	printf("the first data is:%d\n", ptrl->data);


	//insert( 3, 1, ptrl );
	//printf("the first p addr is:%p\n", ptrl);
	//printf("list length after insert element is %d\n", length(ptrl));
	//printf("test data %d \n", ptrl->data);
	List *p = findElementByIndex( index, ptrl );
	if(!p){
		printf("element in index %d is NULL\n", index);
	}
	ptrl = insert(2,2,ptrl);
	printf("the second data is:%d\n", ptrl->data);


	//printf("the second p addr is:%p\n", ptrl);
	//printf("current point to data %d\n", ptrl->data);
	//printf("list length is %d after insert value 2 in index 2 \n", length(ptrl));
	ptrl = insert(1,3,ptrl);
	printf("the third data is:%d\n", ptrl->data);
	//printf("the third p addr is:%p\n", ptrl);
	p = findElementByIndex( index, ptrl);
	printf(" findElementByIndex index is %d and data is %d \n", index, p->data);
	

	p = ptrl;
	//printf("%p\n", p->next);
	while(p != NULL){
		printf("the while data is:%d\n", p->data);
		p = p->next;
	}

	

	p = findByElement(3, ptrl);
	
	if(p != NULL)
		printf("find by element is %d\n", p->data);
	
	printf("length of list before delete is %d\n", length(ptrl));

	// delete element by index...
	deleteElement( 1, ptrl );
	printf("length of list after delete element is %d\n", length(ptrl));

	/*
	*/


	return 0;
}

List* makeEmpty(){
	//printf("make empty sizeof list:%lu\n", sizeof(List));
	List *ptrl = (List*) malloc(sizeof(List));
	// you will be getting segmentation fault if not setting next variate to null.
	ptrl->next = NULL;
	ptrl->data = 0;
	//printf("init data is %d\n",ptrl->data);
	return ptrl;
}

// get list length...
int length(List *ptrl){
	printf("length addr is %p\n", ptrl);
	int i=0;
	List *p = ptrl;
	
	while (p != NULL) {
		printf("p is not NULL, and data is %d\n", p->data);
		p = p->next;
		i++;
	}
	return i;
}

List* findElementByIndex(int index, List *ptrl){
	List *p = ptrl;
	int i = 1;
	while ( i < index && p ){
		p = p->next;
		i++;
	}
	//printf("find by i is %d and index is %d \n", i,index);
	if( i == index ) return p;
	else return NULL;
}

List* findByElement(int element, List *ptrl){
	List *p = ptrl;
	while(p->next != NULL && p->data != element){
		p = p->next;
	}
	return p;
}

List* insert(int ele, int i, List *ptrl){
	printf("insert ptrl addr is :%p\n", ptrl);
	List *p, *s;
	if(i==1){
		p = (List *) malloc(sizeof(List));
		p->data = ele;
		p->next = ptrl;
		return p;

	}else{
		// find the previous element by index....
		p = findElementByIndex(i-1, ptrl);
		if(p){
			s = (List *) malloc(sizeof(List));
			s->data = ele;
			s->next = p->next;
			p->next = s;
		}else{
			printf("insert index error....\n");
		}
	}
	return ptrl;
}

List* deleteElement(int i, List *ptrl){
	List *p, *target;
	if(i == 1){
		p = ptrl->next;
		*ptrl = *ptrl->next;
		free(p);
		return ptrl;
	}
	p = findElementByIndex(i-1, ptrl);
	if(p && p->next){
		target = p->next;
		p->next = target->next;		
		free(target);
	}else{
		printf("previous element or current is not found...\n");
	}
	return ptrl;
}


