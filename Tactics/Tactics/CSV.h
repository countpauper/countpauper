#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

namespace Engine
{
/*    template<typename T, typename U> size_t offsetOf(U T::*member)
    {
        return (char*)&((T*)nullptr->*member) - (char*)nullptr;
    }
*/
  
    namespace Adapter
    {
        template<class T>
        class Interface
        {
        public:
            Interface() = default;
            virtual void Parse(const std::vector<std::wstring>& str, T& dest) const = 0;
            virtual unsigned Columns() const = 0;
        };

        template<class T>
        class String : public Interface<T>
        {
        public:
            String(std::wstring T::*member) :
                member(member)
            {
            }
            void Parse(const std::vector<std::wstring>& str, T& dest) const override
            {
                (&dest)->*member = str.front();
            }
            unsigned Columns() const override { return 1; }
        private:
            std::wstring T::*member;
        };

        template<class T>
        class Integer : public Interface<T>
        {
        public:
            Integer(int T::*member) :
                member(member)
            {
            }
            void Parse(const std::vector<std::wstring>& str, T& dest) const override
            {
                (&dest)->*member =  _wtoi(str.front().data());
            }
            unsigned Columns() const override { return 1; }
        private:
            int T::*member;
        };

        template<class T, typename EnumT>
        class Enumeration : public Interface<T>
        {
        public:
            Enumeration(EnumT T::*member, const std::map<std::wstring, EnumT>& values) :
                member(member),
                values(values)
            {
            }
            void Parse(const std::vector<std::wstring>& str, T& dest) const override
            {
                (&dest)->*member = values.at(str.front());
            }
            unsigned Columns() const override { return 1; }
        private:
            EnumT T::*member;
            std::map<std::wstring, EnumT> values;
        };

        template<class T, class StructT>
        class Struct : public Interface < T >
        {
        public:
            Struct(StructT T::*member, const std::vector<Adapter::Interface<StructT>*> adapters) :
                member(member),
                adapters(adapters)
            {
            }
            void Parse(const std::vector<std::wstring>& str, T& dest) const override
            {
                unsigned index = 0;
                for (auto adapter : adapters)
                {
                    unsigned columns = adapter->Columns();
                    std::vector<std::wstring> subItems(str.begin() + index, str.begin() + index + columns);
                    adapter->Parse(subItems, (&dest)->*member);
                    index += columns;
                }
            }
            unsigned Columns() const override
            {
                unsigned total = 0;
                for (auto adapter : adapters)
                    total += adapter->Columns();
                return total;
            }
        private:
            StructT T::*member;
            const std::vector<Adapter::Interface<StructT>*> adapters;
        };

        template<class T>
        class Pointer : public Interface<T>
        {
        public:
            Pointer(unsigned offset, const std::map<std::wstring, T*> values) :
                Interface(offset)
            {
            }
            void Parse(const std::wstring& str, BYTE* dest) const override
            {

            }
            unsigned Columns() const override { return 1; }
        };
        /*
        class Optional : public Interface
        {
        public:
            Optional(unsigned offset, Interface& type);
            void Parse(const std::wstring& str, BYTE* dest) override;
        };
        */
    }

    template<class T>
    class CSV
    {
    public:
        CSV(std::wistream& file, std::vector<Adapter::Interface<T>*> columns) :
            file(file),
            columns(columns)
        {
        }
        std::vector<T> Read()
        {
            std::vector<T> result;
            while (!file.eof())
            {
                wchar_t buffer[65536];
                file.getline(buffer, 65536);
                if (buffer[0] == '#')
                    continue;
                result.push_back(T());
                std::wstringstream ss(buffer);
                for (auto column : columns)
                {
                    unsigned columnReq = column->Columns();
                    std::vector<std::wstring> items;
                    for (unsigned i = 0; i < columnReq; ++i)
                    {
                        wchar_t itemBuffer[4096];
                        ss.getline(itemBuffer, 4096, ',');
                        items.push_back(itemBuffer);
                    }
                    column->Parse(items, result.back());
                }
            }
            return result;
        }
    private:
        std::wistream& file;
        std::vector<Adapter::Interface<T>*> columns;
    };

}








