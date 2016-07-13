#include <stdio.h>
#include <stdlib.h>

/**
 * 二分查找
 */
int binarySearch(int arr[], int k, int len);

int main(int argc, char const *argv[])
{
	int arr[10];
	for (int i = 0; i < 10; ++i)
	{
		arr[i] = i+1;
	}
	//int len = sizeof(arr)/sizeof(int);
	//printf(" size of arr is %d\n", len);
	printf("%d\n", binarySearch(arr, 5, sizeof(arr)/sizeof(int)) );
	return 0;
}

int binarySearch(int arr[], int k, int len){
	int left = 1, right = len, mid = (left + right ) / 2;
	printf("the mid is %d\n", mid);
	for (; left <= right; mid  = (( left + right ) / 2))
	{
		printf("the mid in for is %d\n", mid);
		if(arr[mid] > k ) right = mid - 1;
		else if ( arr[mid] < k ) left = mid + 1;
		else return mid;
	}
	return -1;
}

