#include <functional>
#include <mutex>   //NOLINT
#include <thread>  //NOLINT
#include <vector>

#include "generic_key.h"
#include "gtest/gtest.h"
#include "skiplist.h"

namespace skiplist {

static const int ITER_NUM = 10;

template <typename... Args>
void LaunchParallelTest(int num_threads, Args &&... args) {
  std::vector<std::thread> thread_pool;
  for (int thread_iter = 0; thread_iter < num_threads; thread_iter++) {
    thread_pool.push_back(std::thread(args..., thread_iter));
  }
  for (auto &t : thread_pool) {
    t.join();
  }
}
// must use const std::vector<int64_t>& keys, can't ignore the const keywords. Otherwise, this will be error at creating
// thread;
void InsertHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist, const std::vector<int64_t> &keys,
                  __attribute__((unused)) int thread_iter = 0) {
  GenericKey<8> index_key;
  // GenericValue<8> index_value;
  GenericValue<8> index_value;
  for (auto key : keys) {
    index_key.SetFromInteger(key);
    index_value.SetFromInteger(key) ;
    skiplist->Insert(index_key, index_value);
  }
}
void InsertSplitHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist,
                       const std::vector<int64_t> &keys, int thread_num, int thread_iter) {
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  for (auto key : keys) {
    if (key % thread_num == thread_iter) {
      index_key.SetFromInteger(key);
      index_value.SetFromInteger(key);
      skiplist->Insert(index_key, index_value);
    }
  }
}

void DeleteHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist, const std::vector<int64_t> &keys,
                  __attribute__((unused)) int thread_iter = 0) {
  GenericKey<8> index_key;
  for (const auto &key : keys) {
    index_key.SetFromInteger(key);
    skiplist->Remove(index_key);
  }
}

void DeleteSplitHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist,
                       const std::vector<int64_t> &keys, int thread_num, int thread_iter) {
  GenericKey<8> index_key;
  for (auto key : keys) {
    if (key % thread_num == thread_iter) {
      index_key.SetFromInteger(key);
      skiplist->Remove(index_key);
    }
  }
}

void LookupHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist, const std::vector<int64_t> &keys,
                  __attribute__((unused)) int thread_iter = 0) {
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  std::vector<GenericValue<8>> result;
  for (const auto &key : keys) {
    index_key.SetFromInteger(key);
    index_value.SetFromInteger(key);
    result.clear();
    skiplist->Lookup(index_key, &result);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], index_value);
  }
}

void LookupSplitHelper(SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> *skiplist,
                       const std::vector<int64_t> &keys, int thread_num, int thread_iter) {
  GenericKey<8> index_key;
  GenericValue<8> index_value;
  std::vector<GenericValue<8>> result;
  for (auto key : keys) {
    if (key % thread_num == thread_iter) {
      index_key.SetFromInteger(key);
      index_value.SetFromInteger(key) ;
      skiplist->Lookup(index_key, &result);
      EXPECT_EQ(result.size(), 1);
      EXPECT_EQ(result[0], index_value);
    }
  }
}

// basic test
TEST(SkipListTest, BasicTest) { EXPECT_EQ(1, 1); }

// insert test
TEST(SkipListTest, InsertTest1) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scala_keys = 100000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scala_keys; i++) {
    keys.push_back(i);
  }

  // insert
  int thread_num = 2;
  LaunchParallelTest(thread_num, InsertHelper, &skiplist, keys);

  // Lookup
  int i = 0;
  for (auto iter : skiplist) {
    EXPECT_EQ(iter.first.ToInteger(), keys[i++]);
  }
  EXPECT_EQ(skiplist.Size(), keys.size());
}

// insert test
TEST(SkipListTest, InsertTest2) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scala_keys = 100000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scala_keys; i++) {
    keys.push_back(i);
  }

  // insert
  int thread_num = 4;
  LaunchParallelTest(thread_num, InsertSplitHelper, &skiplist, keys, thread_num);

  // Lookup
  int i = 0;
  for (auto iter : skiplist) {
    EXPECT_EQ(iter.first.ToInteger(), keys[i++]);
  }
  EXPECT_EQ(skiplist.Size(), keys.size());
}

TEST(SkipListTest, DeleteTest1) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 100000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }
  InsertHelper(&skiplist, keys);

  int thread_num = 2;
  LaunchParallelTest(thread_num, DeleteHelper, &skiplist, keys);

  EXPECT_EQ(skiplist.Size(), 0);
}

TEST(SkipListTest, DeleteTest2) {
  GenericComparator<8> comparator;
  int max_height = 18;
  SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

  int scale_keys = 100000;
  std::vector<int64_t> keys;
  for (int i = 1; i <= scale_keys; i++) {
    keys.push_back(i);
  }
  InsertHelper(&skiplist, keys);

  int thread_num = 4;
  LaunchParallelTest(thread_num, DeleteSplitHelper, &skiplist, keys, thread_num);

  EXPECT_EQ(skiplist.Size(), 0);
}

TEST(SkipListTest, MixTest1) {
  for (int i = 0; i < ITER_NUM; i++) {
    GenericComparator<8> comparator;
    int max_height = 18;
    SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

    int scala_keys = 100000;
    std::vector<int64_t> for_insert;
    std::vector<int64_t> for_delete;
    int sieve = 2;
    for (int i = 1; i <= scala_keys; i++) {
      if (i % sieve == 0) {
        for_insert.push_back(i);
      } else {
        for_delete.push_back(i);
      }
    }
    InsertHelper(&skiplist, for_delete);

    auto insert_task = [&](int tid) { InsertHelper(&skiplist, for_insert, tid); };
    auto delete_task = [&](int tid) { DeleteHelper(&skiplist, for_delete, tid); };
    std::vector<std::function<void(int)>> tasks;
    tasks.push_back(insert_task);
    tasks.push_back(delete_task);

    int thread_num = 10;
    std::vector<std::thread> thread_pool;
    for (int i = 0; i < thread_num; i++) {
      thread_pool.push_back(std::thread{tasks[i % tasks.size()], i});
    }
    for (auto &t : thread_pool) {
      t.join();
    }

    int size = 0;
    for (auto iter : skiplist) {
      EXPECT_EQ(iter.first.ToInteger(), for_insert[size++]);
    }

    EXPECT_EQ(skiplist.Size(), size);
  }
}

TEST(SkipListTest, MixTest2) {
  for (int i = 0; i < ITER_NUM; i++) {
    GenericComparator<8> comparator;
    int max_height = 18;
    SkipList<GenericKey<8>, GenericValue<8>, GenericComparator<8>> skiplist(comparator, max_height);

    int scala_keys = 100000;
    std::vector<int64_t> perserved_keys;
    std::vector<int64_t> dynamic_keys;
    int sieve = 5;
    for (int i = 1; i <= scala_keys; i++) {
      if (i % sieve == 0) {
        perserved_keys.push_back(i);
      } else {
        dynamic_keys.push_back(i);
      }
    }
    InsertHelper(&skiplist, perserved_keys);

    int size = 0;
    for (auto iter : skiplist) {
      EXPECT_EQ(iter.first.ToInteger(), perserved_keys[size++]);
    }
    EXPECT_EQ(skiplist.Size(), size);

    auto insert_task = [&](int tid) { InsertHelper(&skiplist, dynamic_keys, tid); };
    auto delete_task = [&](int tid) { DeleteHelper(&skiplist, dynamic_keys, tid); };
    auto lookup_task = [&](int tid) { LookupHelper(&skiplist, perserved_keys, tid); };

    std::vector<std::function<void(int)>> tasks;
    tasks.emplace_back(insert_task);
    tasks.emplace_back(delete_task);
    tasks.emplace_back(lookup_task);

    int thread_num = 6;
    std::vector<std::thread> thread_pool;
    for (int i = 0; i < thread_num; i++) {
      thread_pool.emplace_back(std::thread{tasks[i % tasks.size()], i});
    }
    for (auto &t : thread_pool) {
      t.join();
    }

    size = 0;
    for (auto iter : skiplist) {
      if (iter.first.ToInteger() % sieve == 0) {
        EXPECT_EQ(iter.first.ToInteger(), perserved_keys[size++]);
      }
    }
    EXPECT_EQ(size, perserved_keys.size());
  }
}

}  // namespace skiplist