
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
    std::cout << "         跳表命令行界面\n";
    std::cout << "====================================\n";
    std::cout << "1. 插入键值对\n";
    std::cout << "2. 批量插入键值对\n";
    std::cout << "3. 删除键值对\n";
    std::cout << "4. 批量删除键值对\n";
    std::cout << "5. 查询键值对\n";
    std::cout << "6. 范围查询\n";
    std::cout << "7. 持久化到磁盘\n";
    std::cout << "8. 从磁盘加载\n";
    std::cout << "9. 打印跳表内容\n";
    std::cout << "10. 退出\n";
    std::cout << "====================================\n";
    std::cout << "请输入选项: ";
}


void insert_key_value(SkipListAtomic< std::string, int>& skipList) {
    int key;
    std::string value;
    std::cout << "请输入键: ";
    std::cin >> key;
    std::cout << "请输入值: ";
    std::cin >> value;
    skipList.Insert(key, value);
    std::cout << "插入成功！\n";
}

// 成批插入
void batch_insert(SkipListAtomic< std::string, int>& skipList) {
    int count;
    std::cout << "请输入要插入的键值对数量: ";
    std::cin >> count;
    for (int i = 0; i < count; ++i) {
        int key;
        std::string value;
        std::cout << "请输入键 " << i + 1 << ": ";
        std::cin >> key;
        std::cout << "请输入值 " << i + 1 << ": ";
        std::cin >> value;
        skipList.Insert(key, value);
    }
    std::cout << "批量插入成功！\n";
}

void delete_key_value(SkipListAtomic<std::string, int>& skipList) {
    int key;
    std::cout << "请输入键: ";
    std::cin >> key;
    if (skipList.erase(key)) {
        std::cout << "删除成功！\n";
    }
    else {
        std::cout << "键不存在！\n";
    }
}


void batch_delete(SkipListAtomic<std::string, int>& skipList) {
    int count;
    std::cout << "请输入要删除的键值对数量: ";
    std::cin >> count;
    for (int i = 0; i < count; ++i) {
        int key;
        std::cout << "请输入键 " << i + 1 << ": ";
        std::cin >> key;
        if (skipList.erase(key)) {
            std::cout << "键 " << key << " 删除成功！\n";
        }
        else {
            std::cout << "键 " << key << " 不存在！\n";
        }
    }
}



// 查询键值对
void search_key_value(SkipListAtomic<std::string, int>& skipList) {
    int key;
    std::cout << "请输入键: ";
    std::cin >> key;
    auto result = skipList.search(key);
    if (result) {
        std::cout << "查询结果: " << *result << "\n";
    }
    else {
        std::cout << "键不存在！\n";
    }
}

// 范围查询
void range_query(SkipListAtomic<std::string, int>& skipList) {
    int start, end;
    std::cout << "请输入起始键: ";
    std::cin >> start;
    std::cout << "请输入结束键: ";
    std::cin >> end;
    std::cout << "范围查询结果:\n";
    for (int i = start; i <= end; ++i) {
        auto result = skipList.search(i);
        if (result) {
            std::cout << "键 " << i << ": " << *result << "\n";
        }
    }
}


// 持久化到磁盘
void write_to_disk(SkipListAtomic<std::string, int>& skipList) {
    std::string filename;
    std::cout << "请输入文件名（默认: skiplist.dat）: ";
    std::cin.ignore(); // 忽略换行符
    std::getline(std::cin, filename);
    if (filename.empty()) {
        filename = "skiplist.dat";
    }
    skipList.WriteToDisk(filename);
    std::cout << "持久化成功！\n";
}

// 从磁盘加载
void load_from_disk(SkipListAtomic<std::string, int>& skipList) {
    std::string filename;
    std::cout << "请输入文件名（默认: skiplist.dat）: ";
    std::cin.ignore(); // 忽略换行符
    std::getline(std::cin, filename);

    if (filename.empty()) {
        filename = "skiplist.dat";
    }

    try {
        skipList.LoadFromDisk(filename);
        std::cout << "加载成功！\n";

        std::cout << "跳表内容如下：\n";
        skipList.PrintSkipList();
    }
    catch (const std::exception& e) {
        std::cout << "加载失败: " << e.what() << "\n";
    }

}


void print_skip_list(SkipListAtomic<std::string, int>& skipList) {
    skipList.PrintSkipList();
}