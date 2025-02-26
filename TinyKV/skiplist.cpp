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

template <typename valueType, typename keyType>
class SkipList {
private:
	// ����ÿ���ڵ�����ݽṹ
	struct Node {
		keyType key;
		valueType value;

		std::vector<Node*> next;

		Node(const keyType& k, const valueType& v, int level) : key(k), value(v), next(level, nullptr) {}
	};

	int maxLevel;       // ������
	int minLevel;
	int currentLevel;   // ��ǰ����
	Node* head;         // ͷ�ڵ�

	// �����������߼�
	int randomLevel() {
		int level = 1;
		while (std::rand() % 2 && level < maxLevel) {
			++level;
		}
		return level;
	}

public:
	// ����Ĺ��캯������������
	SkipList() : maxLevel(32), minLevel(1) {
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
	valueType* search(keyType key) {
		Node* p = head;
		
		for (int i = currentLevel - 1; i >= 0; --i) {
			while (p->next[i] != nullptr && p->next[i]->key < key) {
				p = p->next[i];
			}
			if (p->next[i] && p->next[i]->key == key) {
				return &(p->next[i]->value);
			}
		}

		return nullptr;
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
		std::vector<Node*> pathList(maxLevel + 1);
		Node* p = head;

		// ���ϵ���ȥ������������С��num������
		for (int i = currentLevel - 1; i >= 0; i--) {
			while (p->next[i] && p->next[i]->key < key) {
				p = p->next[i];
			}
			// ��¼ÿһ���ǰ���ڵ�
			pathList[i] = p;
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
				pathList[i] = head;  // һ���������������head��λ��
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

	// ����ڵ�  ɾ��������ָ�����Ľڵ�
	bool erase(keyType key) {
		std::vector<Node*> pathList(maxLevel + 1);
		Node* p = head;

		// 1.���Ҵ�ɾ���ڵ�
		for (int i = currentLevel - 1; i >= 0; i--) {
			while (p->next[i] && p->next[i]->key < key) {
				p = p->next[i];
			}
			pathList[i] = p;
		}

		// 2.���ڵ��Ƿ����
		p = p->next[0];
		if (!p || p->key != key) {
			return false;
		}

		// 3.����ָ��
		for (int i = 0; i < currentLevel; i++) {
			if (pathList[i]->next[i] != p) {
				break;  // ��ǰ��û�д�ɾ���ڵ�
			}
			pathList[i]->next[i] = p->next[i];
		}

		// 4.�����������
		while (currentLevel > 1 && head->next[currentLevel - 1] == nullptr) {
			--currentLevel; // ��������Ĳ���  ���Ϊ���һ��Ļ��ǾͲ�����
		}

		delete p;
		return true;
	}


	// ����Ĳ���
	void TestSkipList() {
		std::cout << "����Ԫ��: (1, 'one'), (3, 'three'), (7, 'seven'), (4, 'four'), (9, 'nine')\n";
		Insert(1, "one");
		Insert(3, "three");
		Insert(7, "seven");
		Insert(4, "four");
		Insert(9, "nine");

		PrintSkipList();

		std::cout << "\n��ѯԪ�� 7: " << *search(7) << std::endl;
		std::cout << "��ѯԪ�� 5: " << (search(5) ? *search(5) : "Not Found") << std::endl;

		std::cout << "\nɾ��Ԫ�� 4\n";
		erase(4);
		PrintSkipList();

		std::cout << "\nɾ��Ԫ�� 1\n";
		erase(1);
		PrintSkipList();

		std::cout << "\n����Ԫ��: (5, 'five')\n";
		Insert(5, "five");
		PrintSkipList();
	}

};



int main() {
	SkipList<std::string, int> skiplist;
	skiplist.TestSkipList();
	return 0;
}