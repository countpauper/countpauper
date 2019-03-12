#pragma once

#include <functional>
namespace Engine
{

class Connectable
{
public:
	Connectable() = default;
	virtual ~Connectable() = default;
protected:
	friend class Connection;
	virtual void disconnect() = 0;
};

class Connection final
{
public:
	Connection() :
		callback(nullptr)
	{
	}
	Connection(Connectable& callback) :
		callback(&callback)
	{
	}
	Connection(const Connection&) = delete;
	Connection(Connection&& other) :
		callback(other.callback)
	{
		other.callback = nullptr;
	}
	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&& other)
	{
		disconnect();
		callback = other.callback;
		other.callback = nullptr;
		return *this;
	}
	~Connection()
	{
		disconnect();
	}
	void disconnect()
	{
		if (callback)
			callback->disconnect();
	}
private:
	Connectable* callback;
};

template<typename ...Args>
class Callback : protected Connectable
{
public:
	Callback() = default;

	void operator()(Args... args)
	{
		if (callback)
			return callback(std::forward<Args>(args)...);
	}

	Connection connect(const std::function<void(Args...)>& fn)
	{
		// This might leave connections to the old 
		if (callback)
			throw std::runtime_error("Callback already connected");
		callback = fn;
		return Connection(*this);
	}
private:
	void disconnect() override
	{
		callback = std::function<void(Args...)>();
	}
	std::function<void(Args...)> callback;
};


};
