#include <vector>

#include "skiplist.h"
#include "gtest/gtest.h"
#include "generic_key.h"

namespace skiplist{
TEST(SkipListTest, EmptyTest) {
  GenericComparator<8> comparator;
  int max_height = 5;
  SkipList<GenericKey<8>, int64_t, GenericComparator<8>> skiplist(comparator, max_height);

  GenericKey<8> index_key;
  int64_t value;
  std::vector<int64_t> keys;
  int scala_keys = 100;
  for (int i=1; i<=scala_keys; i++) {
    keys.push_back(i);
  }
  for (auto& key : keys) {
    index_key.SetFromInteger(key);
    value = key; 
    skiplist.Insert(index_key, value);
  }

  std::vector<int64_t> result;
  for (auto key : keys) {
    index_key.SetFromInteger(key);
    result.clear();
    EXPECT_EQ(true, skiplist.Lookup(index_key, &result));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], key);
  }
}
}