/*
	�������������ʵ��

	���������������

	����ײ���һ����������


	��һ�㣺1->7

	�ڶ��㣺1->4->7->9

	�����㣺 1->3->4->5->7->8->9->10

	����ˮƽ���С���ֱ����
*/

#include<iostream>
#include<algorithm>
#include<vector>

template <typename T, typename valueType, typename keyType>

class SkipList {
private:
	// ����ÿ���ڵ�����ݽṹ
	typedef struct node {
		keyType key;
		valueType value;

		std::vector<Node*> next;

		Node(const keyType& k, const valueType& v, int level) : key(k), value(v), next(level, nullptr) {}
	}Node*;

	int maxLevel;       // ������
	int currentLevel;   // ��ǰ����
	Node* head;         // ͷ�ڵ�

	std::vector<Node*> pathList;  // ����ʱ����ʱ����

	// �����������߼�
	int randomLevel() {
		int level = 1;
		while (std::rand() % 2 && level < maxLevel) {
			++level;
		}
		return level;
	}


	void TestSkipList() {

	}


public:
	// ����Ĺ��캯������������
	SkipList() : maxLevel(16), minLevel(1) {
		std::srand(std::time(0));  // ��ʼ���������

		head = new Node(keyType(), valueType(), maxLevel);
	}

	~SkipList()
	{
		// ע�⣬����һ�����ͷŵ�
		Node* current = head;

		while (current) {
			Node* next = current->next[0];
			delete current;

			current = next;
		}
	}

	// ��ӡ����
	void PrintSkipList() const {
		for (int i = currentLevel - 1; i >= 0; i--) {
			std::cout << "��" << i + 1 << ":";
			Node* current = head->next[i];

			while (current) {
				std::cout << current->key << "(" << current->value << ")";
				if (current->next[i]) {
					std::cout << "->";
				}
				current = current->next[i];
			}

			std::cout << std::endl;
		}
	}

	// ��ѯ����
	valueType* search(Skiplist* sl, keyType key) {
		Node* p = sl->head;
		
		while (p) {
			// ����Ѱ��Ŀ������  ��Ȼ�����ǿ�����������ȷ�����Ĵ���������
			while (p->next[i] && p->next[i]->key < key) {
				p = p->next[i];
			}
			// û�ҵ�Ŀ�꣬������
			if (!p->next[i] || p->next[i]->key > key) {
				p = p->down;
			}
			else {
				return &(p->value);
			}
		}

		return NULL;
	}

	// ����Ԫ��  �����ڵײ����������в����½ڵ㣬����Ҫ����㷨�������½ڵ�Ĳ���
	/*
		1.���Ҳ���λ�ã�������²����½ڵ�Ĳ���λ��
		2.�������������¥��Խ�ߣ�����Խ��
		3.���²���������������������ԭ��������Ҫ����
		4.�����½ڵ㣺����

		�����һ����Ҫ�ص��ǲ���Ĳ�������ȫ�����

		PathList�����ã���¼ÿһ�� ��С�� Ŀ��key�Ľڵ㣡
	*/

	void Insert(const keyType& key, const valueType& value) {
		// 1.Ѱ�Ҳ���λ��
		pathList.clear();
		Node* p = head;

		// ���ϵ���ȥ������������С��num������
		for (int i = currentLevel - 1; i >= 0; i--) {
			while (p->next[i] && p->next[i]->key < key) {
				p = p->next[i];
			}
			// ��¼ÿһ���ǰ���ڵ�
			pathList.push_back(p);
		}

		// 2.�����ֵ�Ѵ��ڣ������ֵ
		if (p->next[0] && p->next[0]->key == key) {
			p->next[0]->value = value;
			return;
		}

		// 3.��������½ڵ�Ĳ���
		int newLevel = randomLevel();

		// 4.��������Ĳ���
		if (newLevel > currentLevel) {
			// ����½ڵ�������ڵ�ǰ����Ĳ���
			for (int i = currentLevel; i < newLevel; i++) {
				pathList.push_back(head);
			}
			currentLevel = newLevel;
		}

		// �����ڵ�
		Node* newNode = new Node(key, value, newLevel);

		// ����
		for (int i = 0; i < newLevel; i++) {
			newNode->next[i] = pathList[i]->next[i];
			pathList[i]->next[i] = newNode;
		}
	}

	// ����ڵ�
	void erase() {

	}

};

