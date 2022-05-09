#include <iostream>

#include "gtest/gtest.h"
#include "skiplist.h"

namespace skiplist {
TEST(SkipListTest, BasicTest) { EXPECT_EQ(3, 1 + 2); }

std::string usageMessage() {
  std::string message =
      "Enter any of the following commands after the prompt > \n"
      "\t a <k> -- Lookup key <k> (int64_t) and return its associated value.\n"
      "\t i <k, v> -- Insert <k>(int64_t) <v>(int64_t) to skiplit \n"
      "\t d <k> -- Delete key <k> (int64_t) and its associated value. \n"
      "\t f <filename>  -- insert multiple kv from reading file.\n"
      "\t p -- Print the SkipList.\n"
      "\t q -- Quit. (Or use Ctl-D.)\n"
      "Please Enter max height of SkipList \n"
      "Example: 2\n > ";
  return message;
}

TEST(SkipListTest, PrintTest) {
  int max_height;
  int64_t key = 0;
  int64_t value = 0;
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  std::vector<GenericValue<8>> result;
  GenericComparator<8> comparator;
  std::cout << usageMessage();
  std::cin >> max_height;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  std::string file_name;

  char instruction;
  bool quit = false;
  while (!quit) {
    std::cout << "> ";
    std::cin >> instruction;
    switch (instruction) {
      case 'a':
        std::cin >> key;
        index_key.SetFromInteger(key);
        result.clear();
        skiplist.Lookup(index_key, &result);
        for (auto &item : result) {
          std::cout << item << ", ";
        }
        std::cout << std::endl;
        break;
      case 'd':
        std::cin >> key;
        index_key.SetFromInteger(key);
        skiplist.Remove(index_key);
        break;
      case 'i':
        std::cin >> key >> value;
        index_key.SetFromInteger(key);
        index_value.SetFromInteger(key);
        skiplist.Insert(index_key, index_value);
        break;
      case 'f':
        std::cin >> file_name;
        skiplist.InsertFromFile(file_name);
        break;
      case 'p':
        skiplist.Print();
        break;
      case 'q':
        quit = true;
        break;
      default:
        std::cin.ignore(256, '\n');
        std::cout << usageMessage();
        break;
    }
  }
}
}  // namespace skiplist