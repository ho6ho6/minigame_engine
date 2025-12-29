#include "include/component/component_manager.hpp"

namespace n_compomanager {

    bool ComponentManager::HasComponent(EntityId eid, ComponentId cid) const {
        std::lock_guard<std::mutex> lk(masksMutex_);
        auto it = masks_.find(eid);
        if (it == masks_.end()) return false;
        return it->second.test(static_cast<size_t>(cid));
    }

    ComponentMask ComponentManager::GetMask(EntityId eid) const {
        std::lock_guard<std::mutex> lk(masksMutex_);
        auto it = masks_.find(eid);
        if (it == masks_.end()) return ComponentMask{};
        return it->second;
    }

    ComponentManager g_componentManager;

    // 翻訳単位間で使用されるテンプレートの明示的なインスタンス化
    //template void ComponentManager::AddComponent<n_component::MoveComponent>(EntityId, const n_component::MoveComponent&);
    //template void ComponentManager::RemoveComponent<n_component::MoveComponent>(EntityId);
    //template std::optional<n_component::MoveComponent> ComponentManager::GetComponent<n_component::MoveComponent>(EntityId) const;
    //template void ComponentManager::SetComponent<n_component::MoveComponent>(EntityId, const n_component::MoveComponent&);

    // 他のコンポーネントタイプも同様にインスタンス化する

} 