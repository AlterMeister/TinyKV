#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <atomic>
#include <mutex>
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <cstdlib>

template <typename valueType, typename keyType>
class SkipListAtomic {
private:
    // ����ÿ���ڵ�����ݽṹ

    // ԭ��ָ��
        /*

            ʲô��ԭ�ӻ�������
            ʵ�����ڸ�ֵ��ʱ�򣬸�ֵһ�ο����Ƕ�β������м���ܻ��г�ͻ�ķ��ա�
            ����ԭ�ӻ�������������������ա�

            ԭ�ӻ�������
            load:   ��ȡԭ�ӱ�����ֵ�����ᱻ�����̴߳��
            store:  ��ԭ�ӱ�����ֵ�����ᱻ�����ֳ����

            �����ڴ�˳��
            memory_order_acquire:ȷ����ȡ�����������µģ����Һ������������ڶ�ȡ�Ľ��������д������
            memory_order_relax:��Ҫ��������졣�����ʼ����ʱ��û�������̸߳��š�
            memory_order_relase:������Ҫȷ��д������ݶ������߳̿ɼ�������д��֮ǰ�Ĳ������ᱻ���������ڶ�������

        */

    struct Node {
        keyType key;
        valueType value;

        std::vector<std::atomic<Node*>> next;
        std::vector<std::mutex> locks;  // ÿ��һ����

        Node(const keyType& k, const valueType& v, int level) : key(k), value(v), next(level), locks(level) {
            for (int i = 0; i < level; i++) {
                next[i].store(nullptr, std::memory_order_relaxed);
            }
        }
    };

    struct SSTableHeader {
        uint64_t magic_number; // �ļ���ʶ
        uint32_t version;      // ��ʽ�汾
        uint64_t data_offset;  // ���ݿ���ʼƫ��
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
    SkipListAtomic() : maxLevel(32), minLevel(1), currentLevel(1) {
        std::srand(std::time(0));  // ��ʼ���������
        head = new Node(keyType(), valueType(), maxLevel);
    }

    ~SkipListAtomic() {

    }

    void WriteToDisk(std::string& filename);

    // ��������
    void LoadFromDisk(std::string& filename);

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
        std::cout << "��" << i + 1 << ":";
        Node* current = head->next[i].load(std::memory_order_acquire); // ʹ��load

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

// search���������������
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

// �����������Ҫ��������
template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::Insert(const keyType& key, const valueType& value) {
    // 1.Ѱ�Ҳ���λ��
    std::vector<Node*> pathList(maxLevel + 1);
    Node* p = head;

    // 1.���Ҳ���λ��
    for (int i = currentLevel - 1; i >= 0; i--) {
        while (p->next[i].load(std::memory_order_acquire) != nullptr && p->next[i].load(std::memory_order_acquire)->key < key) {
            p = p->next[i].load(std::memory_order_acquire);
        }
        pathList[i] = p;
    }

    // 2. �����ֵ�Ѵ��ڣ������ֵ
    if (p->next[0].load(std::memory_order_acquire) && p->next[0].load(std::memory_order_acquire)->key == key) { 
        p->next[0].load(std::memory_order_acquire)->value = value;
        return;
    }

    // 3. ��������½ڵ�Ĳ���
    int newLevel = randomLevel();

    // 4. �����������
    if (newLevel > currentLevel) {
        for (int i = currentLevel; i < newLevel; ++i) {
            pathList[i] = head;
        }
        currentLevel = newLevel;
    }

    // 5. �����½ڵ�
    Node* newNode = new Node(key, value, newLevel);

    // 6. �����½ڵ㣨����������
    for (int i = 0; i < newLevel; ++i) {
        std::lock_guard<std::mutex> lock(pathList[i]->locks[i]); // ����
        newNode->next[i].store(pathList[i]->next[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
        pathList[i]->next[i].store(newNode, std::memory_order_release);
    }
}

template <typename valueType, typename keyType>
bool SkipListAtomic<valueType, keyType>::erase(keyType key) {
    std::vector<Node*> pathList(maxLevel + 1);
    Node* p = head;

    // 1. ���Ҵ�ɾ���ڵ�
    for (int i = currentLevel - 1; i >= 0; --i) {
        while (p->next[i].load(std::memory_order_acquire) != nullptr && p->next[i].load(std::memory_order_acquire)->key < key) {
            p = p->next[i].load(std::memory_order_acquire);
        }
        pathList[i] = p;
    }

    // 2. ���ڵ��Ƿ����
    p = p->next[0].load(std::memory_order_acquire);
    if (!p || p->key != key) {
        return false;
    }

    // 3. ����ָ�루����������
    for (int i = 0; i < currentLevel; ++i) {
        std::lock_guard<std::mutex> lock(pathList[i]->locks[i]); // ����
        if (pathList[i]->next[i].load(std::memory_order_relaxed) != p) {
            break; // ��ǰ��û�д�ɾ���ڵ�
        }
        pathList[i]->next[i].store(p->next[i].load(std::memory_order_relaxed), std::memory_order_release);
    }

    // 4. �����������
    while (currentLevel > 1 && head->next[currentLevel - 1].load(std::memory_order_acquire) == nullptr) {
        --currentLevel;
    }

    // 5. �ӳ��ͷ��ڴ棨ȷ��û���߳����ڷ��ʸýڵ㣩
    delete p;
    return true;
}

template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::WriteToDisk(std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary);

    // 1.д��Header
    SSTableHeader header;
    header.magic_number = 0x12345678;
    header.version = 1;
    header.data_offset = sizeof(header);

    outfile.write(reinterpret_cast<char*>(&header), sizeof(header));

    // 2.д��Data Block   ֻ��Ҫ�ѵײ��д��
    auto p = head->next[0].load(std::memory_order_acquire);
    while (p != nullptr) {
        outfile.write(reinterpret_cast<const char*>(&p->key), sizeof(p->key));
        outfile.write(reinterpret_cast<const char*>(&p->value), sizeof(p->value));
        p = p->next[0].load(std::memory_order_acquire);
    }

    outfile.close();
}


template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::LoadFromDisk(std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);

    // 1.��ȡheader
    SSTableHeader header;
    infile.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.magic_number != 0x12345678 || header.version != 1) {
        throw std::runtime_error("Invalid SSTable file format");
    }

    // 2.��ȡdata block
    infile.seekg(header.data_offset);
    while (infile) {
        keyType key;
        valueType value;

        infile.read(reinterpret_cast<char*>(&key), sizeof(key));
        infile.read(reinterpret_cast<char*>(&value), sizeof(value));

        if (infile) {
            Insert(key, value);
        }
    }

    infile.close();
}


template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::TestSkipList() {
    std::cout << "����Ԫ��: (1, 'one'), (3, 'three'), (7, 'seven'), (4, 'four'), (9, 'nine')\n";
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

template <typename valueType, typename keyType>
void SkipListAtomic<valueType, keyType>::test_concurrent_access() {
    // �����߳�
    auto insert_func = [this]() {
        for (int i = 0; i < 1000; ++i) {
            this->Insert(i, "value" + std::to_string(i));
        }
        };

    // ɾ���߳�
    auto erase_func = [this]() {
        for (int i = 0; i < 1000; ++i) {
            this->erase(i);
        }
    };

    // ��ѯ�߳�
    auto search_func = [this]() {
        for (int i = 0; i < 1000; ++i) {
            auto result = this->search(i);
            if (result) {
                std::cout << "Found: " << *result << std::endl;
            }
            else {
                std::cout << "Not Found: " << i << std::endl;
            }
        }
    };

    // ��������߳�
    std::vector<std::thread> threads;
    threads.emplace_back(insert_func);
    threads.emplace_back(erase_func);
    threads.emplace_back(search_func);

    for (auto& t : threads) {
        t.join();
    }
}

#endif
