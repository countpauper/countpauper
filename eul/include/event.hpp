namespace eul
{

class Event 
{
public:
    Event() ;
    Event(const Event& o);
    Event& operator=(const Event& o);
    bool operator==(const Event& o) const;
private:
    unsigned id;
    static unsigned lastId;
};

}
