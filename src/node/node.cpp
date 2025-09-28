#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/detail/node.h"

#include "../nodeevents.h"
#include "../nodebuilder.h"

namespace YAML {

template<typename Key>
std::string key_to_string(const Key& key) {
  return streamable_to_string<Key, is_streamable<std::stringstream, Key>::value>().impl(key);
}

Node::Node()
    : m_isValid(true) {}

Node::~Node() {
//  if (m_pNode && (m_pNode->clife_count() == 1))
//    m_pNode->destroy_cross_references();
}

Node::Node(NodeType::value type)
    : m_isValid(true),
      m_pNode(new detail::node) {
  m_pNode->set_type(type);
}

Node::Node(const detail::iterator_value& rhs)
    : m_isValid(rhs.m_isValid),
      m_invalidKey(rhs.m_invalidKey),
      m_pNode(rhs.m_pNode)
{}

Node::Node(Zombie)
    : m_isValid(false)
{}

Node::Node(Zombie, const std::string& key)
    : m_isValid(false),
      m_invalidKey(key)
{}

Node::Node(const detail::node_ptr& node)
    : m_isValid(true),
      m_pNode(node)
{}

void Node::EnsureNodeExists() const {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  if (!m_pNode) {
    m_pNode = detail::node::ptr(new detail::node);
    m_pNode->set_null();
  }
}

bool Node::IsDefined() const {
  if (!m_isValid) {
    return false;
  }
  return m_pNode ? m_pNode->is_defined() : true;
}

Mark Node::Mark() const {
  if (!m_isValid) {
    throw InvalidNode(m_invalidKey);
  }
  return m_pNode ? m_pNode->mark() : Mark::null_mark();
}

NodeType::value Node::Type() const {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  return m_pNode ? m_pNode->type() : NodeType::Null;
}

const std::string& Node::Scalar() const {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  return m_pNode ? m_pNode->scalar() : detail::node_data::empty_scalar();
}

const std::string& Node::Tag() const {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  return m_pNode ? m_pNode->tag() : detail::node_data::empty_scalar();
}

void Node::SetTag(const std::string& tag) {
  EnsureNodeExists();
  m_pNode->set_tag(tag);
}

EmitterStyle::value Node::Style() const {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  return m_pNode ? m_pNode->style() : EmitterStyle::Default;
}

void Node::SetStyle(EmitterStyle::value style) {
  EnsureNodeExists();
  m_pNode->set_style(style);
}

// assignment
bool Node::is(const Node& rhs) const {
  if (!m_isValid || !rhs.m_isValid)
    throw InvalidNode(m_invalidKey);
  if (!m_pNode || !rhs.m_pNode)
    return false;
  return m_pNode->is(rhs.m_pNode);
}

Node& Node::operator=(const Node& rhs) {
  if (is(rhs))
    return *this;
  AssignNode(rhs);
  return *this;
}

void Node::reset(const YAML::Node& rhs) {
  if (!m_isValid || !rhs.m_isValid)
    throw InvalidNode(m_invalidKey);
  m_pNode = rhs.m_pNode;
}

void Node::Assign(const std::string& rhs) {
  EnsureNodeExists();
  m_pNode->set_scalar(rhs);
}

void Node::Assign(const char* rhs) {
  EnsureNodeExists();
  m_pNode->set_scalar(rhs);
}

void Node::Assign(char* rhs) {
  EnsureNodeExists();
  m_pNode->set_scalar(rhs);
}

void Node::AssignData(const Node& rhs) {
  EnsureNodeExists();
  rhs.EnsureNodeExists();

  m_pNode->set_data(rhs.m_pNode);
}

void Node::AssignNode(const Node& rhs) {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  rhs.EnsureNodeExists();

  if (!m_pNode) {
    m_pNode = rhs.m_pNode;
    return;
  }

  m_pNode->set_ref(rhs.m_pNode);
  m_pNode = rhs.m_pNode;
}

// size/iterator
std::size_t Node::size() const {
  if (!m_isValid)
    throw InvalidNode(m_invalidKey);
  return m_pNode ? m_pNode->size() : 0;
}

const_iterator Node::begin() const {
  if (!m_isValid)
    return const_iterator();
  return m_pNode ? const_iterator(m_pNode->begin())
                 : const_iterator();
}

iterator Node::begin() {
  if (!m_isValid)
    return iterator();
  return m_pNode ? iterator(m_pNode->begin()) : iterator();
}

const_iterator Node::end() const {
  if (!m_isValid)
    return const_iterator();
  return m_pNode ? const_iterator(m_pNode->end()) : const_iterator();
}

iterator Node::end() {
  if (!m_isValid)
    return iterator();
  return m_pNode ? iterator(m_pNode->end()) : iterator();
}

void Node::push_back(const Node& rhs) {
  EnsureNodeExists();
  rhs.EnsureNodeExists();
  m_pNode->push_back(rhs.m_pNode);
}

const Node Node::operator[](const Node& key) const {
  EnsureNodeExists();
  key.EnsureNodeExists();
  detail::node::ptr value =
    static_cast<const detail::node*>(m_pNode.get())->get(key.m_pNode);
  if (!value) {
    return Node(ZombieNode, key_to_string(key));
  }
  return Node(value);
}

Node Node::operator[](const Node& key) {
  EnsureNodeExists();
  key.EnsureNodeExists();
  detail::node::ptr value = m_pNode->get(key.m_pNode);
  return Node(value);
}

bool Node::remove(const Node& key) {
  EnsureNodeExists();
  key.EnsureNodeExists();
  return m_pNode->remove(key.m_pNode);
}

void Node::destroy_cross_references() {
  if (m_pNode)
    m_pNode->destroy_cross_references();
}

Node Clone(const Node& node) {
  NodeEvents events(node);
  NodeBuilder builder;
  events.Emit(builder);
  return builder.Root();
}
}  // namespace YAML
