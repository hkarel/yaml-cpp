#ifndef VALUE_DETAIL_NODE_DATA_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define VALUE_DETAIL_NODE_DATA_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "yaml-cpp/dll.h"
#include "yaml-cpp/node/detail/node_iterator.h"
#include "yaml-cpp/node/iterator.h"
#include "yaml-cpp/node/ptr.h"
#include "yaml-cpp/node/type.h"

namespace YAML {
namespace detail {
class node;
}  // namespace detail
}  // namespace YAML

namespace YAML {
namespace detail {
class YAML_CPP_API node_data : public clife_base {
 public:
  typedef clife_ptr<node_data> ptr;

  node_data();
  node_data(const node_data&) = delete;
  node_data& operator=(const node_data&) = delete;

  void mark_defined();
  void set_mark(const Mark& mark);
  void set_type(NodeType::value type);
  void set_tag(const std::string& tag);
  void set_null();
  void set_scalar(const std::string& scalar);
  void set_style(EmitterStyle::value style);

  bool is_defined() const { return m_isDefined; }
  const Mark& mark() const { return m_mark; }
  NodeType::value type() const {
    return m_isDefined ? m_type : NodeType::Undefined;
  }
  const std::string& scalar() const { return m_scalar; }
  const std::string& tag() const { return m_tag; }
  EmitterStyle::value style() const { return m_style; }

  // size/iterator
  std::size_t size() const;

  const_node_iterator begin() const;
  node_iterator begin();

  const_node_iterator end() const;
  node_iterator end();

  // sequence
  void push_back(const node_ptr& node);
  void insert(const node_ptr& key, const node_ptr& value);

  // indexing
  template <typename Key>
  node_ptr get(const Key& key) const;
  template <typename Key>
  node_ptr get(const Key& key);
  template <typename Key>
  bool remove(const Key& key);

  node_ptr get(const node_ptr& key) const;
  node_ptr get(const node_ptr& key);
  bool remove(const node_ptr& key);

  // map
  template <typename Key, typename Value>
  void force_insert(const Key& key, const Value& value);

  void destroy_cross_references();

 public:
  static const std::string& empty_scalar();

 private:
  void compute_seq_size() const;
  void compute_map_size() const;

  void reset_sequence();
  void reset_map();

  void insert_map_pair(const node_ptr& key, const node_ptr& value);
  void convert_to_map();
  void convert_sequence_to_map();

  template <typename T>
  static node_ptr convert_to_node(const T& rhs);

 private:
  bool m_isDefined;
  Mark m_mark;
  NodeType::value m_type;
  std::string m_tag;
  EmitterStyle::value m_style;

  // scalar
  std::string m_scalar;

  // sequence
  using node_seq = std::vector<node_ptr>;
  node_seq m_sequence;

  mutable std::size_t m_seqSize;

  // map
  using node_map = std::vector<std::pair<node_ptr, node_ptr>>;
  node_map m_map;

  using kv_pair = std::pair<node_ptr, node_ptr>;
  using kv_pairs = std::list<kv_pair>;
  mutable kv_pairs m_undefinedPairs;

  bool m_crossReferencesDestroed = {false};
};
}
}

#endif  // VALUE_DETAIL_NODE_DATA_H_62B23520_7C8E_11DE_8A39_0800200C9A66
