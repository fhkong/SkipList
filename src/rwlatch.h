/**
 *
 * */
#pragma once

#include <condition_variable>
#include <mutex>  //NOLINT

namespace skiplist {
class ReaderWriterLatch {
  using mutex_t = std::mutex;
  using cond_t = std::condition_variable;
  static const uint32_t MAX_READERS = 0x3f3f3f3f;

 public:
  ReaderWriterLatch() = default;
  ~ReaderWriterLatch() { std::lock_guard<mutex_t> _(mtx_); }  // 析构的时候必须要等所有的线程都处理完
  void RLock() {
    std::unique_lock<std::mutex> mtx(mtx_);
    while (writer_entered_ || reader_count_ >= MAX_READERS) {
      reader_.wait(mtx);
    }
    reader_count_ += 1;
  }

  void RUnLock() {
    std::lock_guard<std::mutex> _(mtx_);
    reader_count_ -= 1;
    if (writer_entered_) {
      if (reader_count_ == 0) {
        writer_.notify_one();
      }
    } else {
      if (reader_count_ == MAX_READERS - 1) {
        reader_.notify_one();
      }
    }
  }

  void WLock() {
    std::unique_lock<std::mutex> mtx(mtx_);
    while (writer_entered_) {
      reader_.wait(mtx);
    }
    writer_entered_ = true;
    while (reader_count_ > 0) {
      writer_.wait(mtx);
    }
  }

  void WUnLock() {
    std::lock_guard<std::mutex> _(mtx_);
    writer_entered_ = false;
    reader_.notify_all();
  }

 private:
  std::mutex mtx_;
  cond_t reader_;
  cond_t writer_;
  bool writer_entered_{false};
  uint32_t reader_count_{0};
};
}  // namespace skiplist