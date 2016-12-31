#ifndef HEADER_GUARD_RAW_STRING_H
#define HEADER_GUARD_RAW_STRING_H

#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <iterator>
#include <utility>

class raw_string {
    class _normal_string_flag {};
    class _small_string_flag {};
    class _size {
        // Use last bit as flag for small string.
        std::size_t _s;
    public:
        _size() noexcept {}
        _size(_normal_string_flag flag, std::size_t c) noexcept {
            set(flag, c);
        }
        _size(_small_string_flag flag, std::size_t c) noexcept {
            set(flag, c);
        }

        std::size_t get(_normal_string_flag) const noexcept {
            return _s >> 1;
        }
        std::size_t get(_small_string_flag) const noexcept {
            return (_s & 0xFF) >> 1;
        }

        void set(_normal_string_flag, std::size_t s) noexcept {
            _s = (s << 1);
        }
        void set(_small_string_flag, std::size_t s) noexcept {
            _s = ((_s & ~std::size_t(0xFF)) |
                  (sizeof(_raw_string) - s) << 1);
        }

        bool is_small() const noexcept { return !is_normal(); }
        bool is_normal() const noexcept { return _s & 1; }

        void swap(_size& other) noexcept {
            std::swap(_s, other._s);
        }
    };
    struct _raw_string {
        char* data;
        std::size_t capacity;
        _size size;

        _raw_string(_small_string_flag) noexcept
            : size(_small_string_flag(), 0) {
            *small_string() = '\0';
        }
        _raw_string(_normal_string_flag, char* data,
                    std::size_t size) noexcept
            : data(data),
              capacity(size),
              size(_normal_string_flag(), size) {}
        _raw_string(_small_string_flag,
                    _raw_string&& other) noexcept {
            std::strcpy(small_string(), other.small_string());
        }
        _raw_string(_normal_string_flag,
                    _raw_string&& other) noexcept {
            data = other.data;
            other.data = 0;
            capacity = other.capacity;
            other.capacity = 0;
            size = other.size;
            other.size.set(_small_string_flag(), 0);
        }

        _raw_string(const _raw_string& other) = delete;
        _raw_string(_raw_string&& other) = delete;
        _raw_string& operator=(const _raw_string& other) = delete;
        _raw_string& operator=(_raw_string&& other) = delete;

        ~_raw_string() noexcept {
            if (size.is_normal()) {
                std::free(data);
            }
        }

        void
        assign(_small_string_flag, _raw_string&& other) noexcept {
            if (other.size.is_normal()) {
            } else {
            }
        }

        char* small_string() noexcept {
            return reinterpret_cast<char*>(this);
        }
        const char* small_string() const noexcept {
            return reinterpret_cast<const char*>(this);
        }

        void swap(_small_string_flag, _raw_string& other) {
            if (other.size.is_normal()) {
                _raw_string x(_normal_string_flag(), std::move(other));
                other.size.set(_small_string_flag(),
                               size.get(_small_string_flag()));
                std::strcpy(other.small_string(), small_string());
            } else {
                const std::size_t osize = other.size.get(
                                      _small_string_flag()),
                                  tsize =
                                      size.get(_small_string_flag());
                char buffer[sizeof(_raw_string)];
                std::strcpy(buffer, other.small_string());
                std::strcpy(other.small_string(), small_string());
                std::strcpy(small_string(), buffer);
                other.size.set(_small_string_flag(), tsize);
                size.set(_small_string_flag(), osize);
            }
        }

        void swap(_normal_string_flag, _raw_string& other) {
            if (other.size.is_normal()) {
                std::swap(data, other.data);
                std::swap(capacity, other.capacity);
                std::swap(size, other.size);
            } else {
                const std::size_t osize = other.size.get(
                                      _small_string_flag()),
                                  tsize =
                                      size.get(_small_string_flag());
                char buffer[sizeof(_raw_string)];
                std::strcpy(buffer, other.small_string());
                std::strcpy(other.small_string(), small_string());
                std::strcpy(small_string(), buffer);
                other.size.set(_small_string_flag(), tsize);
                size.set(_small_string_flag(), osize);
            }
        }
    } _impl;

public:
    using value_type = char;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = char&;
    using const_reference = const char&;

    using pointer = char*;
    using const_pointer = const char*;

    using iterator = char*;
    using const_iterator = const char*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    raw_string() noexcept : _impl(_small_string_flag()) {}
    raw_string(raw_string&& other) noexcept
        : _impl(_small_string_flag()) {
        _impl.swap(_small_string_flag(), other._impl);
    }

    size_type max_size() const noexcept {
        return (std::numeric_limits<size_type>::max() - 1) / 2;
    }

    const char* c_str() const noexcept {
        return data();
    }
    const char* data() const noexcept {
        if (_impl.size.is_small()) {
            return _impl.small_string();
        } else {
            return _impl.data;
        }
    }
    char* data() noexcept {
        if (_impl.size.is_small()) {
            return _impl.small_string();
        } else {
            return _impl.data;
        }
    }

    size_type size() const noexcept {
        if (_impl.size.is_small()) {
            return _impl.size.get(_small_string_flag());
        } else {
            return _impl.size.get(_normal_string_flag());
        }
    }

    char& operator[](size_type pos) noexcept {
        return data()[pos];
    }
    const char& operator[](size_type pos) const noexcept {
        return data()[pos];
    }

    char& front() noexcept { return *begin(); }
    const char& front() const noexcept { return *begin(); }
    char& back() noexcept { return *end(); }
    const char& back() const noexcept { return *end(); }

    iterator begin() noexcept { return data(); }
    const_iterator begin() const noexcept { return data(); }
    iterator end() noexcept { return data() + size(); }
    const_iterator end() const noexcept { return data() + size(); }

    size_type capacity() const noexcept {
        if (_impl.size.is_small()) {
            return sizeof(_impl);
        } else {
            return _impl.capacity;
        }
    }

    iterator
    find(const char* other, std::size_t beginning_pos = 0) noexcept {
        return std::strstr(c_str() + beginning_pos, other);
    }
    const_iterator
    find(const char* other,
         std::size_t beginning_pos = 0) const noexcept {
        return std::strstr(c_str() + beginning_pos, other);
    }
    iterator find(const raw_string& other) noexcept {
        return find(other.c_str());
    }
    const_iterator find(const raw_string& other) const noexcept {
        return find(other.c_str());
    }
    iterator
    find(value_type c, std::size_t beginning_pos = 0) noexcept {
        return std::strchr(c_str() + beginning_pos, c);
    }
    const_iterator
    find(value_type c, std::size_t beginning_pos = 0) const noexcept {
        return std::strchr(c_str() + beginning_pos, c);
    }

    iterator rfind(const char* other) noexcept {
        return const_cast<iterator>(
            static_cast<const raw_string&>(*this).rfind(other));
    }
    const_iterator rfind(const char* other) const noexcept {
        iterator ret = 0;
        for (auto i = std::strstr(c_str(), other); i;
             i = std::strstr(++i, other)) {
            ret = i;
        }
        return ret;
    }
    iterator rfind(const raw_string& other) noexcept {
        return rfind(other.c_str());
    }
    const_iterator rfind(const raw_string& other) const noexcept {
        return rfind(other.c_str());
    }
    iterator rfind(value_type c) noexcept {
        return std::strrchr(c_str(), c);
    }
    const_iterator rfind(value_type c) const noexcept {
        return std::strrchr(c_str(), c);
    }

    bool operator==(const raw_string& other) const noexcept {
        return std::strcmp(c_str(), other.c_str()) == 0;
    }
    bool operator!=(const raw_string& other) const noexcept {
        return !(*this == other);
    }

    bool operator<(const raw_string& other) const noexcept {
        return std::strcmp(c_str(), other.c_str()) < 0;
    }
    bool operator>(const raw_string& other) const noexcept {
        return other < *this;
    }
    bool operator<=(const raw_string& other) const noexcept {
        return !(other < *this);
    }
    bool operator>=(const raw_string& other) const noexcept {
        return !(*this < other);
    }
};

#endif
