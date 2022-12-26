#ifndef EVENT_HPP
#define EVENT_HPP

#include <queue>
#include <functional>
#include <unordered_map>
#include <vector>

namespace event {

using EventID = unsigned int;
using SubscriberID = long long;

struct Event {};

class Dispatcher {
public:
    Dispatcher() = default;
    ~Dispatcher() {}

    template <typename EventType>
    void subscribe(std::function<void(const Event&)> &&handler) {
        EventID eventID = getEventID<EventType>();
        m_eventToCallbacks[eventID].push_back(handler);
    }

    template <typename T, typename... Args>
    void post(Args&&... args) {
        T e(std::forward<Args>(args)...);
        EventID eventID = getEventID<T>();
        auto eventCallbackSearch = m_eventToCallbacks.find(eventID);
        if (eventCallbackSearch == m_eventToCallbacks.end()) return;
        for (auto& callback : eventCallbackSearch->second) {
            callback(static_cast<Event&>(e));
        }
    }

private:
    template <typename EventType>
    EventID getEventID() {
        static EventID s_eventID = m_eventIDCounter++;
        return s_eventID;
    }

private:
    EventID m_eventIDCounter = 0;
    std::unordered_map<EventID, std::vector<std::function<void(const Event&)>>> m_eventToCallbacks;
};

} // namespace event

#endif