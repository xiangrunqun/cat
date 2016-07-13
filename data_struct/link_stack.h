
#define MAX_SIZE 10

// struct of single stack...
typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;

Stack* createStack(int size);

int isFull(Stack *s, int maxSize);

void push(Stack *s, int item);

int isEmpty(Stack *s);

int pop(Stack *s);


/** double stack... */
typedef struct {
    int data[MAX_SIZE];
    int top1;// the top pointer of stack 1...
    int top2;// the top pointer of stack 2...
} DStack;

void d_push(DStack *ds, int item, int tag);

int d_pop(DStack *ds, int tag);


/** link stack... */
typedef struct Node {
    int data;
    struct Node *next;
} LinkStack;

LinkStack* createLinkStack();

int isEmptyLinkStack(LinkStack *s);

void link_push(LinkStack *ls, int node);

int link_pop(LinkStack *ls);

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

void push(Stack *s, int item){
    if (s->top == MAX_SIZE -1) {
        printf("stack is full....\n");
    }else{
        s->data[s->top++] = item;
    }
}

int isEmpty(Stack *s){
    if (s->top == -1) {
        return 1;
    }else{
        return 0;
    }
}

int pop(Stack *s){
    if (s->top == -1) {
        printf("stack is empty...\n");
        return 0;
    }else{
        return s->data[s->top--];
    }
}

void d_push(DStack *ds, int item, int tag){
    //printf("top2 is %d, top1 is %d \n", ds->top2, ds->top1);
    if (ds->top2 - ds->top1 == 1) {
        //printf("double stack is full...\n");
        //printf("top2 is %d, top1 is %d \n", ds->top2, ds->top1);
        return ;
    }
    if (tag == 1) {
        ds->data[++(ds->top1)] = item;
    }else{
        ds->data[--(ds->top2)] = item;
    }
}

int d_pop(DStack *ds, int tag){
    if (tag == 1) {
        if (ds->top1 == -1) {
            printf("stack is empty now....\n"); return -1;
        }else{
           return ds->data[ds->top1--];
        }
    }else{
        if (ds->top2 == MAX_SIZE) {
            printf("stack 2 is empty....\n"); return -1;
        }else{
            return ds->data[ds->top2++];
        }
    }
}

LinkStack* createLinkStack(){
    LinkStack *ls = (LinkStack*) malloc(sizeof(LinkStack));
    ls->next = NULL;
    return ls;
}

int isEmptyLinkStack(LinkStack *ls){
    return ls->next == NULL;
}


void link_push(LinkStack *ls, int node){
    LinkStack *p = (LinkStack*) malloc(sizeof(LinkStack));
    p->data = node;
    p->next = ls->next;
    ls->next = p;
}


int link_pop(LinkStack *ls){
    if (isEmptyLinkStack(ls)) {
        printf("link stack is empty...\n");
        return -1;
    }
    LinkStack *p = ls->next;
    ls->next = p->next;
    int data = p->data;
    free(p);
    return data;
}
