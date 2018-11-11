#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "from_string.h"

namespace Engine
{
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

        template<class C, typename T>
        class Setter: public Interface <C>
        {
        public:
            using FuncType = void (C::*)(const T&);

            Setter(FuncType fn) :
                fn(fn)
            {
            }
            void Parse(const std::vector<std::wstring>& str, C& dest) const override
            {
                T v = from_string<T>(str.front());
                (dest.*fn)(v);
            }
            unsigned Columns() const override { return 1; }
        private:
            FuncType fn;
        };

        template<class C>
        class String : public Interface<C>
        {
        public:
            String(std::wstring C::*member) :
                member(member)
            {
            }
            void Parse(const std::vector<std::wstring>& str, C& dest) const override
            {
                (&dest)->*member = str.front();
            }
            unsigned Columns() const override { return 1; }
        private:
            std::wstring C::*member;
        };

        template<class C, typename T>
        class Number : public Interface<C>
        {
        public:
            Number(T C::*member) :
                member(member)
            {
            }
            void Parse(const std::vector<std::wstring>& str, C& dest) const override
            {
                (&dest)->*member =  from_string<T>(str.front());
            }
            unsigned Columns() const override { return 1; }
        private:
            T C::*member;
        };

        template<class C> using Integer = Number<C, int>;
        template<class C> using Unsigned = Number<C, unsigned>;

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
                if (buffer[0] == 0)
                    break;
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








