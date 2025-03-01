#pragma once
#ifndef METHOD_H
#define METHOD_H

#include <iostream>
#include <string>
#include <vector>

void ClearScreen();
void print_menu();
void insert_key_value(SkipListAtomic< std::string, int>& skipList);
void batch_insert(SkipListAtomic< std::string, int>& skipList);
void delete_key_value(SkipListAtomic<std::string, int>& skipList);
void batch_delete(SkipListAtomic<std::string, int>& skipList);
void search_key_value(SkipListAtomic<std::string, int>& skipList);
void range_query(SkipListAtomic<std::string, int>& skipList);
void write_to_disk(SkipListAtomic<std::string, int>& skipList);
void load_from_disk(SkipListAtomic<std::string, int>& skipList);
void print_skip_list(SkipListAtomic<std::string, int>& skipList);

#endif
