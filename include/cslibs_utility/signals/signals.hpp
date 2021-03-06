#ifndef CSLIBS_UTILITY_SIGNALS_HPP
#define CSLIBS_UTILITY_SIGNALS_HPP

#include <mutex>
#include <memory>
#include <atomic>
#include <unordered_map>

namespace cslibs_utility {
namespace signals {
template<typename Slot>
class Signal;

template<typename Slot>
class Signal : public std::enable_shared_from_this<Signal<Slot>> {
public:
    typedef std::shared_ptr<Signal<Slot>> Ptr;

    class Connection {
    public:
        typedef std::shared_ptr<Connection> Ptr;

        Connection(Signal<Slot> &_signal) :
            signal(_signal)
        {
        }

        virtual ~Connection()
        {
            signal.disconnect(this);
        }

    private:
        Signal &signal;
    };

    Signal() :
        enabled(false)
    {
    }

    void enable()
    {
        enabled = true;
    }

    void disable()
    {
        enabled = false;
    }

    bool isEnabled() const
    {
        return enabled;
    }

    template<typename Function>
    typename Connection::Ptr connect(Function&& _f)
    {
        typename Connection::Ptr c(new Signal::Connection(*this));
        connections[c.get()] = std::move(std::forward<Function>(_f));
        return c;
    }

    void disconnect(typename Connection::Ptr &_c)
    {
        std::unique_lock<std::mutex> lock(mutex);
        connections.erase(_c.get());
    }

    void disconnect(Connection *_c)
    {
        std::unique_lock<std::mutex> lock(mutex);
        connections.erase(_c);
    }

    template<typename... Args>
    void operator ()(Args&&... args)
    {
        if(!enabled)
            return;

        std::unique_lock<std::mutex> lock(mutex);
        for(auto &c : connections) {
            c.second(std::move(args...));
        }
    }

    template<typename... Args>
    void operator ()(Args&... args)
    {
        if(!enabled)
            return;

        std::unique_lock<std::mutex> lock(mutex);
        for(auto &c : connections) {
            c.second(args...);
        }
    }

private:
    std::mutex                              mutex;
    std::unordered_map<Connection*, Slot>   connections;
    std::atomic_bool                        enabled;

};
}
}

#endif /* CSLIBS_UTILITY_SIGNALS_HPP */
