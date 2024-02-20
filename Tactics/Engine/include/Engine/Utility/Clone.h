#pragma once
#include <memory>

namespace Engine
{

class IClone
{
public: 
    virtual IClone* clone() const = 0;
};


template<class CloneType>
class Clone : public IClone
{
public:
    //    IClone* clone() const override { return new typename _T(*this); }
    IClone* clone() const override 
    { 
        return dynamic_cast<IClone*>(new CloneType(dynamic_cast<const CloneType&>(*this)));
    }
};

template<class CloneType>
class clone_ptr
{
public:
    clone_ptr() = default;
    clone_ptr(const CloneType& ref) : 
        p(dynamic_cast<CloneType*>((dynamic_cast<const IClone&>(ref)).clone())) 
    {
    } 
    clone_ptr(const clone_ptr& o) : 
        p(dynamic_cast<CloneType*>((dynamic_cast<const IClone&>(*o.p)).clone()))
    {
    }
    clone_ptr(clone_ptr&& o) : 
        p(std::move(o.p)) 
    {
    }
    clone_ptr& operator=(const CloneType& ref)
    {
        p = dynamic_cast<CloneType*>((dynamic_cast<const IClone&>(ref)).clone());
    }
    clone_ptr& operator=(const clone_ptr& o)
    {
        p = dynamic_cast<CloneType*>((dynamic_cast<const IClone&>(o.p)).clone());
        return *this;
    }
    clone_ptr& operator=(clone_ptr&& o)
    {
        p = std::move(o.p);
        return *this;
    }
    CloneType& operator*()
    {
        return *p;
    }
    const CloneType& operator*() const
    {
        return *p;
    }
    CloneType* operator->()
    {
        return p.get();
    }
    const CloneType* operator->() const
    {
        return p.get();
    }
private:
    std::unique_ptr<CloneType> p;
};

}