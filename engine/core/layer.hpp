#ifndef LAYER_HPP
#define LAYER_HPP

#include <string_view>
#include <string>

namespace core {

class Layer {
public:
    Layer(std::string_view name) : m_name(name) {}
    virtual ~Layer() = default; 

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(float dt) {}
    virtual void onUIRender() {}

    const std::string& name() const { return m_name; }

private:
    std::string m_name;
};

} // namespace core

#endif