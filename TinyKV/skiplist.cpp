/*
	本程序是跳表的实现

	跳表是特殊的链表

	跳表底层是一个有序链表


	第一层：1->7

	第二层：1->4->7->9

	第三层： 1->3->4->5->7->8->9->10

	跳表水平排列、垂直排列
*/

#include<iostream>
#include<algorithm>
#include<vector>

template <typename valueType, typename keyType>
class SkipList {
private:
	// 跳表每个节点的数据结构
	struct Node {
		keyType key;
		valueType value;

		std::vector<Node*> next;

		Node(const keyType& k, const valueType& v, int level) : key(k), value(v), next(level, nullptr) {}
	};

	int maxLevel;       // 最大层数
	int currentLevel;   // 当前层数
	Node* head;         // 头节点

	std::vector<Node*> pathList(maxLevel, 0);  // 查找时的临时变量

	// 层数的生成逻辑
	int randomLevel() {
		int level = 1;
		while (std::rand() % 2 && level < maxLevel) {
			++level;
		}
		return level;
	}

	// 跳表的测试
	void TestSkipList() {

	}


public:
	// 跳表的构造函数与析构函数
	SkipList() : maxLevel(16), minLevel(1) {
		std::srand(std::time(0));  // 初始化随机种子

		head = new Node(keyType(), valueType(), maxLevel);
	}

	~SkipList()
	{
		// 注意，这是一层层地释放的
		Node* current = head;

		while (current) {
			Node* next = current->next[0];
			delete current;

			current = next;
		}
	}

	// 打印跳表
	void PrintSkipList() const {
		for (int i = currentLevel - 1; i >= 0; i--) {
			std::cout << "层" << i + 1 << ":";
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

	// 查询跳表
	valueType* search(keyType key) {
		Node* p = head;
		
		while (p) {
			// 左右寻找目标区间  显然，我们可以上来就能确定他的大区间在哪
			while (p->next[i] && p->next[i]->key < key) {
				p = p->next[i];
			}
			// 没找到目标，向下走
			if (!p->next[i] || p->next[i]->key > key) {
				p = p->down;
			}
			else {
				return &(p->value);
			}
		}

		return NULL;
	}

	// 插入元素  不仅在底层有序链表中插入新节点，还需要随机算法来决定新节点的层数
	/*
		1.查找插入位置：逐层向下查找新节点的插入位置
		2.生成随机层数：楼层越高，概率越低
		3.更新层数：如果新跳表层数大于原层数，需要更新
		4.插入新节点：插入

		跳表的一个重要特点是插入的层数是完全随机的

		PathList的作用：记录每一次 次小于 目标key的节点！
	*/

	void Insert(const keyType& key, const valueType& value) {
		// 1.寻找插入位置
		pathList.clear();
		Node* p = head;

		// 从上到下去搜索，搜索次小于num的数字
		for (int i = currentLevel - 1; i >= 0; i--) {
			while (p->next[i] && p->next[i]->key < key) {
				p = p->next[i];
			}
			// 记录每一层的前驱节点
			pathList[i] = p;
		}

		// 2.如果键值已存在，则更新值
		if (p->next[0] && p->next[0]->key == key) {
			p->next[0]->value = value;
			return;
		}

		// 3.随机生成新节点的层数
		int newLevel = randomLevel();

		// 4.更新跳表的层数
		if (newLevel > currentLevel) {
			// 如果新节点层数大于当前跳表的层数
			for (int i = currentLevel; i < newLevel; i++) {
				pathList[i] = head;  // 一个特殊情况，处理head的位置
			}
			currentLevel = newLevel;
		}

		// 创建节点
		Node* newNode = new Node(key, value, newLevel);

		// 插入
		for (int i = 0; i < newLevel; i++) {
			newNode->next[i] = pathList[i]->next[i];
			pathList[i]->next[i] = newNode;
		}

		delete p;
	}

	// 清除节点  删除跳表中指定键的节点
	bool erase(keyType key) {
		pathList.clear();
		Node* p = head;

		// 1.查找待删除节点
		for (int i = currentLevel - 1; i >= 0; i--) {
			while (p->next[i] && p->next[i]->key < key) {
				p = p->next[i];
			}
			pathList[i] = p;
		}

		// 2.检查节点是否存在
		p = p->next[0];
		if (!p || p->key != key) {
			return false;
		}

		// 3.更新指针
		for (int i = 0; i < currentLevel; i++) {
			if (pathList[i]->next[i] != p) {
				break;  // 当前层没有待删除节点
			}
			pathList[i]->next[i] = p->next[i];
		}

		// 4.更新跳表层数
		while (currentLevel > 1 && head->next[currentLevel - 1] == nullptr) {
			--currentLevel; // 降低跳表的层数  如果为最后一层的话那就不变了
		}

		delete p;
		return true;
	}

};



int main() {

}