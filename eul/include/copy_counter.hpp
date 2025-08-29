#pragma once 
namespace eul 
{

// copy counter checks how many times an instance is copied, moved or destructed
// this is useful when unit testing the efficiency&lifetime requirements of container utilities
// but implemented as a generic utility so it can also be used on target.

struct copy_count
{
    int current() const;
    void reset();
    unsigned constructed = 0;
    unsigned copied = 0;
    unsigned moved = 0;
    unsigned assigned = 0;
    unsigned destructed = 0;
};

class copy_counter
{
public:
    copy_counter(copy_count& count=copy_counter::default_count);
    virtual ~copy_counter();
    copy_counter(const copy_counter& original);
    copy_counter(copy_counter&& original);
    copy_counter& operator=(const copy_counter& original);
    copy_counter& operator=(copy_counter&& original);
    bool operator==(const copy_counter& other) const;
    explicit operator bool() const;
    const copy_count* operator->() const;
private:
    static copy_count default_count;
    copy_count* _count;
};

}