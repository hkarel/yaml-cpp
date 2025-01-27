#ifndef NODE_IMPL_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define NODE_IMPL_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/detail/node.h"
#include "yaml-cpp/node/iterator.h"
#include "yaml-cpp/node/node.h"
//#include "yaml-cpp/node/convert.h"

#include <sstream>
#include <string>

namespace YAML {

template <typename T>
inline Node::Node(const T& rhs)
    : m_isValid(true),
      m_pNode(new detail::node) {
  Assign(rhs);
}

// access

// template helpers
template <typename T, typename S>
struct as_if {
  explicit as_if(const Node& node_) : node(node_) {}
  const Node& node;

  T operator()(const S& fallback) const {
    if (!node.m_pNode)
      return fallback;

    T t = fallback;
    if (convert<T>::decode(node, t))
      return t;
    return fallback;
  }
};

template <typename S>
struct as_if<std::string, S> {
  explicit as_if(const Node& node_) : node(node_) {}
  const Node& node;

  std::string operator()(const S& fallback) const {
    if (node.Type() == NodeType::Null)
      return "null";
    if (node.Type() != NodeType::Scalar)
      return fallback;
    return node.Scalar();
  }
};

template <typename T>
struct as_if<T, void> {
  explicit as_if(const Node& node_) : node(node_) {}
  const Node& node;

  T operator()() const {
    if (!node.m_pNode) // no fallback
      throw InvalidNode(node.m_invalidKey);

    T t;
    if (convert<T>::decode(node, t))
      return t;
    throw TypedBadConversion<T>(node.Mark());
  }
};

template <>
struct as_if<std::string, void> {
  explicit as_if(const Node& node_) : node(node_) {}
  const Node& node;

  std::string operator()() const {
    if (node.Type() == NodeType::Undefined) // no fallback
      throw InvalidNode(node.m_invalidKey);
    if (node.Type() == NodeType::Null)
      return "null";
    if (node.Type() != NodeType::Scalar)
      throw TypedBadConversion<std::string>(node.Mark());
    return node.Scalar();
  }
};

// access functions
template <typename T>
inline T Node::as() const {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  return as_if<T, void>(*this)();
}

template <typename T, typename S>
inline T Node::as(const S& fallback) const {
  if (!m_isValid)
    return fallback;
  return as_if<T, S>(*this)(fallback);
}

template <typename T>
inline Node& Node::operator=(const T& rhs) {
  Assign(rhs);
  return *this;
}

template <typename T>
inline void Node::Assign(const T& rhs) {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  AssignData(convert<T>::encode(rhs));
}

// sequence
template <typename T>
inline void Node::push_back(const T& rhs) {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  push_back(Node(rhs));
}

template<typename Key>
std::string key_to_string(const Key& key) {
  return streamable_to_string<Key, is_streamable<std::stringstream, Key>::value>().impl(key);
}

// indexing
template <typename Key>
inline const Node Node::operator[](const Key& key) const {
  EnsureNodeExists();
  detail::node::ptr value =
    static_cast<const detail::node*>(m_pNode.get())->get(key);
  if (!value) {
    return Node(ZombieNode, key_to_string(key));
  }
  return Node(value);
}

template <typename Key>
inline Node Node::operator[](const Key& key) {
  EnsureNodeExists();
  detail::node::ptr value = m_pNode->get(key);
  return Node(value);
}

template <typename Key>
inline bool Node::remove(const Key& key) {
  EnsureNodeExists();
  return m_pNode->remove(key);
}

// map
template <typename Key, typename Value>
inline void Node::force_insert(const Key& key, const Value& value) {
  EnsureNodeExists();
  m_pNode->force_insert(key, value);
}

// free functions
inline bool operator==(const Node& lhs, const Node& rhs) { return lhs.is(rhs); }
}  // namespace YAML

#endif  // NODE_IMPL_H_62B23520_7C8E_11DE_8A39_0800200C9A66
