#include <atomic>
#include <mutex>
#include <iostream>
#include <vector>
#include <thread>
#include "SkipList_atomic.h"



int main() {
    SkipListAtomic<std::string, int> skiplist;
    skiplist.TestSkipList();
    skiplist.test_concurrent_access();
    return 0;
}