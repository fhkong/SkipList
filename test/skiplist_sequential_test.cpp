#include <vector>

#include "generic_key.h"
#include "gtest/gtest.h"
#include "skiplist.h"

namespace skiplist {
TEST(SkipListTest, EmptyTest) {
  GenericComparator<8> comparator;
  int max_height = 5;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);
  EXPECT_EQ(skiplist.Size(), 0);
  std::vector<GenericValue<8>> result;
  GenericKey<8> index_key;
  index_key.SetFromInteger(0);
  EXPECT_EQ(false, skiplist.Lookup(index_key, &result));
  EXPECT_EQ(false, skiplist.Remove(index_key));
}

TEST(SkipListTest, InsertTest) {
  GenericComparator<8> comparator;
  int max_height = 12;  // This can store 2 ^ 12 keys
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  GenericKey<8> index_key;
  GenericValue<8> index_value;
  std::vector<int64_t> keys;
  int scala_keys = 1000;
  for (int i = 1; i <= scala_keys; i++) {
    keys.push_back(i);
  }
  for (auto &key : keys) {
    index_key.SetFromInteger(key);
    index_value.SetFromInteger(key);
    skiplist.Insert(index_key, index_value);
  }

  std::vector<GenericValue<8>> result;
  for (auto key : keys) {
    index_key.SetFromInteger(key);
    result.clear();
    EXPECT_EQ(true, skiplist.Lookup(index_key, &result));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].ToInteger(), key);
  }

  int i = 0;
  for (auto iter : skiplist) {
    EXPECT_EQ(iter.first.ToInteger(), keys[i]);
    i++;
  }

  i = 0;
  for (auto iter = skiplist.begin(); iter != skiplist.end(); ++iter) {
    EXPECT_EQ((*iter).first.ToInteger(), keys[i]);
    i++;
  }
}

TEST(SkipListTest, DeleteTest1) {
  int max_height = 12;
  GenericComparator<8> comparator;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 1000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }

  // Insert the keys to skiplist
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  for (auto &key : keys) {
    index_key.SetFromInteger(key);
    index_value.SetFromInteger(key);
    skiplist.Insert(index_key, index_value);
  }

  int i = 0;
  for (auto iter : skiplist) {
    EXPECT_EQ(iter.first.ToInteger(), keys[i]);
    i++;
  }
  EXPECT_EQ(skiplist.Size(), keys.size());

  for (auto key : keys) {
    index_key.SetFromInteger(key);
    EXPECT_EQ(true, skiplist.Remove(index_key));
  }

  EXPECT_EQ(skiplist.Size(), 0);
}

TEST(SkipListTest, DeleteTest2) {
  int max_height = 12;
  GenericComparator<8> comparator;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 1000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }

  // Insert the keys to skiplist
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  for (auto &key : keys) {
    index_key.SetFromInteger(key);
    index_value.SetFromInteger(key);
    skiplist.Insert(index_key, index_value);
  }

  int i = 0;
  for (auto iter : skiplist) {
    EXPECT_EQ(iter.first.ToInteger(), keys[i]);
    i++;
  }
  EXPECT_EQ(skiplist.Size(), keys.size());

  // for delete random value
  std::set<int64_t> for_delete;
  srand(0x3f3f3f3f);
  for (int i = 0; i < 100; i++) {
    int n = (rand() % scale_keys) + 1;
    for_delete.insert(n);
  }
  for (auto key : for_delete) {
    index_key.SetFromInteger(key);
    EXPECT_EQ(true, skiplist.Remove(index_key));
  }

  EXPECT_EQ(skiplist.Size(), keys.size() - for_delete.size());

  std::vector<GenericValue<8>> result;
  i = 0;
  for (auto iter = skiplist.begin(); iter != skiplist.end();) {
    index_key.SetFromInteger(keys[i]);
    if (for_delete.count(keys[i]) == 0) {
      EXPECT_EQ((*iter).first.ToInteger(), keys[i]);
      ++iter;
    } else {
      EXPECT_EQ(false, skiplist.Lookup(index_key, &result));
    }
    i++;
  }
}

TEST(SkipListTest, MixTest1) {
  GenericComparator<8> comparator;
  int max_height = 12;
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 1000;
  std::vector<int64_t> for_insert;
  std::vector<int64_t> for_delete;
  for (int i = 1; i <= scale_keys; i++) {
    index_key.SetFromInteger(i);
    index_value.SetFromInteger(i);
    skiplist.Insert(index_key, index_value);
    if (i % 2 == 0) {
      for_insert.push_back(i);
    } else {
      for_delete.push_back(i);
    }
  }
  EXPECT_EQ((int)skiplist.Size(), scale_keys);

  for (auto key : for_delete) {
    index_key.SetFromInteger(key);
    EXPECT_EQ(true, skiplist.Remove(index_key));
  }

  for (auto key : for_insert) {
    index_key.SetFromInteger(key);
    index_value.SetFromInteger(key);
    EXPECT_EQ(false, skiplist.Insert(index_key, index_value));
  }
  EXPECT_EQ(skiplist.Size(), for_insert.size());

  for (auto key : for_delete) {
    index_key.SetFromInteger(key);
    EXPECT_EQ(false, skiplist.Remove(index_key));
  }

  int i = 0;
  for (auto iter : skiplist) {
    EXPECT_EQ(iter.first.ToInteger(), for_insert[i]);
    i++;
  }
}
}  // namespace skiplist