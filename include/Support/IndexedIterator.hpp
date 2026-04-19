#pragma once
#include <iterator>

template <typename Container> class IndexedIterator {
  Container data;

public:
  // Automatically determine the type inside the container
  using ValueType = typename Container::value_type;

  struct Entry {
    ValueType value;
    size_t index;
  };

  // Constructor takes ownership of the container (via move)
  explicit IndexedIterator(Container d) : data(std::move(d)) {}

  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = Entry;
    using difference_type = std::ptrdiff_t;

    const IndexedIterator &parent;
    size_t currentIndex;

    Iterator(const IndexedIterator &p, size_t idx)
        : parent(p), currentIndex(idx) {}

    Entry operator*() const {
      return {parent.data[currentIndex], currentIndex};
    }

    Iterator &operator++() {
      currentIndex++;
      return *this;
    }

    bool operator!=(const Iterator &other) const {
      return currentIndex != other.currentIndex;
    }
  };

  Iterator begin() { return Iterator(*this, 0); }
  Iterator end() { return Iterator(*this, data.size()); }
};
