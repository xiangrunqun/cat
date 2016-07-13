#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 10

typedef struct List{
	int data[MAX_SIZE];
	int last;
} list;

/*
 * init an empty list...
 */
List* makeEmpty();
/**
 * find x in the list, and return index.if not found then return -1
 */ 
int find(int x, List *ptrl);
/**
 * insert element before index...	
 */
void insert(int x, int i, List *ptrl);
/**
 * delete element by index...	
 */
void deleteElement(int i, List *ptrl);
/**
 * return length of list...	
 */
int length(List *ptrl);


// aow ~ ~ 
int main(int argc, char const *argv[])
{
	List *ptrl = makeEmpty();
	// insert ....
	printf("insert data....\n");
	insert(5, 1, ptrl);
	insert(3, 2, ptrl);
	insert(1, 3, ptrl);

	printf("%d\n", ptrl->data[0]);
	
	deleteElement(1, ptrl);

	//printf("data length is %lu\n", sizeof(ptrl->data)/sizeof(int));

	find(3, ptrl);

	printf("elment 0,1:%d,%d\n", ptrl->data[0],ptrl->data[1]);

	printf("length of list:%d\n", length(ptrl));

	return 0;
}

List* makeEmpty(){
	//List list = {};
	// it didn't work before having dinner, now it's okey!!!!! why ?????
	List *ptrl = (List*) malloc(sizeof(List));
	//List *ptrl = malloc(sizeof(List));
	//printf("sizeof data %lu\n", sizeof(ptrl->data)/sizeof(int));
	
	ptrl->last = -1;
	// printf("%lu\n", sizeof(ptrl->data));
	return ptrl;

	//return &list;
}

int find(int x , List *ptrl){
	int i=0;
	if(ptrl->last == -1){
		printf("empty list....\n");
		return -1;
	}
	while(ptrl->data[i] != x && ptrl->last>=i){
		printf("find data by index %d is %d\n", i, ptrl->data[i] );
		i++;
	}
	i = i>ptrl->last ? -1:i;
	printf("the index of %d in list is %d\n", x, i);
	return i;
}


void insert(int x, int i, List *ptrl){
	int full_index = MAX_SIZE - 1;
	// list is full...
	if( ptrl->last == full_index ){
		printf("list is full...."); return ;
	}
	// this is wonderful point, not allow empty element between last index and the insert
	// aow ~~~~~
	if(i < 1 || i > ptrl->last+2 ){
		printf("invalid index...."); return ;
	}
	
	for (int j=ptrl->last,k=i-1; k < j; --j)
	{
		ptrl->data[j+1] = ptrl->data[j];
	}
	ptrl->data[i-1] = x;
	ptrl->last++;
}

void deleteElement(int i, List *ptrl){
	// empty list..
	if(ptrl->last == -1){
		printf("list is empty\n" ); return ;
	}
	// verifying range ....
	if(i < 1 || i > length(ptrl) ){
		printf("invalid range.....\n"); return ;
	}
	printf("delete data from index %d....\n", i);
	for (int j = ptrl->last,k=i-1; k < j; k++)
	{
		printf("change %d to front....\n", ptrl->data[k]);
		ptrl->data[k] = ptrl->data[k+1];
	}

	ptrl->last--;
}

int length(List *ptrl){
	return ptrl->last+1;
}

