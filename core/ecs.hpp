#ifndef NEW_ECS_H
#define NEW_ECS_H

#include <bitset>
#include <vector>
#include <queue>
#include <unordered_map>
#include <assert.h>

namespace ecs {

using EntityID = unsigned long long;
using ComponentID = uint32_t;

class BaseComponentPool {
public:
    BaseComponentPool(size_t componentSize, uint32_t maxEntites) : m_componentSize(componentSize) {
        p_data = new char[componentSize * maxEntites];
    }

    inline void *get(size_t index) {
        return p_data + index * m_componentSize;
    }
    virtual void destroy(size_t index) = 0;

private: 
    char *p_data{nullptr};
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
        return new (get(index)) T(std::forward<Args>(args)...);
    }

    inline void destroy(size_t index) override {
        reinterpret_cast<T*>(get(index))->~T();
    }
};

template <int COMPONENTS = 32>
class Scene {
    using ComponentMask = std::bitset<COMPONENTS>;

public:
    struct EntityDescription {
        EntityID id;
        ComponentMask mask;
        bool isValid;
    };  

    Scene(uint32_t maxEntites = 100000) : m_maxEntites(maxEntites), null(maxEntites + 1) {
        m_entities.reserve(m_maxEntites);
        m_componentPools.resize(COMPONENTS, nullptr);
        for (int i = 0; i < m_maxEntites; i++) {
            m_availableIDs.push(static_cast<EntityID>(i));
        }
    }

    ~Scene() {
        for (auto ent : view()) {
            destroyEntity(ent);
        }
    }

    template <typename T>
    ComponentID getComponentID() {
        static ComponentID s_componentID = s_componentCounter++;
        assert(s_componentID < COMPONENTS);
        return s_componentID;
    }

    EntityID createEntity() {
        EntityID ent = m_availableIDs.front();
        m_availableIDs.pop();
        if (m_entities.size() <= ent) {
            m_entities.push_back({ent, ComponentMask{}, true});
        } else {
            assert(!m_entities[ent].isValid && "EntityID should not have been in the available entities queue");
            m_entities[ent].isValid = true;
        }   
        return ent;
    }

    void destroyEntity(EntityID ent) {
        assert(ent != null && "Cant destroy null entity!");
        assert(m_entities[ent].isValid && "Cant destroy already deleted entity!");
        for (int componentID = 0; componentID < COMPONENTS; componentID++) {
            if (m_entities[ent].mask.test(componentID)) {
                if (m_componentPools[componentID]) {
                    m_componentPools[componentID]->destroy(ent);
                }
            }
        }
        m_entities[ent].isValid = false;
        m_entities[ent].mask.reset();
        m_availableIDs.push(ent);
    }

    template <typename... Ts, typename... Args>
    decltype(auto) assign(EntityID ent, Args&&... args) {
        static_assert(sizeof...(Ts) != 0);   // suggested by zilverblade
        if constexpr(sizeof...(Ts) == 1) {
            return (_assign<Ts...>(ent, std::forward<Args>(args)...));
        } else {
            return std::forward_as_tuple<Ts&...>(_assign<Ts>(ent)...);
        }
    }

    template <typename... Ts>
    decltype(auto) get(EntityID ent) {
        static_assert(sizeof...(Ts) != 0);
        if constexpr(sizeof...(Ts) == 1) {
            return (_get<Ts...>(ent));
        } else {
            return std::forward_as_tuple<Ts&...>(_get<Ts>(ent)...);
        }
    }    

    template <typename T>
    bool has(EntityID ent) {
        assert(ent != null && "Cant check components of null entity");
        assert(m_entities[ent].isValid && "Cant check component of deleted entity!");
        assert(ent < m_entities.size() && "Entity not in the entity list!");
        ComponentID componentID = getComponentID<T>();
        return m_entities[ent].mask.test(componentID);
    }

    template <typename... Ts>
    void remove(EntityID ent) {
        static_assert(sizeof...(Ts) != 0);
        _remove<Ts...>(ent);
    }

    bool isValid(EntityID ent) {
        assert(ent != null && "Entity cant be null");
        assert(ent < m_entities.size() && "Entity not in the entity list!");
        return m_entities[ent].isValid;
    }

    template <typename... Ts>
    struct View {
        View(Scene& scene) : scene(scene) {
            if constexpr(sizeof...(Ts) == 0) 
                all = true;
            else {
                ComponentID componentIDs[] = {scene.getComponentID<Ts>()...};
                for (int i = 0; i < sizeof...(Ts); i++) 
                    mask.set(componentIDs[i]);
            }
        }
        struct Iterator {
            Iterator(Scene& scene, EntityID index, ComponentMask mask, bool all) 
              : scene(scene), index(index), mask(mask), all(all) {}
            bool isValidIndex() {
                return scene.isValid(scene.m_entities[index].id) && (all || mask == (mask & scene.m_entities[index].mask));    
            }
            decltype(auto) operator*() const {
                if constexpr(sizeof...(Ts) != 0) {
                    return std::forward_as_tuple<EntityID&, Ts&...>(scene.m_entities[index].id, scene.get<Ts>(scene.m_entities[index].id)...);
                } else {
                    return (scene.m_entities[index].id);
                }
            }
            bool operator==(const Iterator& other) const {
                return index == other.index || index == scene.m_entities.size();
            }
            bool operator!=(const Iterator& other) const {
                return index != other.index && index != scene.m_entities.size();
            }
            decltype(auto) operator++() {
                do {
                    index++;
                } while (index < scene.m_entities.size() && !isValidIndex());
                return *this;
            }
            Scene& scene;
            EntityID index;
            ComponentMask mask;
            bool all;
        };
        const Iterator begin() const {
            EntityID index = 0;
            while (index < scene.m_entities.size() && (mask != (mask & scene.m_entities[index].mask) || !scene.isValid(scene.m_entities[index].id))) {
                index ++;
            }   
            return Iterator{scene, index, mask, all};            
        }
        const Iterator end() const {
            return Iterator{scene, EntityID{scene.m_entities.size()}, mask, all};
        }
        Scene& scene;
        bool all = false;
        ComponentMask mask;
    };

    template <typename... Ts>
    View<Ts...> view() {
        return View<Ts...>(*this);
    }

private:
    template <typename T, typename... Args>
    T& _assign(EntityID ent, Args&&... args) {
        assert(ent != null && "Cant assign to null entity!");
        assert(m_entities[ent].isValid && "Cant assign to a deleted entity!");
        assert(ent < m_entities.size() && "Entity not in the entity list!");
        ComponentID componentID = getComponentID<T>();
        assert(componentID < COMPONENTS && "Too many components");
        assert(!m_entities[ent].mask.test(componentID) && "Entity already has Component");
        if (!m_componentPools[componentID]) 
            m_componentPools[componentID] = new ComponentPool<T>(m_maxEntites);
        T* p_component = reinterpret_cast<ComponentPool<T> *>(m_componentPools[componentID])->construct(ent, std::forward<Args>(args)...);    
        m_entities[ent].mask.set(componentID);
        return *p_component;
    }

    template <typename T>
    T& _get(EntityID ent) {
        assert(ent != null && "Cant get from null entity!");
        assert(m_entities[ent].isValid && "Cant get from a deleted entity!");
        assert(ent < m_entities.size() && "Entity not in the entity list!");
        ComponentID componentID = getComponentID<T>();
        assert(componentID < COMPONENTS && "Too many components");
        assert(m_entities[ent].mask.test(componentID) && "Entity does not have Component");
        return *static_cast<T *>(m_componentPools[componentID]->get(ent));
    }

    template <typename T, typename... O>
    void _remove(EntityID ent) {
        assert(ent != null && "Cant remove from null entity!");
        assert(m_entities[ent].isValid && "Cant remove from a deleted entity!");
        assert(ent < m_entities.size() && "Entity not in the entity list!");
        ComponentID componentID = getComponentID<T>();
        assert(componentID < COMPONENTS && "Too many components");
        assert(m_entities[ent].mask.test(componentID) && "Entity does not have Component");
        m_componentPools[componentID]->destroy(ent);
        if constexpr(sizeof...(O) == 0) return;
        _remove<O...>(ent);
    }

public:
    const EntityID                   null;
    std::vector<EntityDescription>   m_entities;

private:
    const uint32_t                   m_maxEntites;
    std::queue<EntityID>             m_availableIDs;
    std::vector<BaseComponentPool *> m_componentPools;
    ComponentID                      s_componentCounter = 0;
};

} // namespace ecs

#endif