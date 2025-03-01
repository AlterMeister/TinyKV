
#include "SkipList_atomic.h"
#include "method.h"

#ifndef _WIN32
#include <Windows.h>
#endif

void ClearScreen() {
    system("cls");
}

void print_menu() {
    std::cout << "====================================\n";
    std::cout << "         ���������н���\n";
    std::cout << "====================================\n";
    std::cout << "1. �����ֵ��\n";
    std::cout << "2. ���������ֵ��\n";
    std::cout << "3. ɾ����ֵ��\n";
    std::cout << "4. ����ɾ����ֵ��\n";
    std::cout << "5. ��ѯ��ֵ��\n";
    std::cout << "6. ��Χ��ѯ\n";
    std::cout << "7. �־û�������\n";
    std::cout << "8. �Ӵ��̼���\n";
    std::cout << "9. ��ӡ��������\n";
    std::cout << "10. �˳�\n";
    std::cout << "====================================\n";
    std::cout << "������ѡ��: ";
}


void insert_key_value(SkipListAtomic< std::string, int>& skipList) {
    int key;
    std::string value;
    std::cout << "�������: ";
    std::cin >> key;
    std::cout << "������ֵ: ";
    std::cin >> value;
    skipList.Insert(key, value);
    std::cout << "����ɹ���\n";
}

// ��������
void batch_insert(SkipListAtomic< std::string, int>& skipList) {
    int count;
    std::cout << "������Ҫ����ļ�ֵ������: ";
    std::cin >> count;
    for (int i = 0; i < count; ++i) {
        int key;
        std::string value;
        std::cout << "������� " << i + 1 << ": ";
        std::cin >> key;
        std::cout << "������ֵ " << i + 1 << ": ";
        std::cin >> value;
        skipList.Insert(key, value);
    }
    std::cout << "��������ɹ���\n";
}

void delete_key_value(SkipListAtomic<std::string, int>& skipList) {
    int key;
    std::cout << "�������: ";
    std::cin >> key;
    if (skipList.erase(key)) {
        std::cout << "ɾ���ɹ���\n";
    }
    else {
        std::cout << "�������ڣ�\n";
    }
}


void batch_delete(SkipListAtomic<std::string, int>& skipList) {
    int count;
    std::cout << "������Ҫɾ���ļ�ֵ������: ";
    std::cin >> count;
    for (int i = 0; i < count; ++i) {
        int key;
        std::cout << "������� " << i + 1 << ": ";
        std::cin >> key;
        if (skipList.erase(key)) {
            std::cout << "�� " << key << " ɾ���ɹ���\n";
        }
        else {
            std::cout << "�� " << key << " �����ڣ�\n";
        }
    }
}



// ��ѯ��ֵ��
void search_key_value(SkipListAtomic<std::string, int>& skipList) {
    int key;
    std::cout << "�������: ";
    std::cin >> key;
    auto result = skipList.search(key);
    if (result) {
        std::cout << "��ѯ���: " << *result << "\n";
    }
    else {
        std::cout << "�������ڣ�\n";
    }
}

// ��Χ��ѯ
void range_query(SkipListAtomic<std::string, int>& skipList) {
    int start, end;
    std::cout << "��������ʼ��: ";
    std::cin >> start;
    std::cout << "�����������: ";
    std::cin >> end;
    std::cout << "��Χ��ѯ���:\n";
    for (int i = start; i <= end; ++i) {
        auto result = skipList.search(i);
        if (result) {
            std::cout << "�� " << i << ": " << *result << "\n";
        }
    }
}


// �־û�������
void write_to_disk(SkipListAtomic<std::string, int>& skipList) {
    std::string filename;
    std::cout << "�������ļ�����Ĭ��: skiplist.dat��: ";
    std::cin.ignore(); // ���Ի��з�
    std::getline(std::cin, filename);
    if (filename.empty()) {
        filename = "skiplist.dat";
    }
    skipList.WriteToDisk(filename);
    std::cout << "�־û��ɹ���\n";
}

// �Ӵ��̼���
void load_from_disk(SkipListAtomic<std::string, int>& skipList) {
    std::string filename;
    std::cout << "�������ļ�����Ĭ��: skiplist.dat��: ";
    std::cin.ignore(); // ���Ի��з�
    std::getline(std::cin, filename);

    if (filename.empty()) {
        filename = "skiplist.dat";
    }

    try {
        skipList.LoadFromDisk(filename);
        std::cout << "���سɹ���\n";

        std::cout << "�����������£�\n";
        skipList.PrintSkipList();
    }
    catch (const std::exception& e) {
        std::cout << "����ʧ��: " << e.what() << "\n";
    }

}


void print_skip_list(SkipListAtomic<std::string, int>& skipList) {
    skipList.PrintSkipList();
}