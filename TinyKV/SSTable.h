#ifndef SSTABLE_H
#define SSTABLE_H

/*
	[Header]
	-Magic Number
	-Version
	-Data Offset
	-Number of Entries

	[Data Block]
	-Key-value Pairs  [按键排序]

	[Footer]
	-CheckSuu
*/

#include<iostream>
#include<vector>
#include<fstream>
#include<mutex>
#include<fstream>
#include"SkipList_atomic.h"


template<typename valueType, typename keyType>
class SSTable {
private:
	struct SSTableHeader {
		uint64_t magic_number; // 文件标识
		uint32_t version;      // 格式版本
		uint64_t data_offset;  // 数据块起始偏移
	};

	// 计算校验和 验证数据的完整性
	uint32_t checksum(const SkipListAtomic<valueType, keyType>& skiplist) {
		uint32_t check = 0;
		std::hash<keyType> key_hash;
		std::hash<valueType> value_hash;

		auto p = skiplist.getHead()->next[0].load(std::memory_order_acquire);
		while (p != nullptr) {
			check += static_cast<uint32_t>(key_hash(p->key)) + static_cast<uint32_t>(value_hash(p->value));
			p = p->next[0].load(std::memory_order_acquire);
		}

		return check;
	}

public:
	// 写入磁盘
	void WriteToDisk(const SkipListAtomic<valueType, keyType>& skiplist, const std::string& filename) {
		std::ofstream outfile(filename, std::ios::binary);

		// 1.写入Header
		SSTableHeader header;
		header.magic_number = 0x12345678;
		header.version = 1;
		header.data_offset = sizeof(header);
		outfile.write(reinterpret_cast<char*>(&header), sizeof(header));

		// 2.写入Data Block   只需要把底层给写入
		auto p = skiplist.getHead()->next[0].load(std::memory_order_acquire);
		while (p != nullptr) {
			outfile.write(reinterpret_cast<const char*>(&p->key), sizeof(p->key));
			outfile.write(reinterpret_cast<const char*>(&p->value), sizeof(p->value));
			p = p->next[0].load(std::memory_order_acquire);
		}

		outfile.close();

		// 3.写入Footer
		/*
		uint32_t checksumRes = checksum(skiplist);
		outfile.write(reinterpret_cast<char*>(&checksumRes), sizeof(checksumRes));
		*/
	}

	// 读出磁盘
	void LoadFromDisk(SkipListAtomic<valueType, keyType>& skiplist, std::string &filename) {
		std::ifstream infile(filename, std::ios::binary);

		// 1.读取header
		SSTableHeader header;
		infile.read(reinterpret_cast<char*>(&header), sizeof(header));

		if (header.magic_number != 0x12345678 || header.version != 1) {
			throw std::runtime_error("Invalid SSTable file format");
		}

		// 2.读取data block
		infile.seekg(header.data_offset);
		while (infile) {
			keyType key;
			valueType value;

			infile.read(reinterpret_cast<char*>(&key), sizeof(key));
			infile.read(reinterpret_cast<char*>(&value), sizeof(value));

			if (infile) {
				skiplist.Insert(key, value);
			}
		}

		infile.close();

		/*
		uint32_t check;
		infile.read(reinterpret_cast<char*>(&check), sizeof(check));

		if (check != checksum(skiplist)) {
			std::cout << check << std::endl;
			std::cout << checksum(skiplist) << std::endl;

			throw std::runtime_error("CheckSum failed!");
		}
		*/
	}

};


#endif
