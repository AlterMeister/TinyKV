#include "SkipList_atomic.h"


int main() {
    SkipListAtomic<std::string, int> skipList;

    // 插入一些数据
    skipList.Insert(1, "one");
    skipList.Insert(2, "two");
    skipList.Insert(3, "three");
    skipList.Insert(4, "four");
    skipList.Insert(5, "funf");
    skipList.Insert(6, "sech");
    skipList.Insert(7, "sieben");
    skipList.Insert(8, "archt");
    skipList.Insert(9, "neun");
    skipList.Insert(12, "zwolf");
    skipList.Insert(45, "funfundvierbig");
    skipList.Insert(32, "zweiunddreibig");
    skipList.Insert(21, "einsundzweibig");
    skipList.Insert(42, "fourty-two");
    skipList.Insert(63, "sixty-three");

    // 持久化到磁盘
    std::string filename = "skiplist.dat";
    skipList.WriteToDisk(filename);

    skipList.PrintSkipList();

    // 清空跳表
    SkipListAtomic<std::string, int> newSkipList;

    // 从磁盘加载数据
    newSkipList.LoadFromDisk(filename);

    // 验证加载的数据
    auto result1 = newSkipList.search(1);
    auto result2 = newSkipList.search(2);
    auto result3 = newSkipList.search(3);
    auto result4 = newSkipList.search(4);
    auto result5 = newSkipList.search(5);
    auto result6 = newSkipList.search(6);

    std::cout << "Key 1: " << (result1 ? *result1 : "Not Found") << std::endl;
    std::cout << "Key 2: " << (result2 ? *result2 : "Not Found") << std::endl;
    std::cout << "Key 3: " << (result3 ? *result3 : "Not Found") << std::endl;
    std::cout << "Key 4: " << (result4 ? *result4 : "Not Found") << std::endl;
    std::cout << "Key 5: " << (result5 ? *result5 : "Not Found") << std::endl;
    std::cout << "Key 6: " << (result6 ? *result6 : "Not Found") << std::endl;

    
    newSkipList.PrintSkipList();

    return 0;
}
