/*
	应用化跳表
*/

#include "SkipList_atomic.h"
#include "method.h"

int main() {
    SkipListAtomic<std::string, int> skipList;
    int choice;

    while (true) {
        ClearScreen();
        print_menu();
        std::cin >> choice;

        switch (choice) {
        case 1:
            insert_key_value(skipList);
            break;
        case 2:
            batch_insert(skipList);
            break;
        case 3:
            delete_key_value(skipList);
            break;
        case 4:
            batch_delete(skipList);
            break;
        case 5:
            search_key_value(skipList);
            break;
        case 6:
            range_query(skipList);
            break;
        case 7:
            write_to_disk(skipList);
            break;
        case 8:
            load_from_disk(skipList);
            break;
        case 9:
            print_skip_list(skipList);
            break;
        case 10:
            std::cout << "退出程序。\n";
            return 0;
        default:
            std::cout << "无效选项，请重新输入！\n";
            break;
        }

        // 暂停，等待用户按回车继续
        std::cout << "\n按回车继续...";
        std::cin.ignore(); // 忽略换行符
        std::cin.get();    // 等待用户输入
    }

    return 0;
}