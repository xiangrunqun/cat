#include "tree_node_struct_for_traversal.c"

#define MAX_SIZE 10

// struct of single stack...
typedef struct {
    BinTree data[MAX_SIZE];
    int top;
} Stack;

Stack* createStack(int size);

int isFull(Stack *s, int maxSize);

void push(Stack *s, BinTree item);

int isEmpty(Stack *s);

BinTree pop(Stack *s);

BinTree getTopEle(Stack *s);

Stack* createStack(int size){
    Stack *s = (Stack*) malloc(sizeof(Stack));
    s->top = -1;
    return s;
}

int isFull(Stack *s, int maxSize){
    if(s->top == maxSize -1){
        return 1;
    }else{
        return 0;
    }
}

void push(Stack *s, BinTree item){
    if (s->top == MAX_SIZE -1) {
        printf("stack is full....\n");
    }else{
        s->data[++(s->top)] = item;
    }
}

int isEmpty(Stack *s){
    if (s->top == -1) {
        return 1;
    }else{
        return 0;
    }
}

BinTree pop(Stack *s){
    if (s->top == -1) {
        printf("stack is empty...\n");
        return NULL;
    }else{
        return s->data[s->top--];
    }
}

BinTree getTopEle(Stack *s){
    if (s->top == -1) {
        printf("stack is empty...\n");
        return NULL;
    }else{
        return s->data[s->top];
    }
}
