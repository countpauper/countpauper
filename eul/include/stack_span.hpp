#pragma once 
#include <span>
#include <malloc.h>

namespace eul 
{
template<typename T> 
std::span<T> stack_span(std::size_t amount)
{
    return std::span<T>(static_cast<T*>(alloca(amount * sizeof(T))), amount); 
}

}