#pragma once 
#include <deque>

namespace Interpreter 
{

// In infinite object stream uses all available memory as a buffer
// as opposed to a cirular stream
template<class T>   // T is the object type, which must be copyable
class InfiniteObjectStream 
{
public:
    InfiniteObjectStream<T>& operator<<(const T& object)
    {
        objects.push_back(object);
        return *this;
    }
    InfiniteObjectStream<T>& operator>>(T& object)
    {
        object = object[0];
        objects.pop_front();
        return *this;
    }
    std::deque<T> Flush()
    {
        std::deque<T> result;
        std::swap(result, objects);
        return result;
    }    
private:
    std::deque<T> objects;
};
        

}