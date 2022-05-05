#pragma once

#include <mutex>  // NOLINT
#include <vector>

namespace skiplist {

#define SKIPLIST_TEMPLATE_ARGUMENTS template <typename KeyType, typename ValueType, typename KeyComparator>
#define SKIPLIST_TYPE SkipList<KeyType, ValueType, KeyComparator>

SKIPLIST_TEMPLATE_ARGUMENTS
class SkipList {
 public:
  explicit SkipList(const KeyComparator &comparator, size_t max_height = 5, size_t rnd = 0xdeadbeef);

  bool Insert(const KeyType &key, const ValueType &value);
  bool Remove(const KeyType &key);
  bool Lookup(const KeyType &key, std::vector<ValueType> *result);

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

 private:
  std::mutex mtx_;
  KeyComparator comparator_;
  size_t max_height_;
  size_t cur_height_;
  size_t rnd_;
  SkipListNode *head_;
};

}  // namespace skiplist
