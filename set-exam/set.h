#include <cassert> // assert
#include <iterator> // std::reverse_iterator
#include <utility> // std::pair, std::swap

template <typename T>
struct set {
  struct iterator;
  using const_iterator = iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  set() : root()
  {}
  set(set const& other) : root() {
    try {
      for (T const& i : other) {
        insert(i);
      }
    } catch (...) {
      clear();
      throw;
    }
  }
  set& operator=(set const& other) {
    if (this != &other) {
      set tmp(other);
      tmp.swap(*this);
    }
    return *this;
  }
  ~set() {
    clear();
  }

  void clear() {
    del(root.left);
    root.left = nullptr;
  }

  bool empty() {
    return root.left == nullptr;
  }

private:
  struct base_node;

public:
  void del(base_node* a) {
    if (a != nullptr) {
      del(a->left);
      del(a->right);
      delete(static_cast<node*>(a));
    }
  }

  const_iterator begin() const {
    auto tmp = const_cast<base_node*>(&root);
    while (tmp->left != nullptr) {
      tmp = tmp->left;
    }
    return const_iterator(tmp);
  }
  const_iterator end() const {
    return const_iterator(const_cast<base_node*>(&root));
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  std::pair<iterator, bool> insert(T const& other) {
    iterator res = find(other);
    if (res != end()) {
      return {res, false};
    }
    bool to_left = true;
    base_node* prev = &root;
    base_node* place = root.left;
    while (place != nullptr) {
      prev = place;
      if (static_cast<node*>(place)->value < other) {
        place = place->right;
        to_left = false;
      } else {
        place = place->left;
        to_left = true;
      }
    }
    node* tmp = new node(other);
    if (to_left) {
      prev->left = tmp;
    } else {
      prev->right = tmp;
    }
    tmp->parent = prev;
    return {iterator(static_cast<base_node*>(tmp)), true};
  }
  iterator erase(const_iterator iter) {
    base_node* tmp = iter.get();
    ++iter;
    if (tmp->left == nullptr && tmp->right == nullptr) {
      if (tmp->is_right()) {
        tmp->parent->right = nullptr;
      } else {
        tmp->parent->left = nullptr;
      }
    } else if (tmp->left != nullptr && tmp->right == nullptr) {
      if (tmp->is_right()) {
        tmp->parent->right = tmp->left;
      } else {
        tmp->parent->left = tmp->left;
      }
      tmp->left->parent = tmp->parent;
    } else if (tmp->left == nullptr && tmp->right != nullptr) {
      if (tmp->is_right()) {
        tmp->parent->right = tmp->right;
      } else {
        tmp->parent->left = tmp->right;
      }
      tmp->right->parent = tmp->parent;
    } else if (tmp->right->left == nullptr) {
      if (tmp->is_right()) {
        tmp->parent->right = tmp->right;
      } else {
        tmp->parent->left = tmp->right;
      }
      tmp->right->parent = tmp->parent;
      tmp->right->left = tmp->left;
      tmp->left->parent = tmp->right;
    } else {
      base_node* nw = tmp->right;
      while (nw->left != nullptr) {
        nw = nw->left;
      }
      if (nw->is_right()) {
        nw->parent->right = nw->right;
      } else {
        nw->parent->left = nw->right;
      }
      if (nw->right != nullptr) {
        nw->right->parent = nw->parent;
      }
      if (tmp->is_right()) {
        tmp->parent->right = nw;
      } else {
        tmp->parent->left = nw;
      }
      nw->parent = tmp->parent;
      nw->left = tmp->left;
      tmp->left->parent = nw;
      nw->right = tmp->right;
      tmp->right->parent = nw;
    }
    delete(static_cast<node*>(tmp));
    return iter;
  }
  const_iterator find(T const& other) const {
    base_node* tmp = root.left;
    while (tmp != nullptr) {
      if (static_cast<node*>(tmp)->value < other) {
        tmp = tmp->right;
      } else if (other < static_cast<node*>(tmp)->value) {
        tmp = tmp->left;
      } else {
        return const_iterator(tmp);
      }
    }
    return end();
  }
  const_iterator lower_bound(T const& other) const {
    const_iterator tmp = find(other);
    if (tmp != end()) {
      return tmp;
    }
    return upper_bound(other);
  }
  const_iterator upper_bound(T const& other) const {
    base_node* tmp = root.left;
    base_node* res = nullptr;
    while (tmp != nullptr) {
      if (other < static_cast<node*>(tmp)->value) {
        res = tmp;
        tmp = tmp->left;
      } else {
        tmp = tmp->right;
      }
    }
    if (res == nullptr) {
      return end();
    }
    return const_iterator(res);
  }

  void swap(set& other) {
    if (!this->empty() && !other.empty()) {
      std::swap(root.left, other.root.left);
      std::swap(root.left->parent, other.root.left->parent);
    } else if (!empty() && other.empty()) {
      other.root.left = root.left;
      root.left->parent = &other.root;
      root.left = nullptr;
    } else if (empty() && !other.empty()) {
      root.left = other.root.left;
      other.root.left->parent = &root;
      other.root.left = nullptr;
    }
  }

  friend void swap(set& a, set& b) {
    a.swap(b);
  }

private:
  struct base_node {
    base_node* parent{nullptr};
    base_node* left{nullptr};
    base_node* right{nullptr};

    bool is_left() {
      return parent->left == this;
    }

    bool is_right() {
      return parent->right == this;
    }
  };

  struct node : base_node {
    T value;

    explicit node(T const& val) : base_node(), value(val) {}
  };

  base_node root;

public:
  struct iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T const;
    using pointer = T const*;
    using reference = T const&;

    iterator() = default;
    explicit iterator(base_node* other) : it(other) {}
    iterator(set<T>::iterator const& other) : it(other.it) {}
    iterator& operator=(set<T>::iterator const& other) = default;

    reference operator*() const {
      return static_cast<node*>(it)->value;
    }
    pointer operator->() const {
      return &static_cast<node*>(it)->value;
    }

    iterator& operator++() & {
      base_node* tmp = it;
      if (tmp->right != nullptr) {
        tmp = tmp->right;
        while (tmp->left != nullptr) {
          tmp = tmp->left;
        }
      } else {
        while (tmp->parent != nullptr && tmp->is_right()) {
          tmp = tmp->parent;
        }
        if (tmp->parent != nullptr) {
          tmp = tmp->parent;
        }
      }
      it = tmp;
      return *this;
    }
    iterator operator++(int) & {
      iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    iterator& operator--() & {
      base_node* tmp = it;
      if (tmp->left != nullptr) {
        tmp = tmp->left;
        while (tmp->right != nullptr) {
          tmp = tmp->right;
        }
      } else {
        while (tmp->parent != nullptr && tmp->is_left()) {
          tmp = tmp->parent;
        }
        if (tmp->parent != nullptr) {
          tmp = tmp->parent;
        }
      }
      it = tmp;
      return *this;
    }
    iterator operator--(int) & {
      iterator tmp(*this);
      --(*this);
      return tmp;
    }

    bool operator==(iterator const& b) const {
      return this->it== b.it;
    }

    bool operator!=(iterator const& b) const {
      return this->it != b.it;
    }

    base_node* get() {
      return it;
    }

  private:
    void del(node* a) {
      if (a != nullptr) {
        del(static_cast<node*>(a->left));
        del(static_cast<node*>(a->right));
        delete(a);
      }
    }

    base_node* it;
  };
};
