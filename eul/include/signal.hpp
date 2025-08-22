#pragma once 

#include "intrusive_list.hpp"
#include <functional>

namespace eul
{

template<typename ...Args> 
class Signal;

template<typename ...Args>
class Slot : protected LinkedList::Node
{
public:
    using Callback = std::function<void(Args...)>;
    using SignalType = Signal<Args...>;

    Slot() = default;
    Slot(SignalType& signal, const Callback& cb) :
        Slot(signal.Connect(cb))
    {
    }
    inline void Disconnect() 
    {
        if (queued)
            disconnected = true;
        else
        {
            disconnected = false;
            Unlink();
        }
        queued = false;
    }
    inline bool IsConnected() const 
    {
        return !disconnected && IsLinked();
    }
private:
    Slot(LinkedList& list, Callback cb) :
        Node(list),
        callback(cb)
    {
    }
    void Queue()
    {
        queued = true;
    }
    void Dequeue()
    {
        queued = false;
        if (disconnected)
        {
            Disconnect();
        }
    }
    friend class Signal<Args...>;
    Callback callback;
    bool queued:1 = false; 
    bool disconnected:1 = false;
};

template<typename ...Args>
class Signal
{
public:
    Signal() = default;
    using SlotType = Slot<Args...>;
    using Callback = SlotType::Callback;
    SlotType Connect(Callback cb)
    {
        return SlotType(slots, cb);
    }
    void DisconnectAll() 
    {
        slots.clear();
    }
    
    template<typename... Pargs>
    void operator()(Pargs&&... args) const
    {
        for(LinkedList::Node& node: slots) 
        {
            static_cast<SlotType&>(node).Queue();
        }
        for(const LinkedList::Node& node: slots) 
        {
            const SlotType& slot = static_cast<const SlotType&>(node);
            if (slot.queued)
                static_cast<const SlotType&>(slot).callback(std::forward<Pargs>(args)...);
        }
        for(LinkedList::Node& node: slots) 
        {
            static_cast<SlotType&>(node).Dequeue();
        }
    }
    std::size_t connections() const { return slots.size(); }
private:
    LinkedList slots;
};
}