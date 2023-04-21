#ifndef RENDERER_PIPELINE_HPP
#define RENDERER_PIPELINE_HPP

#include "../core/event.hpp"
#include "../core/panel.hpp"
#include "../gfx/timer.hpp"

#include <entt/entt.hpp>

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <ctti/type_id.hpp>

namespace renderer {

class any {
    typedef void(*destroy_t)(void *);
    typedef void *(*create_t)(const void *);
public:
    any() : m_value(nullptr), m_destroy([](void *){}), m_create([](const void *) -> void * { return nullptr; }) {}

    any(const any& other) 
      : m_create(other.m_create),
        m_destroy(other.m_destroy),
        m_typeId(other.m_typeId) {
        m_value = m_create(other.m_value);
    }

    any& operator=(const any& other) {
        m_create = other.m_create;
        m_destroy = other.m_destroy;
        m_typeId = other.m_typeId;
        m_value = m_create(other.m_value);
        return *this;
    }

    template <typename T>
    explicit any(const T& other) 
      : m_typeId(ctti::type_id<T>()),
        m_destroy([](void *value) {
            delete reinterpret_cast<T *>(value);
        }),
        m_create([](const void *other) -> void * {
            return new T(*reinterpret_cast<const T *>(other));
        }) {
            m_value = m_create(&other);
        }
    ~any() {
        m_destroy(m_value);
    }   

    template <typename T>
    T& operator=(const T& other) {
        ctti::type_id_t otherTypeId = ctti::type_id<T>();

        if (m_typeId == otherTypeId) {
            *reinterpret_cast<T *>(m_value) = other;
            return *reinterpret_cast<T *>(m_value);
        }

        m_destroy(m_value);
        m_create = [](const void *other) -> void * {
            return new T(*reinterpret_cast<const T *>(other));
        };
        m_value = m_create(&other);
        m_typeId = otherTypeId;
        m_destroy = [](void *value) {
            delete reinterpret_cast<T *>(value);
        };
        return *reinterpret_cast<T *>(m_value);
    }

    template <typename T>
    T& as() {
        if (!m_value)
            throw std::runtime_error("any has no data!");
        if (ctti::type_id<T>() != m_typeId) 
            throw std::runtime_error("any contains different type!\n\tcontains: " + m_typeId.name().cppstring() + "\n\ttrying to cast as: " + ctti::type_id<T>().name().cppstring());
        return *reinterpret_cast<T *>(m_value);
    }

private:
    void *          m_value;
    ctti::type_id_t m_typeId;
    create_t        m_create;
    destroy_t       m_destroy;
};

class RenderContext {
public:
    // inserts if key does not exist
    any& at(const std::string& key) {
        return m_data[key];
    }   

    bool contains(const std::string& key) {
        return m_data.contains(key);
    }   

private:    
    std::map<std::string, any> m_data;
};

class RenderPass;

class BasePipeline : public core::BasePanel {
public:
    BasePipeline(event::Dispatcher& dispatcher, const std::string& pipelineName) : m_dispatcher(dispatcher), BasePanel(pipelineName) {}
    virtual ~BasePipeline() {}

    void addRenderPass(std::shared_ptr<RenderPass> renderPass);
    void render(entt::registry& registry, RenderContext &renderContext);
    void UI() override;

    virtual void preRender(entt::registry& registry) = 0;
    virtual void postRender(entt::registry& registry) = 0;
    virtual void pipelineUI() {}

protected:
    event::Dispatcher& m_dispatcher;
    RenderContext *renderContext;
    gfx::AsyncTimerQuery timer{10};
    std::map<std::string, float> stats;

private:
    std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
};

} // namespace renderer

#endif