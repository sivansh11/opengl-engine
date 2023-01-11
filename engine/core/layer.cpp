#include "layer.hpp"

namespace core {

Layer::Layer(const std::string& name) : m_name(name) {

}

const std::string& Layer::getName() {
    return m_name;
}

} // namespace core
