/**
 * Copyright (c) 2014 Graeme Hill (http://graemehill.ca)
 * Complete refactor (c) 2018 Jorma Rebane (http://github.com/RedFox20)
 * Distributed under MIT Software License
 */
#pragma once
#include <array>
#include <string>
#include <vector>

#ifndef CPPGUID_API
#  if _MSC_VER
#    define CPPGUID_API __declspec(dllexport)
#  else // clang/gcc
#    define CPPGUID_API __attribute__((visibility("default")))
#  endif
#endif

namespace cppguid
{
    // Class to represent a GUID/UUID. Each instance acts as a wrapper around a
    // 16 byte value that can be passed around by value. It also supports
    // conversion to string (via the stream operator <<) and conversion from a
    // string via constructor.
    struct CPPGUID_API Guid
    {
        unsigned char bytes[16];

        // Initializes a new GUID; This is the main interface to use for creating new GUID-s
        static Guid create();

        // Equivalent to Guid::create().str();
        // "00112233-4455-6677-8899-AABBCCDDEEFF"
        static std::string create_str();
    
        // creates a zeroed guid
        Guid() noexcept;
        
        // initialize from different array types
        explicit Guid(const unsigned char* bytes) noexcept;
        explicit Guid(const std::array<unsigned char, 16>& bytes)  noexcept : Guid(bytes.data()) {}
        explicit Guid(const std::vector<unsigned char>& bytes)     noexcept : Guid(bytes.data()) {}

        /**
         * Initialize GUID from a string: "00112233-4455-6677-8899-AABBCCDDEEFF"
         * If parsing fails, then Guid::valid() will return false
         */
        explicit Guid(const std::string& fromString) noexcept;
        Guid(const Guid& other) noexcept;
        
        Guid& operator=(const Guid& other) noexcept;
        bool operator==(const Guid& other) const noexcept;
        bool operator!=(const Guid& other) const noexcept;

        /**
         * @return Current GUID as a string: "00112233-4455-6677-8899-AABBCCDDEEFF"
         */
        std::string str() const;
        explicit operator std::string() const { return str(); }

        void swap(Guid& other) noexcept;

        bool valid() const noexcept;
        explicit operator bool() const { return valid(); }
        
        void zeroify(); // set all bytes to zero
    };

    // support any type of stream
    // and always serialize as a string
    template<class Stream> Stream& operator<<(Stream& s, const Guid& guid)
    {
        return s << guid.str();
    }

}

namespace std
{
    template <> inline void swap(cppguid::Guid& a, cppguid::Guid& b) noexcept
    {
        a.swap(b);
    }
    template <> struct hash<cppguid::Guid>
    {
        size_t operator()(const cppguid::Guid& guid) const;
    };
}
