#ifndef NODE_DETAIL_NODE_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define NODE_DETAIL_NODE_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "yaml-cpp/dll.h"
#include "yaml-cpp/emitterstyle.h"
#include "yaml-cpp/node/detail/node_ref.h"
#include "yaml-cpp/node/ptr.h"
#include "yaml-cpp/node/type.h"
#include <set>
#include <atomic>

namespace YAML {
namespace detail {
class node : public clife_base {
 private:
  struct less {
    bool operator ()(const node_ptr& l, const node_ptr& r) const
      {return l->m_index < r->m_index;}
  };

 public:
  typedef clife_ptr<node> ptr;

  node();
  ~node();

  node(const node&) = delete;
  node& operator=(const node&) = delete;

  bool is(const node::ptr& rhs) const { return m_pRef.get() == rhs->m_pRef.get(); }
  const node_ref::ptr ref() const { return m_pRef; }

  bool is_defined() const { return m_pRef->is_defined(); }
  const Mark& mark() const { return m_pRef->mark(); }
  NodeType::value type() const { return m_pRef->type(); }

  const std::string& scalar() const { return m_pRef->scalar(); }
  const std::string& tag() const { return m_pRef->tag(); }
  EmitterStyle::value style() const { return m_pRef->style(); }

  template <typename T>
  bool equals(const T& rhs);
  bool equals(const char* rhs);

  void mark_defined();
  void add_dependency(const node::ptr& rhs);
  void set_ref (const node::ptr& rhs);
  void set_data(const node::ptr& rhs);

  void set_mark(const Mark& mark);

  void set_type(NodeType::value type);
  void set_null();
  void set_scalar(const std::string& scalar);
  void set_tag(const std::string& tag);

  // style
  void set_style(EmitterStyle::value style);

  // size/iterator
  std::size_t size() const;

  const_node_iterator begin() const;
  node_iterator begin();

  const_node_iterator end() const;
  node_iterator end();

  // sequence
  void push_back(const node::ptr& input);
  void insert(const node::ptr& key, const node::ptr& value);

  // indexing
  template <typename Key>
  node::ptr get(const Key& key) const {
    // NOTE: this returns a non-const node so that the top-level Node can wrap
    // it, and returns a pointer so that it can be nullptr (if there is no such
    // key).
    return static_cast<const node_ref*>(m_pRef.get())->get(key);
  }
  template <typename Key>
  node::ptr get(const Key& key) {
    node::ptr value = m_pRef->get(key);
    value->add_dependency(node::ptr(this));
    return value;
  }
  template <typename Key>
  bool remove(const Key& key) {
    return m_pRef->remove(key);
  }

  node::ptr get(const node::ptr& key) const;
  node::ptr get(const node::ptr& key);

  bool remove(const node::ptr& key);

  // map
  template <typename Key, typename Value>
  void force_insert(const Key& key, const Value& value) {
    m_pRef->force_insert(key, value);
  }

  void destroy_cross_references();

 private:
  node_ref::ptr m_pRef;
  using nodes = std::set<node::ptr, less>;
  nodes m_dependencies;
  size_t m_index = {0};
  static YAML_CPP_API std::atomic<size_t> m_amount;

  bool m_crossReferencesDestroed = {false};
};
}  // namespace detail
}  // namespace YAML

#endif  // NODE_DETAIL_NODE_H_62B23520_7C8E_11DE_8A39_0800200C9A66
