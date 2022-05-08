#include <cassert>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>

#include "skiplist.h"

namespace skiplist {
SKIPLIST_TEMPLATE_ARGUMENTS
SKIPLIST_TYPE::SkipList(const KeyComparator &comparator, size_t max_height, size_t rnd)
    : comparator_(comparator), max_height_(max_height), rnd_(rnd), size_(0) {
  LOG_INFO("Construct SkipList with max_height: %lu and random seed: %lu", max_height, rnd);
  srand(rnd_);  // Set random seed for random function
  // Invalid key, value to head node
  KeyType key{};
  ValueType value{};
  head_ = CreateNode(key, value, max_height);
}

SKIPLIST_TEMPLATE_ARGUMENTS
bool SKIPLIST_TYPE::Lookup(const KeyType &key, std::vector<ValueType> *result) {
  std::lock_guard<std::mutex> _(mtx_);
  LOG_INFO("Lookup: <%ld>", key.ToInteger());

  int level = max_height_ - 1;
  auto cur = head_;
  while (level >= 0) {
    auto p = cur->forward_[level];
    while (p && comparator_(p->key_, key) < 0) {
      p = p->forward_[level];
      cur = cur->forward_[level];
    }
    if (p && comparator_(p->key_, key) == 0) {
      result->push_back(p->value_);
      return true;
    }
    level--;
  }
  return false;
}

SKIPLIST_TEMPLATE_ARGUMENTS
bool SKIPLIST_TYPE::Insert(const KeyType &key, const ValueType &value) {
  std::lock_guard<std::mutex> _(mtx_);
  // firstly, we will lookup the skiplist for the key to be inserted.
  int level = max_height_ - 1;
  auto cur = head_;
  while (level >= 0) {
    auto p = cur->forward_[level];
    while (p && comparator_(p->key_, key) < 0) {
      p = p->forward_[level];
      cur = cur->forward_[level];
    }
    if (p && comparator_(p->key_, key) == 0) {
      LOG_WARN("The key: %lu has already existed!", key.ToInteger());
      return false;
    }
    level--;
  }

  size_t height = RandomHeight();
  LOG_INFO("ThreadID: %lu, Insert: <%ld, %ld> with height: %lu", std::hash<std::thread::id>{}(std::this_thread::get_id()), key.ToInteger(), value, height);
  SkipListNode *new_node = new SkipListNode(key, value, height);

  level = max_height_ - 1;
  cur = head_;
  while (level >= 0) {
    auto p = cur->forward_[level];
    while (p && comparator_(p->key_, key) < 0) {
      p = p->forward_[level];
      cur = cur->forward_[level];
    }
    if (level < static_cast<int>(height)) {
      new_node->forward_[level] = cur->forward_[level];
      cur->forward_[level] = new_node;
    }
    level--;
  }
  size_ += 1;
  return true;
}

SKIPLIST_TEMPLATE_ARGUMENTS
bool SKIPLIST_TYPE::Remove(const KeyType &key) {
  std::lock_guard<std::mutex> _(mtx_);
  LOG_INFO("Remove: %ld", key.ToInteger());

  int level = max_height_ - 1;
  auto cur = head_;
  SkipListNode *delete_node = nullptr;
  while (level >= 0) {
    auto p = cur->forward_[level];
    while (p && comparator_(p->key_, key) < 0) {
      p = p->forward_[level];
      cur = cur->forward_[level];
    }
    if (p && comparator_(p->key_, key) == 0) {
      delete_node = p;
      cur->forward_[level] = p->forward_[level];
    }
    level--;
  }
  if (delete_node == nullptr) {
    LOG_WARN("The key is not exists.");
    return false;
  }
  delete delete_node;
  size_ -= 1;
  return true;
}

SKIPLIST_TEMPLATE_ARGUMENTS
typename SKIPLIST_TYPE::SkipListNode *SKIPLIST_TYPE::CreateNode(const KeyType &key, const ValueType &value,
                                                                int height) {
  LOG_INFO("CreateNode with level: %d", height);
  SkipListNode *new_node = new SkipListNode(key, value, height);
  assert(new_node != nullptr);
  return new_node;
}

SKIPLIST_TEMPLATE_ARGUMENTS
size_t SKIPLIST_TYPE::RandomHeight() {
  // Increase height with probablility 1 in kBranching
  static const size_t kBranching = 2;
  size_t height = 1;
  while (height < max_height_ && (size_t)rand() < (RAND_MAX / kBranching)) {
    height += 1;
  }
  assert(height <= max_height_);
  assert(0 < height);
  return height;
}

SKIPLIST_TEMPLATE_ARGUMENTS
void SKIPLIST_TYPE::Print() {
  for (int h = max_height_ - 1; h >= 0; --h) {
    auto p = head_->forward_[h];
    std::cout << "Level-" << h << ": ";
    while (p != nullptr) {
      std::cout << "<" << p->key_ << "," << p->value_ << "> ";
      p = p->forward_[h];
    }
    std::cout << std::endl;
  }
}

SKIPLIST_TEMPLATE_ARGUMENTS
void SKIPLIST_TYPE::InsertFromFile(const std::string &file_name) {
  std::ifstream input(file_name);
  if (!input) {
    std::cout << "Can't open the file: " << file_name << std::endl;
    return;
  }
  int64_t key;
  int64_t value;
  KeyType index_key;
  while (input) {
    input >> key >> value;
    std::cout << "<" << key << "," << value << ">" << std::endl;
    index_key.SetFromInteger(key);
    Insert(index_key, value);
  }
}

////////////////// Iterator /////////////////
SKIPLIST_TEMPLATE_ARGUMENTS
typename SKIPLIST_TYPE::Iterator SKIPLIST_TYPE::begin() {
  LOG_INFO("Iterator begin");
  return Iterator{head_->forward_[0]};
}

SKIPLIST_TEMPLATE_ARGUMENTS
typename SKIPLIST_TYPE::Iterator SKIPLIST_TYPE::end() {
  LOG_INFO("Iterator end");
  // int level = max_height_ - 1;
  // auto cur = head_;
  // while (level >= 0) {
  //   auto p = cur->forward_[level];
  //   while (p) {
  //     p = p->forward_[level];
  //     cur = cur->forward_[level];
  //   }
  //   level--;
  // }
  // return Iterator{cur->forward_[0]};
  return Iterator{nullptr};
}

SKIPLIST_TEMPLATE_ARGUMENTS
SKIPLIST_TYPE::~SkipList() {
  if (head_ != nullptr) {
    while (head_->forward_[0] != nullptr) {
      auto node = head_->forward_[0];
      head_->forward_[0] = node->forward_[0];
      delete node;
    }
    delete head_;
  }
}

template class SkipList<GenericKey<8>, int64_t, GenericComparator<8>>;
}  // namespace skiplist
