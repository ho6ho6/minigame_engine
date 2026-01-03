#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include <cstdint>
#include <unordered_map>
#include <bitset>
#include <mutex>
#include <optional>
#include "include/component/componentDefaults.h" // n_component::MoveComponent, TransformComponent, etc.
#include <type_traits>

namespace n_compomanager
{


    // 最大コンポーネント数（必要に応じて増やす）
    constexpr size_t kMaxComponents = 64;
    using ComponentMask = std::bitset<kMaxComponents>;
    using EntityId = int64_t;

    // コンポーネントIDを列挙
    enum class ComponentId : uint8_t {
        Transform = 0,
        Move = 1,
        Light = 2,
        Rigidbody = 3,
        Start = 4,
        Finish = 5,
        IsPlayer = 6,
        Sprite = 7,
        // ここに追加していく
    };

    // 先頭付近に追加（既に type_traits を include しているのでそのまま使えます）
    template<typename T>
    using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;


    // 主テンプレート 特殊化がない時->エラーを出す
    template<typename T>
    struct ComponentTypeId;

    template<> struct ComponentTypeId<n_component::TransformComponent> { static constexpr ComponentId value = ComponentId::Transform; };
    template<> struct ComponentTypeId<n_component::MoveComponent>      { static constexpr ComponentId value = ComponentId::Move; };
    template<> struct ComponentTypeId<n_component::LightComponent>     { static constexpr ComponentId value = ComponentId::Light; };
    template<> struct ComponentTypeId<n_component::RigidbodyComponent> { static constexpr ComponentId value = ComponentId::Rigidbody; };
    template<> struct ComponentTypeId<n_component::StartComponent>     { static constexpr ComponentId value = ComponentId::Start; };
    template<> struct ComponentTypeId<n_component::FinishComponent>    { static constexpr ComponentId value = ComponentId::Finish; };
    template<> struct ComponentTypeId<n_component::IsPlayerComponent>  { static constexpr ComponentId value = ComponentId::IsPlayer; };
    template<> struct ComponentTypeId<n_component::SpriteComponent>    { static constexpr ComponentId value = ComponentId::Sprite; };

    // ComponentManager
    class ComponentManager {
    public:
        // コンポーネントがアタッチされているか
        bool HasComponent(EntityId eid, ComponentId cid) const;
        template<typename T>
            requires requires {
                { ComponentTypeId<std::remove_cvref_t<T>>::value };
        }
        bool HasComponent(EntityId eid) const {
            using U = remove_cvref_t<T>;
            static_assert(
                requires { ComponentTypeId<U>::value; },
            "ComponentTypeId<T> is not registered. Use n_component::XXXComponent."
                );
            return HasComponent(eid, ComponentTypeId<U>::value);
        }
        //bool HasComponent(EntityId eid) const {
        //    return HasComponent(eid, ComponentTypeId<T>::value);
        //}

        // 追加と削除
        template<typename T>
        void AddComponent(EntityId eid, const T& comp);

        template<typename T>
        void RemoveComponent(EntityId eid);

        // get optional copy
        template<typename T>
        std::optional<T> GetComponent(EntityId eid) const;

        template<typename T>
        void SetComponent(EntityId eid, const T& comp);

        // エディターのユーティリティ: マスクを取得する
        ComponentMask GetMask(EntityId eid) const;

    private:
        // エンティティごとのマスク
        std::unordered_map<EntityId, ComponentMask> masks_;
        mutable std::mutex masksMutex_;

        // コンポーネントごとのストレージ
        std::unordered_map<EntityId, n_component::TransformComponent> transformStorage_;
        std::unordered_map<EntityId, n_component::MoveComponent> moveStorage_;
        std::unordered_map<EntityId, n_component::LightComponent> lightStorage_;
        std::unordered_map<EntityId, n_component::RigidbodyComponent> rigidbodyStorage_;
        std::unordered_map<EntityId, n_component::StartComponent> startStorage_;
        std::unordered_map<EntityId, n_component::FinishComponent> finishStorage_;
        std::unordered_map<EntityId, n_component::IsPlayerComponent> isPlayerStorage_;
        std::unordered_map<EntityId, n_component::SpriteComponent> spriteStorage_;

        mutable std::mutex storageMutex_; // ストレージを保護する

        template<typename T>
        std::unordered_map<EntityId, T>& getMapForType();

        template<typename T>
        const std::unordered_map<EntityId, T>& getMapForType() const;
    };

    extern ComponentManager g_componentManager;

    template<>
    inline std::unordered_map<EntityId, n_component::TransformComponent>& ComponentManager::getMapForType<n_component::TransformComponent>() {
        return transformStorage_;
    }
    template<>
    inline std::unordered_map<EntityId, n_component::MoveComponent>& ComponentManager::getMapForType<n_component::MoveComponent>() {
        return moveStorage_;
    }
    template<>
    inline std::unordered_map<EntityId, n_component::LightComponent>& ComponentManager::getMapForType<n_component::LightComponent>() {
        return lightStorage_;
    }
    template<>
    inline std::unordered_map<EntityId, n_component::RigidbodyComponent>& ComponentManager::getMapForType<n_component::RigidbodyComponent>() {
        return rigidbodyStorage_;
    }
    template<>
    inline std::unordered_map<EntityId, n_component::StartComponent>& ComponentManager::getMapForType<n_component::StartComponent>() {
        return startStorage_;
    }
    template<>
    inline std::unordered_map<EntityId, n_component::FinishComponent>& ComponentManager::getMapForType<n_component::FinishComponent>() {
        return finishStorage_;
    }
    template<>
    inline std::unordered_map<EntityId, n_component::IsPlayerComponent>& ComponentManager::getMapForType<n_component::IsPlayerComponent>() {
        return isPlayerStorage_;
    }
    template<>
    inline std::unordered_map<EntityId, n_component::SpriteComponent>& ComponentManager::getMapForType<n_component::SpriteComponent>() {
        return spriteStorage_;
    }

    // const の特殊化
    template<>
    inline const std::unordered_map<EntityId, n_component::TransformComponent>& ComponentManager::getMapForType<n_component::TransformComponent>() const {
        return transformStorage_;
    }
    template<>
    inline const std::unordered_map<EntityId, n_component::MoveComponent>& ComponentManager::getMapForType<n_component::MoveComponent>() const {
        return moveStorage_;
    }
    template<>
    inline const std::unordered_map<EntityId, n_component::LightComponent>& ComponentManager::getMapForType<n_component::LightComponent>() const {
        return lightStorage_;
    }
    template<>
    inline const std::unordered_map<EntityId, n_component::RigidbodyComponent>& ComponentManager::getMapForType<n_component::RigidbodyComponent>() const {
        return rigidbodyStorage_;
    }
    template<>
    inline const std::unordered_map<EntityId, n_component::StartComponent>& ComponentManager::getMapForType<n_component::StartComponent>() const {
        return startStorage_;
    }
    template<>
    inline const std::unordered_map<EntityId, n_component::FinishComponent>& ComponentManager::getMapForType<n_component::FinishComponent>() const {
        return finishStorage_;
    }
    template<>
    inline const std::unordered_map<EntityId, n_component::IsPlayerComponent>& ComponentManager::getMapForType<n_component::IsPlayerComponent>() const {
        return isPlayerStorage_;
    }
    template<>
    inline const std::unordered_map<EntityId, n_component::SpriteComponent>& ComponentManager::getMapForType<n_component::SpriteComponent>() const {
        return spriteStorage_;
    }

    // --- テンプレート本体実装（Add/Remove/Get/Set） ---
    template<typename T>
    inline void ComponentManager::AddComponent(EntityId eid, const T& comp) {
        using U = remove_cvref_t<T>;
        std::lock_guard<std::mutex> lk(storageMutex_);
        getMapForType<U>()[eid] = comp;
        std::lock_guard<std::mutex> lk2(masksMutex_);
        using Under = std::underlying_type_t<ComponentId>;
        masks_[eid].set(static_cast<size_t>(static_cast<Under>(ComponentTypeId<U>::value)));
    }

    template<typename T>
    inline void ComponentManager::RemoveComponent(EntityId eid) {
        using U = remove_cvref_t<T>;
        std::lock_guard<std::mutex> lk(storageMutex_);
        getMapForType<U>().erase(eid);
        std::lock_guard<std::mutex> lk2(masksMutex_);
        using Under = std::underlying_type_t<ComponentId>;
        masks_[eid].reset(static_cast<size_t>(static_cast<Under>(ComponentTypeId<U>::value)));
    }

    template<typename T>
    inline std::optional<T> ComponentManager::GetComponent(EntityId eid) const {
        using U = remove_cvref_t<T>;
        std::lock_guard<std::mutex> lk(storageMutex_);
        const auto& m = getMapForType<U>();
        auto it = m.find(eid);
        if (it == m.end()) return std::nullopt;
        return it->second;
    }

    template<typename T>
    inline void ComponentManager::SetComponent(EntityId eid, const T& comp) {
        using U = remove_cvref_t<T>;
        std::lock_guard<std::mutex> lk(storageMutex_);
        getMapForType<U>()[eid] = comp;
        std::lock_guard<std::mutex> lk2(masksMutex_);
        using Under = std::underlying_type_t<ComponentId>;
        masks_[eid].set(static_cast<size_t>(static_cast<Under>(ComponentTypeId<U>::value)));
    }
}

#endif // COMPONENT_MANAGER_HPP