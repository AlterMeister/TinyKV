/*
	本程序是跳表的实现

	跳表是特殊的链表

	跳表底层是一个有序链表


	第一层：1->7

	第二层：1->4->7->9

	第三层： 1->3->4->5->7->8->9->10

	跳表水平排列、垂直排列
*/

#include "skiplist.h"



int main() {
	SkipList<std::string, int> skiplist;
	skiplist.TestSkipList();
	return 0;
}