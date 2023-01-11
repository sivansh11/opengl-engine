#ifndef LAYER_HPP
#define LAYER_HPP

#include <string>

namespace core {

class Layer {
public:
    Layer(const std::string& name);
    virtual ~Layer() = default; 

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(float dt) {}
    virtual void onImGuiRender() {}

    const std::string& getName();

private:
    std::string m_name;
};

} // namespace core

#endif