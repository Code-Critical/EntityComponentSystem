#ifndef ECS_H
#define ECS_H

#include <memory>
#include <vector>
#include <bitset>

/*
    Cute little ECS implementation, for shits and giggles.
*/
namespace ecs {
    constexpr uint32_t MAX_ENTITIES = 100;
    constexpr uint32_t MAX_COMPONENTS = 32;

    using entityHandle = uint32_t;
    using componentHandle = uint32_t;
    using componentBitmask = std::bitset<MAX_COMPONENTS>;
    
    template<typename T, int v> struct tagged_type {
        static constexpr int tag = v;
        using type = T;
    };

    static uint32_t entityCount = 0;
    static std::vector<ecs::entityHandle> recycling;

    static ecs::componentBitmask entities[MAX_ENTITIES];
    static void* components[MAX_COMPONENTS] = { nullptr };

    bool registerEntity(ecs::entityHandle& entity);
    bool unregisterEntity(ecs::entityHandle entity);

    template<typename T> bool registerComponent();
    template<typename T> bool unregisterComponent();

    template<typename T> bool enableComponent(ecs::entityHandle entity);
    template<typename T> bool disableComponent(ecs::entityHandle entity);

    template<typename T> bool setComponent(ecs::entityHandle entity, typename T::type value);
    template<typename T> bool getComponent(ecs::entityHandle entity, typename T::type& value);

    template<typename T> struct componentIterator;
}

bool ecs::registerEntity(ecs::entityHandle& entity) {
    if (entityCount >= MAX_ENTITIES) {
        return false;
    }
    
    if (recycling.size() > 0) {
        entity = recycling.back();
        recycling.pop_back();
    } else {
        entity = entityCount;
    }
    
    entityCount++;

    return true;
}

bool ecs::unregisterEntity(ecs::entityHandle entity) {
    if (entity >= MAX_ENTITIES) {
        return false;
    }
    
    recycling.push_back(entity);
    entities[entity].reset();
    entityCount--;

    return true;
}

template<typename T> bool ecs::registerComponent() {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if (components[T::tag] != nullptr) {
        return false;
    }

    components[T::tag] = new C[MAX_ENTITIES];    

    return true;
}

template<typename T> bool ecs::unregisterComponent() {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if (components[T::tag] == nullptr) {
        return false;
    }

    delete[] static_cast<C*>(components[T::tag]);  
    components[T::tag] = nullptr;  

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

    static_cast<C*>(components[T::tag])[entity] = value;

    return true;
}

template<typename T> bool ecs::getComponent(ecs::entityHandle entity, typename T::type& value) {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    if ((entities[entity][T::tag] == false) || (components[T::tag] == nullptr)) {
        return false;
    }

    value = static_cast<C*>(components[T::tag])[entity];

    return true;   
}

template<typename T> struct ecs::componentIterator {
    static_assert((T::tag >= 0) && (T::tag < MAX_COMPONENTS));
    using C = typename T::type;

    struct iterator {
        int location = 0;

        iterator(int offset) {
            location = offset;
        }

        C& operator*() const {
            C* base = static_cast<C*>(components[T::tag]);
            return base[location];
        }
        
        bool operator==(const iterator& operand) const {
            return (location == operand.location);
        }

        bool operator!=(const iterator& operand) const {
            return (location != operand.location);
        }

        iterator& operator++() {
            bool isBounded;
            bool isEnabled;

            do {
                location++;
                isBounded = (0 <= location) && (location < entityCount + recycling.size());
                isEnabled = entities[location][T::tag];
            } while(!isEnabled && isBounded);

            return *this;
        }

        iterator& operator--() {
            bool isBounded;
            bool isEnabled;

            do {
                location--;
                isBounded = (0 <= location) && (location < entityCount + recycling.size());
                isEnabled = entities[location][T::tag];
            } while(!isEnabled && isBounded);

            return *this;
        }
    };

    const iterator begin() const {
        bool isEnabled = entities[0][T::tag];

        if (isEnabled) {
            return iterator(0);
        } else {
            return ++iterator(0);
        }
    }

    const iterator end() const {
        return iterator(entityCount + recycling.size());
    }
};

#endif
