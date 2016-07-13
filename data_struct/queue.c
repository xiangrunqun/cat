#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 10

/** 
 * queue struct...and this is a circle queue....
 */
typedef struct {
	int data[MAX_SIZE];
	int rear;
	int front;
} Queue;

/**
 * declare function....
 */
Queue* createQueue();

int isFull(Queue *q);

int addQ(Queue *q, int ele);

int isEmpty(Queue *q);

int deleteQ(Queue *q);

int main(int argc, char const *argv[])
{
	Queue *q = createQueue();
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		addQ(q, i);
	}
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		deleteQ(q);
	}
	return 0;
}

Queue* createQueue(){
	Queue *q = (Queue*) malloc(sizeof(Queue));
	q->rear = 0;
	q->front = 0;
	return q;
}

int isFull(Queue *q){
	return (q->rear+1) % MAX_SIZE == q->front;
}

int addQ(Queue *q, int ele){
	// if queue is full....
	if(isFull(q)){
		printf("queue is full...\n");
	}else{
		q->rear = ( q->rear + 1 ) % MAX_SIZE;
		q->data[q->rear] = ele;
	}
	return ele;
}

int isEmpty(Queue *q){
	return q->rear == q->front;
}

int deleteQ(Queue *q){
	// if queue is empty....
	if(isEmpty(q)){
		printf("queue is empty now....\n"); return NULL;
	}else{
		int data = q->data[q->front];
		q->front = ( q->front + 1 ) % MAX_SIZE;
		return data;
	}
}

