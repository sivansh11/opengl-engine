#include "timer.hpp"

#include <cassert>

namespace gfx {

AsyncTimerQuery::AsyncTimerQuery(const uint32_t capacity) : m_capacity(capacity) {
    assert(m_capacity > 0);
    m_queries = new uint32_t[m_capacity * 2];
    glGenQueries(m_capacity * 2, m_queries);
}

AsyncTimerQuery::~AsyncTimerQuery() {
    glDeleteQueries(m_capacity * 2, m_queries);
    delete[] m_queries; 
}

void AsyncTimerQuery::begin() {
    if (m_count < m_capacity) 
        glQueryCounter(m_queries[m_start], GL_TIMESTAMP);
    else {
        // throw std::runtime_error("no more queries left, make a timer with a bigger capacity!");
    }
}

void AsyncTimerQuery::end() {
    if (m_count < m_capacity) {
        glQueryCounter(m_queries[m_start + m_capacity], GL_TIMESTAMP);
        m_start = (m_start + 1) % m_capacity;
        m_count++;
    } else {
        // throw std::runtime_error("no more queries left, make a timer with a bigger capacity!");
    }
}

std::optional<uint64_t> AsyncTimerQuery::popTimeStamp() {
    if (m_count == 0)
        return std::nullopt;
    
    uint32_t index = (m_start + m_capacity - m_count) % m_capacity;

    GLint startResultAvailable;
    GLint endResultAvailable;

    glGetQueryObjectiv(m_queries[index], GL_QUERY_RESULT_AVAILABLE, &startResultAvailable);
    glGetQueryObjectiv(m_queries[index + m_capacity], GL_QUERY_RESULT_AVAILABLE, &endResultAvailable);

    if (startResultAvailable == GL_FALSE || endResultAvailable == GL_FALSE) {
        return std::nullopt;
    }

    m_count--;

    uint64_t startTimeStamp;
    uint64_t endTimeStamp;
    glGetQueryObjectui64v(m_queries[index], GL_QUERY_RESULT, &startTimeStamp);
    glGetQueryObjectui64v(m_queries[index + m_capacity], GL_QUERY_RESULT, &endTimeStamp);
    return endTimeStamp - startTimeStamp;
}

} // namespace gfx
