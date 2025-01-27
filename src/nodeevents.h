#ifndef NODE_NODEEVENTS_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define NODE_NODEEVENTS_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include "yaml-cpp/anchor.h"
#include "yaml-cpp/node/detail/node.h"
#include "yaml-cpp/node/detail/node_ref.h"

#include <map>
#include <vector>

namespace YAML {
namespace detail {
class node;
}  // namespace detail
}  // namespace YAML

namespace YAML {
class EventHandler;
class Node;

class NodeEvents {
 public:
  explicit NodeEvents(const Node& node);
  ~NodeEvents();

  NodeEvents(const NodeEvents&) = delete;
  NodeEvents(NodeEvents&&) = delete;
  NodeEvents& operator=(const NodeEvents&) = delete;
  NodeEvents& operator=(NodeEvents&&) = delete;

  void Emit(EventHandler& handler);

 private:
  struct less {
    bool operator ()(const detail::node_ref_ptr& l, const detail::node_ref_ptr& r) const {
      return l.get() < r.get();
    }
  };

  class AliasManager {
   public:
    AliasManager() : m_anchorByIdentity{}, m_curAnchor(0) {}

    void RegisterReference(const detail::node_ptr& node);
    anchor_t LookupAnchor(const detail::node_ptr& node) const;

   private:
    anchor_t _CreateNewAnchor() { return ++m_curAnchor; }

   private:
    using AnchorByIdentity = std::map<const detail::node_ref_ptr, anchor_t, less>;
    AnchorByIdentity m_anchorByIdentity;

    anchor_t m_curAnchor;
  };

  void Setup(const detail::node_ptr& node);
  void Emit(const detail::node_ptr& node, EventHandler& handler,
            AliasManager& am) const;
  bool IsAliased(const detail::node_ptr& node) const;

 private:
  detail::node_ptr m_root;

  using RefCount = std::map<detail::node_ref_ptr, int, less>;
  RefCount m_refCount;
};
}  // namespace YAML

#endif  // NODE_NODEEVENTS_H_62B23520_7C8E_11DE_8A39_0800200C9A66
