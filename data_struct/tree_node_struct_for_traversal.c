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
	// used to recognize tree node has been access....
	int has_accessed;
} ;
