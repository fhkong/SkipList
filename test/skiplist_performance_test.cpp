/** Performance analysis for skiplist
 * Author: fhkong
 * Time: 2022.05.09
 * **/
#include <chrono>
#include <thread>
#include <vector>

#include "gtest/gtest.h"
#include "skiplist.h"

namespace skiplist {
template <typename... Args>
void LuanchParallelTest(int thread_num, Args &&... args) {
  std::vector<std::thread> thread_pool;
  for (int thread_iter = 0; thread_iter < thread_num; thread_iter++) {
    thread_pool.emplace_back(std::thread{args..., thread_iter});
  }
  for (auto &t : thread_pool) {
    t.join();
  }
}

void InsertHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist,
                  const std::vector<int64_t> &keys, int thread_iter = 0) {
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  for (const auto &key : keys) {
    index_key.SetFromInteger(key);
    index_value.SetFromInteger(key);
    skiplist->Insert(index_key, index_value);
  }
}

void InsertSplitHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist,
                       const std::vector<int64_t> &keys, int thread_num, int thread_iter) {
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  for (const auto &key : keys) {
    if (key % thread_num == thread_iter) {
      index_key.SetFromInteger(key);
      index_value.SetFromInteger(key);
      skiplist->Insert(index_key, index_value);
    }
  }
}

void LookupHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist,
                  const std::vector<int64_t> &keys, int thread_iter = 0) {
  GenericKey<8> index_key;
  std::vector<GenericValue<8>> result;
  for (const auto &key : keys) {
    index_key.SetFromInteger(key);
    result.clear();
    skiplist->Lookup(index_key, &result);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].ToInteger(), key);
  }
}

void LookupSplitHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist,
                       const std::vector<int64_t> &keys, int thread_num, int thread_iter) {
  GenericKey<8> index_key;
  std::vector<GenericValue<8>> result;
  for (const auto &key : keys) {
    if (key % thread_num == thread_iter) {
      index_key.SetFromInteger(key);
      result.clear();
      skiplist->Lookup(index_key, &result);
      EXPECT_EQ(result.size(), 1);
      EXPECT_EQ(result[0].ToInteger(), key);
    }
  }
}

// insert 100w items through single thread
TEST(PerformanceTest, InsertTest1) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 1000000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }
  auto start_time = std::chrono::high_resolution_clock::now();
  InsertHelper(&skiplist, keys);
  auto end_time = std::chrono::high_resolution_clock::now();

  EXPECT_EQ(skiplist.Size(), keys.size());
  auto span = end_time - start_time;
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
  std::cout << "\n--------------- Insert Performance (Single Thread)--------------------" << std::endl;
  std::cout << "Insert " << scale_keys << " items\n"
            << "\t Time Duration: " << duration << std::endl
            << "\t Throughout: " << (float)(scale_keys)*1e6 / duration << std::endl;
}

// insert 100w items through multiple thread
TEST(PerformanceTest, InsertTest2) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 1000000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }
  auto start_time = std::chrono::high_resolution_clock::now();
  int thread_num = 4;
  LuanchParallelTest(thread_num, InsertSplitHelper, &skiplist, keys, thread_num);
  auto end_time = std::chrono::high_resolution_clock::now();

  EXPECT_EQ(skiplist.Size(), keys.size());
  auto span = end_time - start_time;
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
  std::cout << "\n--------------- Insert Performance (Four Threads)--------------------" << std::endl;
  std::cout << "Insert " << scale_keys << " items\n"
            << "\t Time Duration: " << duration << std::endl
            << "\t Throughout: " << (float)(scale_keys)*1e6 / duration << std::endl;
}

// Lookup 100w items through single thread
TEST(PerformanceTest, LookupTest1) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 1000000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }
  InsertHelper(&skiplist, keys);
  EXPECT_EQ(skiplist.Size(), keys.size());

  auto start_time = std::chrono::high_resolution_clock::now();
  LookupHelper(&skiplist, keys);
  auto end_time = std::chrono::high_resolution_clock::now();

  auto span = end_time - start_time;
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
  std::cout << "\n--------------- Lookup Performance (Single Thread)--------------------" << std::endl;
  std::cout << "Lookup " << scale_keys << " items\n"
            << "\t Time Duration: " << duration << std::endl
            << "\t Throughout: " << (float)(scale_keys)*1e6 / duration << std::endl;
}

// insert 100w items through multiple thread
TEST(PerformanceTest, LookupTest2) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 1000000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }
  InsertHelper(&skiplist, keys);
  auto start_time = std::chrono::high_resolution_clock::now();
  int thread_num = 4;
  LuanchParallelTest(thread_num, LookupSplitHelper, &skiplist, keys, thread_num);
  auto end_time = std::chrono::high_resolution_clock::now();

  EXPECT_EQ(skiplist.Size(), keys.size());
  auto span = end_time - start_time;
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
  std::cout << "\n--------------- Lookup Performance (Four Threads)--------------------" << std::endl;
  std::cout << "Lookup " << scale_keys << " items\n"
            << "\t Time Duration: " << duration << std::endl
            << "\t Throughout: " << (float)(scale_keys)*1e6 / duration << std::endl;
}
}  // namespace skiplist