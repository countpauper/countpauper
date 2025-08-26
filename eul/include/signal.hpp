#pragma once 

#include "intrusive_list.hpp"
#include <functional>

namespace eul
{

template<typename ...Args> 
class signal;

template<typename ...Args>
class slot : protected intrusive_list::node
{
public:
    using callback = std::function<void(Args...)>;
    using signalType = signal<Args...>;

    slot() = default;
    slot(signalType& signal, const callback& cb) :
        slot(signal.connect(cb))
    {
    }
    inline void disconnect() 
    {
        if (queued)
            disconnected = true;
        else
        {
            disconnected = false;
            unlink();
        }
        queued = false;
    }
    inline bool connected() const 
    {
        return !disconnected && linked();
    }
private:
    slot(intrusive_list& list, callback cb) :
        node(list),
        cb(cb)
    {
    }
    void queue()
    {
        queued = true;
    }
    void dequeue()
    {
        queued = false;
        if (disconnected)
        {
            disconnect();
        }
    }
    friend class signal<Args...>;
    callback cb;
    bool queued:1 = false; 
    bool disconnected:1 = false;
};

template<typename ...Args>
class signal
{
public:
    signal() = default;
    using SlotType = slot<Args...>;
    using callback = SlotType::callback;
    SlotType connect(callback cb)
    {
        if (cb)
            return SlotType(_slots, cb);
        else
            return SlotType();
    }
    void disconnect() 
    {
        _slots.clear();
    }
    
    template<typename... Pargs>
    void operator()(Pargs&&... args) const
    {
        for(intrusive_list::node& _node: _slots) 
        {
            static_cast<SlotType&>(_node).queue();
        }
        for(const intrusive_list::node& _node: _slots) 
        {
            const SlotType& slot = static_cast<const SlotType&>(_node);
            if (slot.queued)
                static_cast<const SlotType&>(slot).cb(std::forward<Pargs>(args)...);
        }
        for(intrusive_list::node& _node: _slots) 
        {
            static_cast<SlotType&>(_node).dequeue();
        }
    }
    std::size_t connections() const { return _slots.size(); }
private:
    intrusive_list _slots;
};

}