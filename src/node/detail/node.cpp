//#include <algorithm>
//#include <cassert>
//#include <iterator>
//#include <sstream>

#include "yaml-cpp/node/detail/node.h"  // IWYU pragma: keep

namespace YAML {
namespace detail {

node::node() : m_pRef(new node_ref)//, m_dependencies{}, m_index{}
{}

node::~node()
{}

void node::mark_defined() {
  if (is_defined())
    return;

  m_pRef->mark_defined();
  for (node::ptr dependency : m_dependencies)
    dependency->mark_defined();
  m_dependencies.clear();
}

void node::add_dependency(const node::ptr& rhs) {
  if (is_defined())
    rhs->mark_defined();
  else
    m_dependencies.insert(rhs);
}

void node::set_ref(const node::ptr& rhs) {
  if (rhs->is_defined())
    mark_defined();
  m_pRef = rhs->m_pRef;
}
void node::set_data(const node::ptr& rhs) {
  if (rhs->is_defined())
    mark_defined();
  m_pRef->set_data(rhs->m_pRef);
}

void node::set_mark(const Mark& mark) {
  m_pRef->set_mark(mark);
}

void node::set_type(NodeType::value type) {
  if (type != NodeType::Undefined)
    mark_defined();
  m_pRef->set_type(type);
}
void node::set_null() {
  mark_defined();
  m_pRef->set_null();
}
void node::set_scalar(const std::string& scalar) {
  mark_defined();
  m_pRef->set_scalar(scalar);
}
void node::set_tag(const std::string& tag) {
  mark_defined();
  m_pRef->set_tag(tag);
}

// style
void node::set_style(EmitterStyle::value style) {
  mark_defined();
  m_pRef->set_style(style);
}

// size/iterator
std::size_t node::size() const {
  return m_pRef->size();
}

const_node_iterator node::begin() const {
  return static_cast<const node_ref&>(*m_pRef).begin();
}

node_iterator node::begin() {
  return m_pRef->begin();
}

const_node_iterator node::end() const {
  return static_cast<const node_ref&>(*m_pRef).end();
}
node_iterator node::end() {
  return m_pRef->end();
}

// sequence
void node::push_back(const node::ptr& input) {
  m_pRef->push_back(input);
  input->add_dependency(node::ptr(this));
  m_index = m_amount.fetch_add(1);
}

void node::insert(const node::ptr& key, const node::ptr& value) {
  m_pRef->insert(key, value);
  key->add_dependency(node::ptr(this));
  value->add_dependency(node::ptr(this));
}

node::ptr node::get(const node::ptr& key) const {
  // NOTE: this returns a non-const node so that the top-level Node can wrap
  // it, and returns a pointer so that it can be nullptr (if there is no such
  // key).
  return static_cast<const node_ref*>(m_pRef.get())->get(key);
}

node::ptr node::get(const node::ptr& key) {
  node::ptr value = m_pRef->get(key);
  key->add_dependency(node::ptr(this));
  value->add_dependency(node::ptr(this));
  return value;
}

bool node::remove(const node::ptr& key) {
  return m_pRef->remove(key);
}

void node::destroy_cross_references() {
  if (m_crossReferencesDestroed)
    return;
  m_crossReferencesDestroed = true;

  if (m_pRef)
    m_pRef->destroy_cross_references();

  for (node::ptr node : m_dependencies)
    if (node)
      node->destroy_cross_references();
}

}  // namespace detail
}  // namespace YAML
