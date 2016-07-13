#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 我真是个傻逼，下面这行代码相当于定义一个类型的别名
 * 如 typedef int *duang;
   看懂了吗，笨蛋
 */
typedef struct TreeNode *BinTree;
struct TreeNode{
	char *data;
	BinTree left;
	BinTree right;
} ;

/**
 * access order 
 */
void preOrderTraversal(BinTree tree);

BinTree createChildTree(BinTree tree, char *left_data, char *right_data);

BinTree initTree();

char alphbet[] = "ABCDFGIEH";

int main(int argc, char const *argv[])
{
	// create binary tree...
	BinTree tree = initTree();
	preOrderTraversal(tree);
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
		BinTree right = (BinTree) malloc(sizeof(BinTree));
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
