#pragma once
#include <iostream>
#include "MatrixIO.h"

namespace Net
{
	namespace IO
	{
		const char separator = ' ';

		template<typename T> std::string classname(const T& obj)
		{
			std::string fullName = std::string(typeid(obj).name());
			assert(fullName.substr(0, 6) == "class ");
			return fullName.substr(6, std::string::npos);
		}

		template<typename T> std::string classname_ptr(const T* obj)
		{
			if (!obj)
				return "null";
			else
				return classname(*obj);
		}

		template<typename T>
		bool CompareUniquePtr(const std::unique_ptr<T>& ptr, const T& ref)
		{
			return ptr.get() == &ref;
		}

	}
}
