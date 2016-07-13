// #include <stdio.h>
// #include <stdlib.h>

// #define MAX_SIZE 10

/** 
 * queue struct...and this is a circle queue....
 */
typedef struct {
	BinTree data[MAX_SIZE];
	int rear;
	int front;
} Queue;

/**
 * declare function....
 */
Queue* createQueue();

int isFull(Queue *q);

BinTree addQ(Queue *q, BinTree ele);

int isEmpty(Queue *q);

BinTree deleteQ(Queue *q);

// int main(int argc, char const *argv[])
// {
// 	int test_data[10]={1,2,3,4,5,6,7,8,9,10};
// 	Queue *q = createQueue();
// 	for (int i = 0; i < MAX_SIZE; ++i)
// 	{
// 		addQ(q, &test_data[i]);
// 	}
// 	for (int i = 0; i < MAX_SIZE; ++i)
// 	{
// 		deleteQ(q);
// 	}
// 	return 0;
// }

Queue* createQueue(){
	Queue *q = (Queue*) malloc(sizeof(Queue));
	q->rear = 0;
	q->front = 0;
	return q;
}

int isFull(Queue *q){
	return (q->rear+1) % MAX_SIZE == q->front;
}

BinTree addQ(Queue *q, BinTree ele){
	// if queue is full....
	if(isFull(q)){
		printf("queue is full...\n");
	}else{
		q->data[q->rear] = ele;
		q->rear = ( q->rear + 1 ) % MAX_SIZE;
	}
	// printf("after add queue...%p\n", ele);
	// if(ele) printf("add ele is NULL...and data is %c\n", *(ele->data));
	return ele;
}

int isEmpty(Queue *q){
	if(q->rear == q->front) return 1;
	else return 0;
}

BinTree deleteQ(Queue *q){
	// if queue is empty....
	if(isEmpty(q)){
		printf("queue is empty now....\n"); return NULL;
	}else{
		BinTree data = q->data[q->front];
		q->front = ( q->front + 1 ) % MAX_SIZE;
		// if(!data->data) printf("data in delete queue is NULL...\n");
		// printf("in del function...%p\n", data);
		return data;
	}
}

