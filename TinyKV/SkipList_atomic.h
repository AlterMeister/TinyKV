#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <atomic>
#include <mutex>
#include <iostream>
#include <vector>
#include <thread>

template <typename valueType, typename keyType>
class SkipListAtomic {
private:
    // 跳表每个节点的数据结构
    struct Node {
        keyType key;
        valueType value;

        // 原子指针
        std::vector<std::atomic<Node*>> next;
        std::vector<std::mutex> locks;  // 每层一个锁

        Node(const keyType& k, const valueType& v, int level) : key(k), value(v), next(level), locks(level) {
            for (int i = 0; i < level; i++) {
                next[i].store(nullptr, std::memory_order_relaxed); // 修复：添加分号
            }
        }
    };

    int maxLevel;       // 最大层数
    int minLevel;
    int currentLevel;   // 当前层数
    Node* head;         // 头节点

    // 层数的生成逻辑
    int randomLevel() {
        int level = 1;
        while (std::rand() % 2 && level < maxLevel) {
            ++level;
        }
        return level;
    }

public:
    // 跳表的构造函数与析构函数
    SkipListAtomic() : maxLevel(32), minLevel(1), currentLevel(1) {
        std::srand(std::time(0));  // 初始化随机种子
        head = new Node(keyType(), valueType(), maxLevel);
    }

    ~SkipListAtomic() {
        // 注意，这是一层层地释放的
        Node* current = head;
        while (current) {
            Node* next = current->next[0].load(std::memory_order_relaxed);
            delete current;
            current = next;
        }
    }

    void PrintSkipList() const;
    valueType* search(keyType key);
    void Insert(const keyType& key, const valueType& value);
    bool erase(keyType key);
    void TestSkipList();
    void test_concurrent_access();
};

template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::PrintSkipList() const {
    for (int i = currentLevel - 1; i >= 0; i--) {
        std::cout << "层" << i + 1 << ":";
        Node* current = head->next[i].load(std::memory_order_acquire); // 使用load方法

        while (current) {
            std::cout << current->key << "(" << current->value << ")";
            if (current->next[i].load(std::memory_order_acquire)) {
                std::cout << "->";
            }
            current = current->next[i].load(std::memory_order_acquire);
        }

        std::cout << std::endl;
    }
}

// search操作无需加锁保护
template <typename valueType, typename keyType>
valueType* SkipListAtomic<valueType, keyType>::search(keyType key) {
    Node* p = head;

    for (int i = currentLevel - 1; i >= 0; --i) {
        while (p->next[i].load(std::memory_order_acquire) != nullptr && p->next[i].load(std::memory_order_acquire)->key < key) {
            p = p->next[i].load(std::memory_order_acquire);
        }
        if (p->next[i].load(std::memory_order_acquire) && p->next[i].load(std::memory_order_acquire)->key == key) {
            return &(p->next[i].load(std::memory_order_acquire)->value);
        }
    }

    return nullptr;
}

// 插入操作必须要加锁保护
template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::Insert(const keyType& key, const valueType& value) {
    // 1.寻找插入位置
    std::vector<Node*> pathList(maxLevel + 1);
    Node* p = head;

    // 1.查找插入位置
    for (int i = currentLevel - 1; i >= 0; i--) {
        while (p->next[i].load(std::memory_order_acquire) != nullptr && p->next[i].load(std::memory_order_acquire)->key < key) {
            p = p->next[i].load(std::memory_order_acquire);
        }
        pathList[i] = p;
    }

    // 2. 如果键值已存在，则更新值
    if (p->next[0].load(std::memory_order_acquire) && p->next[0].load(std::memory_order_acquire)->key == key) { 
        p->next[0].load(std::memory_order_acquire)->value = value;
        return;
    }

    // 3. 随机生成新节点的层数
    int newLevel = randomLevel();

    // 4. 更新跳表层数
    if (newLevel > currentLevel) {
        for (int i = currentLevel; i < newLevel; ++i) {
            pathList[i] = head;
        }
        currentLevel = newLevel;
    }

    // 5. 创建新节点
    Node* newNode = new Node(key, value, newLevel);

    // 6. 插入新节点（加锁保护）
    for (int i = 0; i < newLevel; ++i) {
        std::lock_guard<std::mutex> lock(pathList[i]->locks[i]); // 加锁
        newNode->next[i].store(pathList[i]->next[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
        pathList[i]->next[i].store(newNode, std::memory_order_release);
    }
}

template <typename valueType, typename keyType>
bool SkipListAtomic<valueType, keyType>::erase(keyType key) {
    std::vector<Node*> pathList(maxLevel + 1);
    Node* p = head;

    // 1. 查找待删除节点
    for (int i = currentLevel - 1; i >= 0; --i) {
        while (p->next[i].load(std::memory_order_acquire) != nullptr &&
            p->next[i].load(std::memory_order_acquire)->key < key) {
            p = p->next[i].load(std::memory_order_acquire);
        }
        pathList[i] = p;
    }

    // 2. 检查节点是否存在
    p = p->next[0].load(std::memory_order_acquire);
    if (!p || p->key != key) {
        return false;
    }

    // 3. 更新指针（加锁保护）
    for (int i = 0; i < currentLevel; ++i) {
        std::lock_guard<std::mutex> lock(pathList[i]->locks[i]); // 加锁
        if (pathList[i]->next[i].load(std::memory_order_relaxed) != p) {
            break; // 当前层没有待删除节点
        }
        pathList[i]->next[i].store(p->next[i].load(std::memory_order_relaxed), std::memory_order_release);
    }

    // 4. 更新跳表层数
    while (currentLevel > 1 && head->next[currentLevel - 1].load(std::memory_order_acquire) == nullptr) {
        --currentLevel;
    }

    // 5. 延迟释放内存（确保没有线程正在访问该节点）
    delete p;
    return true;
}

template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::TestSkipList() {
    std::cout << "插入元素: (1, 'one'), (3, 'three'), (7, 'seven'), (4, 'four'), (9, 'nine')\n";
    Insert(1, "one");
    Insert(3, "three");
    Insert(7, "seven");
    Insert(4, "four");
    Insert(9, "nine");
    Insert(11, "eleven");
    Insert(32, "thirty-two");
    Insert(76, "seventy-six");
    Insert(99, "ninety-nine");
    Insert(100, "one hundrud");

    PrintSkipList();

    std::cout << "\n查询元素 7: " << *search(7) << std::endl;
    std::cout << "查询元素 5: " << (search(5) ? *search(5) : "Not Found") << std::endl;

    std::cout << "\n删除元素 4\n";
    erase(4);
    PrintSkipList();

    std::cout << "\n删除元素 1\n";
    erase(1);
    PrintSkipList();

    std::cout << "\n插入元素: (5, 'five')\n";
    Insert(5, "five");
    PrintSkipList();
}

template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::test_concurrent_access() {
    // 插入线程
    auto insert_func = [this]() {
        for (int i = 0; i < 1000; ++i) {
            this->Insert(i, "value" + std::to_string(i));
        }
        };

    // 删除线程
    auto erase_func = [this]() {
        for (int i = 0; i < 1000; ++i) {
            this->erase(i);
        }
        };

    // 查询线程
    auto search_func = [this]() {
        for (int i = 0; i < 1000; ++i) {
            auto result = this->search(i);
            if (result) {
                std::cout << "Found: " << *result << std::endl;
            }
        }
        };

    // 启动多个线程
    std::vector<std::thread> threads;
    threads.emplace_back(insert_func);
    threads.emplace_back(erase_func);
    threads.emplace_back(search_func);

    for (auto& t : threads) {
        t.join();
    }
}

#endif
