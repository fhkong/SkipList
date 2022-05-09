#pragma once

#include <cassert>
#include <mutex>  // NOLINT
#include <vector>

#include "generic_key.h"
#include "logger.h"
#include "rwlatch.h"

namespace skiplist {

#define SKIPLIST_TEMPLATE_ARGUMENTS template <typename KeyType, typename ValueType, typename KeyComparator>
#define SKIPLIST_TYPE SkipList<KeyType, ValueType, KeyComparator>

SKIPLIST_TEMPLATE_ARGUMENTS
class SkipList {
 public:
  explicit SkipList(const KeyComparator &comparator, size_t max_height = 5, size_t branching = 2,
                    size_t rnd = 0xdeadbeef);

  bool Insert(const KeyType &key, const ValueType &value);
  bool Remove(const KeyType &key);
  bool Lookup(const KeyType &key, std::vector<ValueType> *result);

  size_t Size() { return size_; }
  void Print();
  void InsertFromFile(const std::string &file_name);

  ~SkipList();

 private:
  class SkipListNode {
   public:
    explicit SkipListNode(const KeyType &key, const ValueType &value, int height)
        : key_(key), value_(value), height_(height) {
      assert(0 < height);  // 0 represent the lowest level.
      forward_ = static_cast<SkipListNode **>(new SkipListNode *[height]);
      for (int i = 0; i < height; i++) {
        forward_[i] = nullptr;
      }
    }

    ~SkipListNode() {
      if (forward_ != nullptr) {
        delete[] forward_;
      }
    }

    KeyType key_;
    ValueType value_;
    size_t height_;           // for delete operation
    SkipListNode **forward_;  // The forward pointers array
  };
  SkipListNode *CreateNode(const KeyType &key, const ValueType &value, int height);
  size_t RandomHeight();

  /************** Iterator Unit **********************/
 private:
  class Iterator {
    using KVPAIR = std::pair<KeyType, ValueType>;

   public:
    Iterator(SkipListNode *node) { cur = node; }

    KVPAIR operator*() {
      assert(cur != nullptr);
      return KVPAIR{cur->key_, cur->value_};
    }

    Iterator &operator++() {
      assert(cur != nullptr);
      this->cur = this->cur->forward_[0];
      return *this;
    }
    bool operator==(const Iterator &itr) const { return cur == itr.cur; }
    bool operator!=(const Iterator &itr) const { return cur != itr.cur; }
    ~Iterator() = default;

   private:
    SkipListNode *cur;
  };

 public:
  Iterator begin();
  Iterator end();

 private:
  // std::mutex mtx_;
  KeyComparator comparator_;
  size_t max_height_;
  size_t branching_;
  size_t rnd_;
  size_t size_;
  ReaderWriterLatch rwlatch_;
  SkipListNode *head_;
};

}  // namespace skiplist
