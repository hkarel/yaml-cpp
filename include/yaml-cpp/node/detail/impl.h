#ifndef NODE_DETAIL_IMPL_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define NODE_DETAIL_IMPL_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "yaml-cpp/node/detail/node.h"
#include "yaml-cpp/node/detail/node_data.h"

#include <algorithm>
#include <type_traits>

namespace YAML {
namespace detail {
template <typename Key, typename Enable = void>
struct get_idx {
  static node_ptr get(const std::vector<node_ptr>& /* sequence */, const Key&) {
    return {};
  }
};

template <typename Key>
struct get_idx<Key,
               typename std::enable_if<std::is_unsigned<Key>::value &&
                                       !std::is_same<Key, bool>::value>::type> {
  static node_ptr get(const std::vector<node_ptr>& sequence, const Key& key) {
    return key < sequence.size() ? sequence[key] : node_ptr();
  }

  static node_ptr get(std::vector<node_ptr>& sequence, const Key& key) {
    if (key > sequence.size() || (key > 0 && !sequence[key - 1]->is_defined()))
      return node_ptr();
    if (key == sequence.size())
      sequence.push_back(node_ptr(new node));
    return sequence[key];
  }
};

template <typename Key>
struct get_idx<Key, typename std::enable_if<std::is_signed<Key>::value>::type> {
  static node_ptr get(const std::vector<node_ptr>& sequence, const Key& key) {
    return key >= 0 ? get_idx<std::size_t>::get(
                          sequence, static_cast<std::size_t>(key))
                    : node_ptr();
  }
  static node_ptr get(std::vector<node_ptr>& sequence, const Key& key) {
    return key >= 0 ? get_idx<std::size_t>::get(
                          sequence, static_cast<std::size_t>(key))
                    : node_ptr();
  }
};

template <typename Key, typename Enable = void>
struct remove_idx {
  static bool remove(std::vector<node_ptr>&, const Key&, std::size_t&) {
    return false;
  }
};

template <typename Key>
struct remove_idx<
    Key, typename std::enable_if<std::is_unsigned<Key>::value &&
                                 !std::is_same<Key, bool>::value>::type> {

  static bool remove(std::vector<node_ptr>& sequence, const Key& key,
                     std::size_t& seqSize) {
    if (key >= sequence.size()) {
      return false;
    } else {
      sequence.erase(sequence.begin() + key);
      if (seqSize > key) {
          --seqSize;
      }
      return true;
    }
  }
};

template <typename Key>
struct remove_idx<Key,
                  typename std::enable_if<std::is_signed<Key>::value>::type> {

  static bool remove(std::vector<node_ptr>& sequence, const Key& key,
                     std::size_t& seqSize) {
    return key >= 0 ? remove_idx<std::size_t>::remove(
                          sequence, static_cast<std::size_t>(key), seqSize)
                    : false;
  }
};

template <typename T>
inline bool node::equals(const T& rhs) {
  T lhs;
  if (convert<T>::decode(Node(node_ptr(this)), lhs)) {
    return lhs == rhs;
  }
  return false;
}

inline bool node::equals(const char* rhs) {
  std::string lhs;
  if (convert<std::string>::decode(Node(node_ptr(this)), lhs)) {
    return lhs == rhs;
  }
  return false;
}

// indexing
template <typename Key>
inline node_ptr node_data::get(const Key& key) const {
  switch (m_type) {
    case NodeType::Map:
      break;
    case NodeType::Undefined:
    case NodeType::Null:
      return nullptr;
    case NodeType::Sequence:
      if (node_ptr pNode = get_idx<Key>::get(m_sequence, key))
        return pNode;
      return nullptr;
    case NodeType::Scalar:
      throw BadSubscript(m_mark, key);
  }

  auto it = std::find_if(m_map.begin(), m_map.end(), [&](const kv_pair m) {
    return m.first->equals(key);
  });

  return it != m_map.end() ? it->second : node_ptr();
}

template <typename Key>
inline node_ptr node_data::get(const Key& key) {
  switch (m_type) {
    case NodeType::Map:
      break;
    case NodeType::Undefined:
    case NodeType::Null:
    case NodeType::Sequence:
      if (node_ptr pNode = get_idx<Key>::get(m_sequence, key)) {
        m_type = NodeType::Sequence;
        return pNode;
      }

      convert_to_map();
      break;
    case NodeType::Scalar:
      throw BadSubscript(m_mark, key);
  }

  auto it = std::find_if(m_map.begin(), m_map.end(), [&](const kv_pair m) {
    return m.first->equals(key);
  });

  if (it != m_map.end()) {
    return it->second;
  }

  node_ptr k = convert_to_node(key);
  node_ptr v {new node};
  insert_map_pair(k, v);
  return v;
}

template <typename Key>
inline bool node_data::remove(const Key& key) {
  if (m_type == NodeType::Sequence) {
    return remove_idx<Key>::remove(m_sequence, key, m_seqSize);
  }

  if (m_type == NodeType::Map) {
    kv_pairs::iterator it = m_undefinedPairs.begin();
    while (it != m_undefinedPairs.end()) {
      kv_pairs::iterator jt = std::next(it);
      if (it->first->equals(key)) {
        m_undefinedPairs.erase(it);
      }
      it = jt;
    }

    auto iter = std::find_if(m_map.begin(), m_map.end(), [&](const kv_pair m) {
      return m.first->equals(key);
    });

    if (iter != m_map.end()) {
      m_map.erase(iter);
      return true;
    }
  }

  return false;
}

// map
template <typename Key, typename Value>
inline void node_data::force_insert(const Key& key, const Value& value) {
  switch (m_type) {
    case NodeType::Map:
      break;
    case NodeType::Undefined:
    case NodeType::Null:
    case NodeType::Sequence:
      convert_to_map();
      break;
    case NodeType::Scalar:
      throw BadInsert();
  }

  node_ptr k = convert_to_node(key);
  node_ptr v = convert_to_node(value);
  insert_map_pair(k, v);
}

template <typename T>
inline node_ptr node_data::convert_to_node(const T& rhs) {
  Node value = convert<T>::encode(rhs);
  value.EnsureNodeExists();
  return value.m_pNode;
}
}
}

#endif  // NODE_DETAIL_IMPL_H_62B23520_7C8E_11DE_8A39_0800200C9A66
