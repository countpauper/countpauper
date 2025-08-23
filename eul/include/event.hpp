namespace eul
{

class event 
{
public:
    event() ;
    event(const event& o);
    event& operator=(const event& o);
    bool operator==(const event& o) const;
private:
    unsigned _id;
    static unsigned _last_Id;
};

}
