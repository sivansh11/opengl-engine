#ifndef ECS_HPP
#define ECS_HPP

#include <bitset>
#include <vector>
#include <queue>
#include <unordered_map>
#include <assert.h>

namespace ecs {

using EntityID = unsigned long long;
using ComponentID = uint32_t;

namespace utils {

static ComponentID s_componentCounter = 0;
    
} // namespace utils

const EntityID null = 0;

class BaseComponentPool {
public:
    BaseComponentPool(size_t componentSize, uint32_t maxEntity) : m_componentSize(componentSize) {
        p_data = new char[componentSize * maxEntity];
    }

    inline void *get(size_t index) {
        return p_data + index * m_componentSize;
    }

    virtual void destroy(size_t index) = 0;

private:
    char  *p_data{nullptr};
    size_t m_componentSize{0};
};

template <typename T>
class ComponentPool : public BaseComponentPool {
public:
    ComponentPool(uint32_t maxEntities) : BaseComponentPool(sizeof(T), maxEntities) {}
    ~ComponentPool() {
        delete[] p_data;
    }

    template <typename... Args>
    inline T* construct(size_t index, Args&&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return new (get(index)) T();
        } else {
            return new (get(index)) T{std::forward<Args>(args)...};
        }
    }

    inline void destroy(size_t index) override {
        reinterpret_cast<T*>(get(index))->~T();
    }
private:

};

template <int COMPONENTS = 32>
class Scene {
    using ComponentMask = std::bitset<COMPONENTS>;
    friend class Group;

public:
    struct EntityDescription {
        EntityID      id;
        ComponentMask mask;
        bool          isValid;
    };

    Scene(uint32_t maxEntities = 100000) : m_maxEntities(maxEntities) {
        m_entities.reserve(m_maxEntities);
        m_componentPools.resize(COMPONENTS, nullptr);
        for (int i = 1; i < m_maxEntities; i++) {
            m_availableEntities.emplace(static_cast<EntityID>(i));
        }
    }
    ~Scene() {
        for (auto ent : group()) {
            destroyEntity(ent);
        }
        for (int i = 0; i < COMPONENTS; i++) {
            if (m_componentPools[i])  {
                delete m_componentPools[i];
                m_componentPools[i] = nullptr;
            }
        }
    }

    EntityID createEntity() {
        assert(m_availableEntities.size() > 0 && "Entity limit reached");
        EntityID ent = m_availableEntities.front();
        m_availableEntities.pop();
        if (m_entities.size() <= ent) {
            m_entities.push_back({ent, ComponentMask{}, true});
        } else {
            assert(!m_entities[index(ent)].isValid);
        }
        return ent;
    }

    void destroyEntity(EntityID ent) {
        assert(ent != null && "Cant destroy null entity!");
        assert(m_entities[index(ent)].isValid && "Cant destroy already deleted entity!");
        for (int componentID = 0; componentID < COMPONENTS; componentID++) {
            if (m_entities[index(ent)].mask.test(componentID)) {
                assert(m_componentPools[componentID]);
                m_componentPools[componentID]->destroy(index(ent));
            }
        }
        m_entities[index(ent)].isValid = false;
        m_entities[index(ent)].mask.reset();
        m_availableEntities.push(ent);
    }

    template <typename... Ts, typename... Args> 
    decltype(auto) assign(EntityID ent, Args&&... args) {
        static_assert(sizeof...(Ts) != 0 && "Components Not Specified");
        if constexpr(sizeof...(Ts) == 1) {
            return (_assign<Ts...>(ent, std::forward<Args>(args)...));
        } else {
            return std::forward_as_tuple<Ts&...>(_assign<Ts>(ent)...);
        }
    }

    template <typename... Ts>
    decltype(auto) get(EntityID ent) {
        static_assert(sizeof...(Ts) != 0 && "Components Not Specified");
        if constexpr(sizeof...(Ts) == 1) {
            return (_get<Ts...>(ent));
        } else {
            return std::forward_as_tuple<Ts&...>(_get<Ts>(ent)...);
        }
    }

    template <typename... Ts>
    void remove(EntityID ent) {
        static_assert(sizeof...(Ts) != 0 && "Components Not Specified");
        _remove<Ts...>(ent);
    }

    template <typename T>
    bool has(EntityID ent) {
        assert(ent != null && "Cant check for component in null entity!");
        assert(m_entities[index(ent)].isValid && "Cant check for component in a deleted entity!");
        assert(index(ent) < m_entities.size() && "Entity not in the entity list!");
        ComponentID componentID = getComponentID<T>();
        return m_entities[index(ent)].mask.test(componentID);
    }

    template <typename T>
    ComponentID getComponentID() {
        static ComponentID s_componentID = utils::s_componentCounter++;
        assert(s_componentID < COMPONENTS && "Too many components!");
        return s_componentID;
    }

    bool isValid(EntityID ent) {
        return m_entities[index(ent)].isValid;
    }

    template <typename... Ts>
    struct Group {
        Group(Scene& scene) : scene(scene) {
            if constexpr(sizeof...(Ts) == 0)
                all = true;
            else {
                ComponentID componentIDs[] = {scene.getComponentID<Ts>()...};
                for (int i = 0; i < sizeof...(Ts); i++) 
                    mask.set(componentIDs[i]);
            }
        }

        struct iter {
            iter(Scene& scene, EntityID index, ComponentMask mask, bool all) 
              : scene(scene), index(index), all(all), mask(mask)  {}
            bool isValidIndex() {
                return scene.isValid(index) && (all || mask == (mask & scene.m_entities[scene.index(index)].mask));
            }

            decltype(auto) operator *() {
                if constexpr(sizeof...(Ts) != 0) {
                    return std::forward_as_tuple<EntityID&, Ts&...>(index, scene.get<Ts>(index)...);
                } else {
                    return (index);
                }
            }

            bool operator==(const iter& other) const {
                return index == other.index || scene.index(index) == scene.m_entities.size();
            }
            bool operator!=(const iter& other) const {
                return index != other.index && scene.index(index) != scene.m_entities.size();
            }

            decltype(auto) operator++() {
                do {
                    index++;
                } while (scene.index(index) < scene.m_entities.size() && !isValidIndex());
                return *this;
            }
            
            Scene&        scene;
            EntityID      index;
            bool          all;
            ComponentMask mask;
        };

        const iter begin() const {
            EntityID index = 1;
            while (scene.index(index) < scene.m_entities.size() && (mask != (mask & scene.m_entities[scene.index(index)].mask) || !scene.isValid(index))) {
                index++;
            }
            return iter(scene, index, mask, all);
        }

        const iter end() const {
            return iter(scene, EntityID(scene.m_entities.size() + 1), mask, all);
        }

        Scene&        scene;
        bool          all = false;
        ComponentMask mask;
    };
    
    template <typename... Ts>
    Group<Ts...> group() {
        return Group<Ts...>(*this);
    }

private:
    template <typename T, typename... Args>
    T& _assign(EntityID ent, Args&&... args) {
        assert(ent != null && "Cant assign to null entity!");
        assert(m_entities[index(ent)].isValid && "Cant assign to deleted entity!");
        assert(index(ent) < m_entities.size() && "Entity not in the entity list!");
        ComponentID componentID = getComponentID<T>();
        assert(!m_entities[index(ent)].mask.test(componentID) && "Entity already has component!");
        if (!m_componentPools[componentID]) 
            m_componentPools[componentID] = new ComponentPool<T>(m_maxEntities);
        EntityID entID = index(ent);
        T* p_component = reinterpret_cast<ComponentPool<T> *>(m_componentPools[componentID])->construct(entID, std::forward<Args>(args)...);
        m_entities[entID].mask.set(componentID);
        return *p_component;
    }

    template <typename T>
    T& _get(EntityID ent) {
        assert(ent != null && "Cant get from null entity!");
        assert(m_entities[index(ent)].isValid && "Cant get from deleted entity!");
        assert(index(ent) < m_entities.size() && "Entity not in the entity list!");
        ComponentID componentID = getComponentID<T>();
        EntityID entID = index(ent);
        assert(m_entities[index(ent)].mask.test(componentID) && "Entity does not have Component!");
        return *static_cast<T *>(m_componentPools[componentID]->get(entID));
    }

    template <typename T, typename... O>
    void _remove(EntityID ent) {
        assert(ent != null && "Cant remove from null entity!");
        assert(m_entities[index(ent)].isValid && "Cant remove from a deleted entity!");
        assert(index(ent) < m_entities.size() && "Entity not in entity list!");
        ComponentID componentID = getComponentID<T>();
        assert(m_entities[index(ent)].mask.test(componentID) && "Entity does not have component!");
        EntityID entID = index(ent);
        m_componentPools[componentID]->destroy(entID);
        m_entities[entID].mask.reset(componentID);
        if constexpr(sizeof...(O) == 0) return;
        else
        _remove<O...>(ent);
    }

    inline EntityID index(EntityID ent) {
        return ent - 1;
    }

private:
    const uint32_t                   m_maxEntities;
    std::vector<EntityDescription>   m_entities;
    std::queue<EntityID>             m_availableEntities;
    std::vector<BaseComponentPool *> m_componentPools;       
};

} // namespace ecs

#endif