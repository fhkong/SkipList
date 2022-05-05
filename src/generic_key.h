#pragma once

#include <cstring>

namespace skiplist {
template <size_t KeySize>
class GenericKey {
 public:
  inline void SetFromInteger(int64_t key) {
    memset(data_, 0, KeySize);
    memcpy(data_, &key, sizeof(int64_t));
  }

  inline int64_t ToInteger() const { return *reinterpret_cast<int64_t *>(const_cast<char *>(data_)); }

  friend std::ostream &operator<<(std::ostream &os, const GenericKey &key) {
    os << key.ToInteger();
    return os;
  }

 private:
  char data_[KeySize];
};

template <size_t KeySize>
class GenericComparator {
 public:
  inline int operator()(const GenericKey<KeySize> &lhs, const GenericKey<KeySize> &rhs) const {
    if (lhs.ToInteger() < rhs.ToInteger()) {
      return -1;
    } else if (lhs.ToInteger() == rhs.ToInteger()) {
      return 0;
    }
    return 1;
  }
};

}  // namespace skiplist