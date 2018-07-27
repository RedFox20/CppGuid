/**
 * Copyright (c) 2014 Graeme Hill (http://graemehill.ca) https://github.com/graeme-hill/crossguid
 * Complete refactor (c) 2018 Jorma Rebane (http://github.com/RedFox20)
 * Distributed under MIT Software License
 */
#include <cppguid/guid.h>
#include <cctype>

#ifdef __APPLE__
#  include <CoreFoundation/CFUUID.h>
#elif _WIN32
#  include <objbase.h>
#elif __ANDROID__
#  include <jni.h>
#  include <assert.h>
#elif __linux__
#  define LIBUUID 1
#  include <uuid/uuid.h>
#endif

namespace cppguid
{
    
    Guid::Guid() noexcept : bytes{ 0 } { }

    Guid::Guid(const unsigned char* bytes) noexcept
    {
        auto* src = (const uint64_t*)bytes;
        auto* dst = (uint64_t*)this->bytes;
        dst[0] = src[0];
        dst[1] = src[1];
    }

    // converts a single hex char to a number (0 - 15)
    static int hexToInt(char ch)
    {
        if ('0' <= ch && ch <= '9') return ch - '0';
        if ('a' <= ch && ch <= 'f') return ch - 'W'; // hack 'a'-10 == 'W'
        if ('A' <= ch && ch <= 'F') return ch - '7'; // hack 'A'-10 == '7'
        return 0;
    }

    // converts the two hexadecimal characters to an unsigned char (a byte)
    static int hexPairToChar(char a, char b)
    {
        return (hexToInt(a) << 4) + hexToInt(b);
    }

    // create a guid from string
    Guid::Guid(const std::string& fromString) noexcept
    {
        const int maxGuidLength = 36;
        int len = 0;
        size_t i = 0;
        for (; i < fromString.size() && i < (maxGuidLength-1); ++i)
        {
            char ch = fromString[i];
            if (ch == '-')
                continue;

            char ch2 = fromString[++i];
            if (!std::isxdigit(ch) || !std::isxdigit(ch2)) // Invalid string so bail
            {
                zeroify();
                return;
            }

            auto byte = hexPairToChar(ch, ch2);
            bytes[len++] = byte;
        }

        // if there were fewer than 16 bytes in the string then guid is bad
        if (len < 16)
        {
            zeroify();
            return;
        }
    }

    Guid::Guid(const Guid& other) noexcept
    {
        memcpy(bytes, other.bytes, sizeof(bytes));
    }

    void Guid::zeroify()
    {
        memset(bytes, 0, sizeof(bytes));
    }

    Guid& Guid::operator=(const Guid& other) noexcept
    {
        Guid(other).swap(*this);
        return *this;
    }

    bool Guid::operator==(const Guid& other) const noexcept
    {
        return memcmp(bytes, other.bytes, sizeof(bytes)) == 0;
    }

    bool Guid::operator!=(const Guid& other) const noexcept
    {
        return memcmp(bytes, other.bytes, sizeof(bytes)) != 0;
    }

    void Guid::swap(Guid& other) noexcept
    {
        std::swap(bytes, other.bytes);
    }

    bool Guid::valid() const noexcept
    {
        static Guid empty;
        return *this != empty;
    }

    static const char HEX[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

    struct hex_builder
    {
        int len = 0;
        char buf[40];
        void append(const uint8_t* data, int n)
        {
            for (int i = 0; i < n; ++i)
            {
                uint8_t ch = data[i];
                buf[len++] = HEX[ch >> 4];
                buf[len++] = HEX[ch & 0x0f];
            }
        }
        void dash()
        {
            buf[len++] = '-';
        }
    };

    std::string Guid::str() const
    {
        // "00112233-4455-6677-8899-AABBCCDDEEFF"
        hex_builder h;
        h.append(&bytes[0], 4); h.dash();
        h.append(&bytes[4], 2); h.dash();
        h.append(&bytes[6], 2); h.dash();
        h.append(&bytes[8], 2); h.dash();
        h.append(&bytes[10], 6);
        return std::string{ h.buf, h.buf + h.len };
    }

    std::string Guid::create_str()
    {
        return create().str();
    }

#ifdef LIBUUID
    Guid Guid::create()
    {
        uuid_t id;
        uuid_generate(id);
        return Guid{ id };
    }
#endif

#ifdef __APPLE__
    Guid Guid::create()
    {
        auto newId = CFUUIDCreate(NULL);
        auto bytes = CFUUIDGetUUIDBytes(newId);
        CFRelease(newId);

        unsigned char byteArray[16] =
        {
            bytes.byte0,
            bytes.byte1,
            bytes.byte2,
            bytes.byte3,
            bytes.byte4,
            bytes.byte5,
            bytes.byte6,
            bytes.byte7,
            bytes.byte8,
            bytes.byte9,
            bytes.byte10,
            bytes.byte11,
            bytes.byte12,
            bytes.byte13,
            bytes.byte14,
            bytes.byte15
        };
        return Guid{ byteArray };
    }
#endif

#ifdef _WIN32
    Guid Guid::create()
    {
        GUID newId;
        CoCreateGuid(&newId);
        std::array<unsigned char, 16> bytes =
        {
            (unsigned char)((newId.Data1 >> 24) & 0xFF),
            (unsigned char)((newId.Data1 >> 16) & 0xFF),
            (unsigned char)((newId.Data1 >> 8) & 0xFF),
            (unsigned char)((newId.Data1) & 0xff),

            (unsigned char)((newId.Data2 >> 8) & 0xFF),
            (unsigned char)((newId.Data2) & 0xff),

            (unsigned char)((newId.Data3 >> 8) & 0xFF),
            (unsigned char)((newId.Data3) & 0xFF),

            (unsigned char)newId.Data4[0],
            (unsigned char)newId.Data4[1],
            (unsigned char)newId.Data4[2],
            (unsigned char)newId.Data4[3],
            (unsigned char)newId.Data4[4],
            (unsigned char)newId.Data4[5],
            (unsigned char)newId.Data4[6],
            (unsigned char)newId.Data4[7]
        };
        return Guid{ bytes };
    }
#endif

#ifdef __ANDROID__

    static JNIEnv* JniEnv()
    {
        static thread_local JNIEnv* env = [] // init once for each thread
        {
            Assert(javaVM, "JniEnv() used before JNI_OnLoad(). Avoid calling JNI methods in static initializers.");
            JNIEnv* e = nullptr;
            if (javaVM->GetEnv((void**)&e, JNI_VERSION_1_6) != JNI_OK)
                javaVM->AttachCurrentThread(&e, nullptr);
            assert(e && "javaVM->AttachCurrentThread() returned null JNIEnv*");
            return e;
        }();
        return env;
    }

    Guid Guid::create()
    {
        JNIEnv* env = JniEnv();


        env->DeleteLocalRef(obj);

        Class UUID { "java/util/UUID" }; // @note We can't store jclass statically
        auto randomUUID              = UUID.SMethod("randomUUID", "()Ljava/util/UUID;");
        auto getMostSignificantBits  = UUID.Method("getMostSignificantBits", "()J");
        auto getLeastSignificantBits = UUID.Method("getLeastSignificantBits", "()J");

        auto javaUuid = randomUUID.Object(nullptr);
        jlong mostSignificant  = getMostSignificantBits.Long(javaUuid);
        jlong leastSignificant = getLeastSignificantBits.Long(javaUuid);

        unsigned char bytes[16] =
        {
            (unsigned char)((mostSignificant >> 56) & 0xFF),
            (unsigned char)((mostSignificant >> 48) & 0xFF),
            (unsigned char)((mostSignificant >> 40) & 0xFF),
            (unsigned char)((mostSignificant >> 32) & 0xFF),
            (unsigned char)((mostSignificant >> 24) & 0xFF),
            (unsigned char)((mostSignificant >> 16) & 0xFF),
            (unsigned char)((mostSignificant >> 8) & 0xFF),
            (unsigned char)((mostSignificant) & 0xFF),
            (unsigned char)((leastSignificant >> 56) & 0xFF),
            (unsigned char)((leastSignificant >> 48) & 0xFF),
            (unsigned char)((leastSignificant >> 40) & 0xFF),
            (unsigned char)((leastSignificant >> 32) & 0xFF),
            (unsigned char)((leastSignificant >> 24) & 0xFF),
            (unsigned char)((leastSignificant >> 16) & 0xFF),
            (unsigned char)((leastSignificant >> 8) & 0xFF),
            (unsigned char)((leastSignificant) & 0xFF)
        };
        return Guid{ bytes };
    }
#endif

}

namespace std
{
    size_t hash<cppguid::Guid>::operator()(const cppguid::Guid& guid) const
    {
        #if INTPTR_MAX == INT64_MAX // 64-bit
            static_assert(sizeof(size_t) == 8, "Expected 64-bit build");
            constexpr size_t FNV_offset_basis = 14695981039346656037ULL;
            constexpr size_t FNV_prime = 1099511628211ULL;
        #elif INTPTR_MAX == INT32_MAX // 32-bit
            static_assert(sizeof(size_t) == 4, "Expected 32-bit build");
            constexpr size_t FNV_offset_basis = 2166136261U;
            constexpr size_t FNV_prime = 16777619U;
        #endif
        const unsigned char* p = guid.bytes;
        size_t value = FNV_offset_basis;
        for (auto e = p + 16; p < e; ++p) {
            value ^= (size_t)*p;
            value *= FNV_prime;
        }
        return value;
    }
}
