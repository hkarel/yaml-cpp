#ifndef VALUE_DETAIL_ITERATOR_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define VALUE_DETAIL_ITERATOR_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "yaml-cpp/dll.h"
#include "yaml-cpp/node/detail/node_iterator.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/ptr.h"
#include <cstddef>
#include <iterator>


namespace YAML {
namespace detail {
struct iterator_value;

template <typename V>
class iterator_base {

 private:
  template <typename>
  friend class iterator_base;
  struct enabler {};
  using base_type = node_iterator;

  struct proxy {
    explicit proxy(const V& x) : m_ref(x) {}
    V* operator->() { return std::addressof(m_ref); }
    operator V*() { return std::addressof(m_ref); }

    V m_ref;
  };

 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = V;
  using difference_type = std::ptrdiff_t;
  using pointer = V*;
  using reference = V&;

 public:
  iterator_base() : m_iterator() {}
  explicit iterator_base(base_type rhs)
      : m_iterator(rhs) {}

  template <class W>
  iterator_base(const iterator_base<W>& rhs,
                typename std::enable_if<std::is_convertible<W*, V*>::value,
                                        enabler>::type = enabler())
      : m_iterator(rhs.m_iterator) {}

  iterator_base<V>& operator++() {
    ++m_iterator;
    return *this;
  }

  iterator_base<V> operator++(int) {
    iterator_base<V> iterator_pre(*this);
    ++(*this);
    return iterator_pre;
  }

  template <typename W>
  bool operator==(const iterator_base<W>& rhs) const {
    return m_iterator == rhs.m_iterator;
  }

  template <typename W>
  bool operator!=(const iterator_base<W>& rhs) const {
    return m_iterator != rhs.m_iterator;
  }

  value_type operator*() const {
    const typename base_type::value_type& v = *m_iterator;
    if (v.pNode)
      return value_type(Node(*v));
    if (v.first && v.second)
      return value_type(Node(v.first), Node(v.second));
    return value_type();
  }

  proxy operator->() const { return proxy(**this); }

 private:
  base_type m_iterator;
};
}  // namespace detail
}  // namespace YAML

#endif  // VALUE_DETAIL_ITERATOR_H_62B23520_7C8E_11DE_8A39_0800200C9A66
