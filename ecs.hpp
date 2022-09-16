#ifndef ECS_H
#define ECS_H

#include <memory>
#include <vector>
#include <bitset>

namespace ecs {
    constexpr uint32_t MAX_ENTITIES = 1000000;
    constexpr uint32_t MAX_COMPONENTS = 32;

    using entityHandle = uint32_t;
    using componentBitmask = std::bitset<MAX_COMPONENTS>;
    
    template<typename T, int v> struct tagged_type {
        static constexpr int tag = v;
        using type = T;
    };
    
    static std::vector<ecs::componentBitmask> entities;
    static std::shared_ptr<void> components[MAX_COMPONENTS];
    static std::vector<ecs::entityHandle> recycling;
    
    bool registerEntity(ecs::entityHandle& entity);
    bool unregisterEntity(ecs::entityHandle entity);

    template<typename T> bool registerComponent();
    template<typename T> bool unregisterComponent();

    template<typename T> bool enableComponent(ecs::entityHandle entity);
    template<typename T> bool disableComponent(ecs::entityHandle entity);

    template<typename T> bool setComponent(ecs::entityHandle entity, typename T::type value);
    template<typename T> bool getComponent(ecs::entityHandle entity, typename T::type& value);

    template<typename T> bool getIterator(std::vector<typename T::type>*& iterator);
}

bool ecs::registerEntity(ecs::entityHandle& entity) {
    if (recycling.size() > 0) {
        entity = recycling.back();
        entities[entity].reset();
        recycling.pop_back();
    } else {
        entity = entities.size();
        entities.push_back(0);
    }

    return true;
}

bool ecs::unregisterEntity(ecs::entityHandle entity) {
    ecs::entityHandle last = entities.size() - 1;

    if (entity == last) {
        entities.pop_back();
    } else if (entity < last) {
        recycling.push_back(entity);
        entities[entity].reset();
    } else {
        return false;
    }

    return true;
}

template<typename T> bool ecs::registerComponent() {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if (components[T::tag] != nullptr) {
        return false;
    }

    components[T::tag] = std::make_shared<std::vector<C>>();

    std::vector<C>* ref = static_cast<std::vector<C>*>(components[T::tag].get());

    return true;
}

template<typename T> bool ecs::unregisterComponent() {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if (components[T::tag] == nullptr) {
        return false;
    }

    components[T::tag].reset();

    return true;
}

template<typename T> bool ecs::enableComponent(ecs::entityHandle entity) {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if (entities[entity][T::tag] == true) {
        return false;
    }

    entities[entity][T::tag] = true;

    return true;
}

template<typename T> bool ecs::disableComponent(ecs::entityHandle entity) {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if (entities[entity][T::tag] == false) {
        return false;
    }

    entities[entity][T::tag] = false;

    return true;
}

template<typename T> bool ecs::setComponent(ecs::entityHandle entity, typename T::type value) {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if ((entities[entity][T::tag] == false) || (components[T::tag] == nullptr)) {
        return false;
    }

    std::vector<C>* ref = static_cast<std::vector<C>*>(components[T::tag].get());

    if (entities.size() != ref->size()) {
        ref->resize(entities.size());
    }

    if (entity >= ref->size()) {
        return false;
    }

    (*ref)[entity] = value;

    return true;
}

template<typename T> bool ecs::getComponent(ecs::entityHandle entity, typename T::type& value) {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if ((entities[entity][T::tag] == false) || (components[T::tag] == nullptr)) {
        return false;
    }

    std::vector<C>* ref = static_cast<std::vector<C>*>(components[T::tag].get());

    if (entities.size() != ref->size()) {
        ref->resize(entities.size());
    }

    if (entity >= ref->size()) {
        return false;
    }

    value = (*ref)[entity];

    return true;   
}

template<typename T> bool ecs::getIterator(std::vector<typename T::type>*& iterator) {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));

    if (components[T::tag] == nullptr) {
        return false;
    }

    iterator = static_cast<std::vector<typename T::type>*>(ecs::components[T::tag].get());

    return true;
}

#endif
