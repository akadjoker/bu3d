

#pragma once
#include "Config.hpp"

class Chars
{
private:
    static constexpr size_t SSO_BUFFER_SIZE = 15; // Tamanho do buffer SSO para strings pequenas

    size_t m_length{0};
    size_t m_capacity{0};
    union
    {
        char sso_data[SSO_BUFFER_SIZE + 1];
        char *m_data;
    };
    bool is_heap{false};

    void copy_data(const char *src, size_t len);
    void expand_capacity(size_t new_capacity);

public:
    Chars();

    Chars(const char *str);

    Chars(const char *str, size_t length);

    Chars(const Chars &other);

    Chars(Chars &&other) noexcept;
    Chars(char c);

    explicit Chars(int number);
    explicit Chars(float number);
    explicit Chars(double number);
    ~Chars();

    Chars &operator=(const Chars &str);
    Chars &operator+=(const char *rhs);
    Chars &operator+=(char c);
    Chars &operator+=(const Chars &rhs);
    Chars &operator=(Chars &&str) noexcept;
    bool operator==(const Chars &rhs) const;
    bool operator!=(const Chars &rhs) const;
     char &operator[](u32 index);
    const char &operator[](u32 index) const;

    size_t length() const { return m_length; }
    size_t size() const { return m_length; }
    size_t capacity() const { return m_capacity; }
    bool empty() const { return m_length == 0; }
    const char *c_str() const
    {
        return (is_heap ? m_data : sso_data);
    }

    float ToFloat() const;
    double ToDouble() const;
    int ToInt() const;
};
