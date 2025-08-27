#pragma once 

#include "intrusive_list.hpp"
#include <functional>

namespace eul
{

template<typename ...Args> 
class signal;

template<class... Args >
class __slot :  protected intrusive_list::node
{  
public:
    __slot() = default;
    virtual ~__slot() = default;
    virtual void operator()(Args...) = 0;

    void disconnect()
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
protected:
    template<typename... FriendArgs>
    friend class signal;
    
    __slot(intrusive_list& list) : 
        intrusive_list::node(list)
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

    bool queued:1 = false; 
    bool disconnected:1 = false;
};

template<class F, class... Args>
requires std::invocable<F, Args...>
class slot 
    : public __slot<Args...>
{
public:
    using BaseType = __slot<Args...>;

    slot() = default;


    slot(signal<Args...>& signal, F&& cb) :
        slot(signal.connect(std::move(cb)))
    {
    }

    void operator()(Args... args) override
    {
        if (!BaseType::queued)
            return;
        std::invoke(fn, std::forward<Args>(args)...);
    }
protected:
    template<typename... FriendArgs>
    friend class signal;
    
    slot(intrusive_list& list, F&& fn) :
        BaseType(list),
        fn(std::move(fn))
    {
    }

    F fn;

};

template<typename...Args>
class slot_fn: public slot<std::function<void(Args...)>, Args...>
{
public:
    using SlotType = slot<std::function<void(Args...)>, Args...>;
    using SlotType::slot;
};

template<typename ...Args>
class signal
{
public:
    signal() = default;

    auto connect(std::function<void(Args...)>&& cb)
    {
        if (cb)
            return slot_fn<Args...>(_slots, std::move(cb));
        else
            return slot_fn<Args...>();
    }

    template<typename Fn>
    requires std::invocable<Fn, Args...>
    auto connect2(Fn&& fn)
    {
        return slot<Fn, Args...>(_slots, std::move(fn));
    }

    void disconnect() 
    {
        _slots.clear();
    }
    
    template<typename... Pargs>
    void operator()(Pargs&&... args) const
    {
        using SlotType = __slot<Args...>;
        for(intrusive_list::node& _node: _slots) 
        {
            auto& s = static_cast<SlotType&>(_node);
            s.queue();
        }
        for(intrusive_list::node& _node: _slots) 
        {
            auto& s = static_cast<SlotType&>(_node);
            s(std::forward<Pargs>(args)...);
        }
        for(intrusive_list::node& _node: _slots) 
        {
            auto& s = static_cast<SlotType&>(_node);
            s.dequeue();
        }        
    }
    std::size_t connections() const { return _slots.size(); }
private:
    intrusive_list _slots;
};

}