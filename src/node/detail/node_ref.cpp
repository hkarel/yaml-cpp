//#include <algorithm>
//#include <cassert>
//#include <iterator>
//#include <sstream>

//#include "yaml-cpp/exceptions.h"
//#include "yaml-cpp/node/detail/memory.h"
#include "yaml-cpp/node/detail/node_ref.h"
#include "yaml-cpp/node/detail/node.h"  // IWYU pragma: keep
#include "yaml-cpp/node/detail/node_data.h"
//#include "yaml-cpp/node/detail/node_iterator.h"
//#include "yaml-cpp/node/ptr.h"
//#include "yaml-cpp/node/type.h"

namespace YAML {
namespace detail {

const_node_iterator node_ref::begin() const {
  return static_cast<const node_data*>(m_pData.get())->begin();
}
node_iterator node_ref::begin() {
  return m_pData->begin();
}

const_node_iterator node_ref::end() const {
  return static_cast<const node_data*>(m_pData.get())->end();
}
node_iterator node_ref::end() {
  return m_pData->end();
}

void node_ref::push_back(const node_ptr& node) {
  m_pData->push_back(node);
}
void node_ref::insert(const node_ptr& key, const node_ptr& value) {
  m_pData->insert(key, value);
}

node_ptr node_ref::get(const node_ptr& key) const {
  return static_cast<const node_data*>(m_pData.get())->get(key);
}
node_ptr node_ref::get(const node_ptr& key) {
  return m_pData->get(key);
}
bool node_ref::remove(const node_ptr& key) {
  return m_pData->remove(key);
}

void node_ref::destroy_cross_references() {
  if (m_crossReferencesDestroed)
    return;
  m_crossReferencesDestroed = true;

  if (m_pData)
    m_pData->destroy_cross_references();
}

}  // namespace detail
}  // namespace YAML
