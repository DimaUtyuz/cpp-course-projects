#pragma once
#include <cstddef>

template <typename T>
struct vector {
  using iterator = T*;
  using const_iterator = T const*;

  vector() :
    size_(0),
    capacity_(0),
    data_(nullptr) {}

  vector(vector const& other) : vector() {
    extend_buffer(other.size_, other.data_, other.size_);
  }

  vector& operator=(vector const& other) {
    if (this != &other) {
      vector t = vector(other);
      t.swap(*this);
    }
    return *this;
  }

  ~vector() {
    destruct_range(begin(), end());
    operator delete(data_);
    size_ = capacity_ = 0;
    data_ = nullptr;
  }

  T& operator[](size_t i) {
    return data_[i];
  }
  T const& operator[](size_t i) const {
    return data_[i];
  }

  T* data() {
    return data_;
  }
  T const* data() const {
    return data_;
  }
  size_t size() const {
    return size_;
  }

  T& front() {
    return data_[0];
  }
  T const& front() const {
    return data_[0];
  }

  T& back() {
    return data_[size_ - 1];
  }
  T const& back() const {
    return data_[size_ - 1];
  }
  void push_back(T const& t) {
    if (capacity_ == size_) {
      T t_tmp = t;
      extend_buffer(capacity_ == 0 ? 1 : capacity_ * 2, data_, size_);
      new (data_ + size_) T(t_tmp);
    } else {
      new (data_ + size_) T(t);
    }
    size_++;
  }
  void pop_back() {
    size_--;
    data_[size_].~T();
  }

  bool empty() const {
    return size_ == 0;
  }

  size_t capacity() const {
    return capacity_;
  }
  void reserve(size_t new_cap) {
    if (new_cap > capacity_) {
      extend_buffer(new_cap, data_, size_);
    }
  }
  void shrink_to_fit() {
    if (size_ != capacity_) {
      extend_buffer(size_, data_, size_);
    }
  }

  void clear() {
    destruct_range(begin(), end());
    size_ = 0;
  }

  void swap(vector& other) noexcept {
    std::swap(other.size_, size_);
    std::swap(other.capacity_, capacity_);
    std::swap(other.data_, data_);
  }

  iterator begin() {
    return data_;
  }
  iterator end() {
    return data_ + size_;
  }

  const_iterator begin() const {
    return data_;
  }
  const_iterator end() const {
    return data_ + size_;
  }

  iterator insert(const_iterator pos, T const& t) {
    size_t p = pos - begin();
    push_back(t);
    for (size_t i = size_ - 1; i > p; i--) {
      std::swap(data_[i], data_[i - 1]);
    }
    return const_cast<iterator>(pos);
  }

  iterator erase(const_iterator pos) {
    return erase(pos, pos + 1);
  }

  iterator erase(const_iterator first, const_iterator last) {
    for (auto it = const_cast<iterator>(last); it < end(); it++) {
      std::swap(*it, *(it - (last - first)));
    }
    auto old_size = size_;
    size_ -= last - first;
    destruct_range(data_ + size_, data_ + old_size);
    return const_cast<iterator>(first);
  }

private:
  void extend_buffer(size_t new_capacity, T* source, size_t count) {
    if (new_capacity == 0) {
      operator delete(data_);
      data_ = nullptr;
      capacity_ = 0;
      return;
    }
    T* new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
    size_t i = 0;
    try {
      while (i != count) {
        new (new_data + i) T(source[i]);
        i++;
      }
    } catch (...) {
      destruct_range(new_data, new_data + i);
      operator delete(new_data);
      throw;
    }
    destruct_range(begin(), end());
    operator delete(data_);
    data_ = new_data;
    capacity_ = new_capacity;
    size_ = count;
  }

  static void destruct_range(T* start, T* end) {
    if (start != nullptr) {
      for (auto it = --end; it >= start; it--) {
        it->~T();
      }
    }
  }

  T* data_;
  size_t size_;
  size_t capacity_;
}; 
