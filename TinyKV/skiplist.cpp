/*
	�������������ʵ��

	���������������

	����ײ���һ����������


	��һ�㣺1->7

	�ڶ��㣺1->4->7->9

	�����㣺 1->3->4->5->7->8->9->10

	����ˮƽ���С���ֱ����
*/

#include "skiplist.h"



int main() {
	SkipList<std::string, int> skiplist;
	skiplist.TestSkipList();
	return 0;
}