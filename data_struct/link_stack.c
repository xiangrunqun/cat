
#include <stdio.h>
#include <stdlib.h>
#include "link_stack.h"

int main(int argc, const char * argv[]) {
    /**
    Stack *s = createStack(MAX_SIZE);
    printf("stack is full %d\n", isFull(s, MAX_SIZE));
    for (int i=0; i<MAX_SIZE; i++) {
        push(s, i);
    }
    push(s, 11);
    printf("stack is empty %d\n", isEmpty(s));
    pop(s);
    pop(s);
    pop(s);
    printf("the size of stack is %d\n", s->top+1);
    */
    
    // initialize double stack
    /**
    DStack *ds = (DStack*) malloc(sizeof(DStack));
    ds->top1 = -1;
    ds->top2 = MAX_SIZE;
    for (int i=0; i<MAX_SIZE; i++) {
        d_push(ds, i, 1);
        d_push(ds, i, 2);
    }
    for (int i=0; i<MAX_SIZE; i++) {
        d_pop(ds, 1);
        d_pop(ds, 2);
    }
     */
    
    // initalize link stack ...
    
    LinkStack *ls = createLinkStack();
    for (int i=0; i<5; i++) {
        link_push(ls, i);
    }
    for (int i=0; i<6; i++) {
        link_pop(ls);
    }
    return 0;
}
