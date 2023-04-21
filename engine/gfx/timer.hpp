#ifndef GFX_TIMER_HPP
#define GFX_TIMER_HPP

#include <iostream>

#include <glad/glad.h>

#include <optional>

namespace gfx {

class AsyncTimerQuery {
public:
    AsyncTimerQuery(const uint32_t capacity);
    ~AsyncTimerQuery();

    void begin();
    void end();

    std::optional<uint64_t> popTimeStamp();

private:
    uint32_t m_capacity;
    uint32_t *m_queries;
    uint32_t m_start{0};
    uint32_t m_count{0};
};

} // namespace gfx

#endif