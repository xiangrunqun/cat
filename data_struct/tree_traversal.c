#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// include my head file....ao~~~~
#include "link_stack_char.h"

char alphbet[] = "ABCDFGIEH";

/**
 * traversal tree previously...
 */
void preOrderTraversal(BinTree tree);
/**
 * traversal tree in middle order....
 */
void inOrderTraversal(BinTree tree);
/**
 * traversal tree in post order....
 */
void postOrderTraversal(BinTree tree);

BinTree createChildTree(BinTree tree, char *left_data, char *right_data);

BinTree initTree();
/**
 * you know waht i mean....
 */
void preOrderTraversalInCircle(BinTree tree);
void inOrderTraversalInCircle(BinTree tree);
void postOrderTraversalInCircle(BinTree tree);

int main(int argc, char const *argv[])
{
	/**
	 * create binary tree...
	 */
	BinTree tree = initTree();

	/**
	 * access binary tree in recursion...
	 */
	// previous ....
	// preOrderTraversal(tree);
	// middle ....
	// inOrderTraversal(tree);
	// post ....
	// postOrderTraversal(tree);


	/**
	 * duang~~~~~~ in circle .....
	 */
	// traversal tree in another way....
	// preOrderTraversalInCircle(tree);
	// inOrderTraversalInCircle(tree);
	postOrderTraversalInCircle(tree);


	free(tree);
	return 0;
}

void preOrderTraversal(BinTree tree){
	// typical ordinary code .....
	/**
	printf("data of tree node is --- %s\n", tree->data);
	if(tree->left != NULL){
		preOrderTraversal(tree->left);
	}else if(tree->right != NULL){
		preOrderTraversal(tree->right);
	}
	*/
	// printf("%c\n", tree->data);
	// more smarter way...
	if(tree){
		printf("node data of tree is %c\n", *(tree->data));
		preOrderTraversal(tree->left);
		preOrderTraversal(tree->right);
	}
}
void inOrderTraversal(BinTree tree){
	if(tree){
		inOrderTraversal(tree->left);
		printf("node data of tree is %c\n", *(tree->data));
		inOrderTraversal(tree->right);
	}
}
void postOrderTraversal(BinTree tree){
	if(tree){
		postOrderTraversal(tree->left);
		postOrderTraversal(tree->right);
		printf("node data of tree is %c\n", *(tree->data));
	}
}

BinTree initTree(){
	// create tree hierarchy....
	// root tree node....
	BinTree tree = (BinTree) malloc(sizeof(struct TreeNode));
	tree->data = &alphbet[0]; 
	tree->left = NULL; tree->right = NULL;
	// second level...
	createChildTree(tree, &alphbet[1], &alphbet[2]);

	// third level...
	createChildTree(tree->left, &alphbet[3], &alphbet[4]);
	createChildTree(tree->right, &alphbet[5], &alphbet[6]);

	// fourth level,not complete
	createChildTree(tree->left->right, &alphbet[7], NULL);

	createChildTree(tree->right->left, NULL, &alphbet[8]);
	return tree;
}

BinTree createChildTree(BinTree tree, char *left_data, char *right_data){
	if(left_data){
		BinTree left = (BinTree) malloc(sizeof(TreeNode));
		left->data = left_data; 
		left->left = NULL; left->right = NULL;
		tree->left = left;
	}
	if(right_data){
		BinTree right = (BinTree) malloc(sizeof(TreeNode));
		// BinTree is not a struct type,and it can not be init as a struct...
		// BinTree right = {right_data,NULL,NULL};
		right->data = right_data; 
		right->left = NULL; right->right = NULL;
		// Are you stupid....the variable right will be destroy when createChildTree finish....
		// so you can not init TreeNode in this way...
		// struct TreeNode right = {right_data,NULL,NULL};
		tree->right = right;
	}
	return tree;
}

void preOrderTraversalInCircle(BinTree tree){
	Stack *stack = createStack(MAX_SIZE);
	BinTree bt = tree;
	while(bt || !isEmpty(stack)){
		while(bt){
			printf("%c\n", *(bt->data));
			push(stack, bt);
			bt = bt->left;
		}
		if(!isEmpty(stack)){
			bt = pop(stack);
			bt = bt->right;
		}
	}
}
void inOrderTraversalInCircle(BinTree tree){
	Stack *stack = createStack(MAX_SIZE);
	BinTree bt = tree;
	while(bt || !isEmpty(stack)){
		while(bt){
			push(stack, bt);
			bt = bt->left;
		}
		if(!isEmpty(stack)){
			bt = pop(stack);
			printf("%c\n", *(bt->data));
			bt = bt->right;
		}
	}
}
void postOrderTraversalInCircle(BinTree tree){
	Stack *stack = createStack(MAX_SIZE);
	BinTree bt = tree;
	while(bt || !isEmpty(stack)){
		int flag_over = 0;
		while(bt && !bt->has_accessed){
			push(stack, bt);
			bt = bt->left;
			flag_over = 1;
		}
		if(!isEmpty(stack)){
			bt = getTopEle(stack);
			bt = bt->right;
			if(bt && !bt->has_accessed) continue ;
			
			bt = pop(stack);
			printf("poped data is %c, accessed flag is %d\n", *(bt->data), bt->has_accessed);
			bt->has_accessed = 1;
			bt = bt->right;
			flag_over = 1;
		}
		if(!flag_over) break;
	}
}
