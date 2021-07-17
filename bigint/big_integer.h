#pragma once

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

struct big_integer
{
    big_integer();
    big_integer(big_integer const& other);
    big_integer(unsigned long long a);
    big_integer(long long a);
    big_integer(unsigned long a);
    big_integer(long a);
    big_integer(unsigned int a);
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer() = default;

    big_integer& operator=(big_integer const& other) = default;

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    uint32_t operator[](size_t);
    uint32_t operator[](size_t) const;

    friend void swap(big_integer& a, big_integer& b);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

    void abs();

private:
    void fill_number(uint64_t a);
    void bin_operator(big_integer const& b, const std::function<uint32_t (uint32_t, uint32_t)>& f);
    static void resize(big_integer& a, size_t sz);
    static void min_length(big_integer& a);
    static big_integer divide(big_integer& a, big_integer& b);
    static big_integer long_divide(big_integer& a, big_integer& b);
    static big_integer invert_add(big_integer tmp, uint64_t carry);
    static uint32_t divide_short(big_integer& a, uint32_t b);
    uint32_t get_mask();
    uint32_t get_mask() const;
    bool is_negative;
    std::vector<uint32_t> number;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);
