#include "big_integer.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <ostream>
#include <stdexcept>

uint16_t static constexpr block = 32;
uint64_t static constexpr base = 1LL << block;
size_t static constexpr len = 9;
uint32_t static constexpr billion = 1'000'000'000;
big_integer static const zero = 0;

big_integer::big_integer() :
    is_negative(false),
    number(1, 0) {}

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(unsigned long long a) : is_negative(false) {
    fill_number(a);
}

big_integer::big_integer(long long a) : is_negative(a < 0) {
    fill_number(static_cast<uint64_t>(a));
}

big_integer::big_integer(unsigned long a) : big_integer(static_cast<unsigned long long>(a)) {}
big_integer::big_integer(long a) : big_integer(static_cast<long long>(a)) {}
big_integer::big_integer(unsigned int a) : big_integer(static_cast<unsigned long long>(a)) {}
big_integer::big_integer(int a) : big_integer(static_cast<long long>(a)) {}

void big_integer::fill_number(uint64_t a) {
    number.push_back(a % base);
    a /= base;
    number.push_back(a % base);
    min_length(*this);
}

big_integer::big_integer(std::string const& str) {
    size_t n = str.length();
    bool negate = false;
    if (n == 0) {
        throw std::invalid_argument(str);
    }
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        if (n == 1) {
            throw std::invalid_argument(str);
        }
        if (str[0] == '-') {
            negate = true;
        }
        start++;
    }
    for (size_t i = start; i < n; i++) {
        if (str[i] < '0' || str[i] > '9') {
            throw std::invalid_argument(str);
        }
    }
    size_t first = (n - start) % len + start;
    std::string buffer;
    if (first == start) {
        buffer = "0";
    } else {
        buffer = str.substr(start, first - start);
    }
    start = first;
    big_integer result(std::stoi(buffer));
    for(; start < n; start += len) {
        buffer = str.substr(start, len);
        result = result * billion + std::stoi(buffer);
    }
    if (negate) {
        result = -result;
    }
    is_negative = result.is_negative;
    number = result.number;
}

void swap(big_integer& a, big_integer& b) {
    std::swap(a.number, b.number);
    std::swap(a.is_negative, b.is_negative);
}

uint32_t big_integer::operator[](size_t i) {
    if (i < this->number.size()) {
        return number[i];
    } else {
        return get_mask();
    }
}

uint32_t big_integer::operator[](size_t i) const {
    if (i < this->number.size()) {
        return number[i];
    } else {
        return get_mask();
    }
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    size_t n = std::max(this->number.size(), rhs.number.size()) + 1;
    this->number.resize(n, get_mask());
    uint64_t result = 0;
    for (size_t i = 0; i < n; i++) {
        result += static_cast<uint64_t>((*this)[i]) + rhs[i];
        number[i] = static_cast<uint32_t>(result % base);
        result >>= block;
    }
    is_negative ^= (result == 1) ^ rhs.is_negative;
    min_length(*this);
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    return *this += -rhs;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    bool negative = this->is_negative ^ rhs.is_negative;
    this->abs();
    big_integer b(rhs);
    b.abs();
    size_t n1 = this->number.size();
    size_t n2 = b.number.size();
    std::vector<uint32_t> new_number(n1 + n2, 0);
    uint64_t result = 0;
    for (size_t i = 0; i < n2; i++) {
        auto element = static_cast<uint64_t>(b[i]);
        for (size_t j = 0; j < n1; j++) {
            result += element * (*this)[j] + new_number[i + j];
            new_number[i + j] = static_cast<uint32_t>(result % base);
            result /= base;
        }
        new_number[i + n1] += result;
        result = 0;
    }
    this->number = new_number;
    if (negative) {
        *this = -*this;
    }
    min_length(*this);
    return *this;
}

void big_integer::abs() {
    if (this->is_negative) {
        *this = -*this;
    }
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    bool negative = this->is_negative ^ rhs.is_negative;
    this->abs();
    big_integer b(rhs);
    b.abs();
    divide(*this, b);
    if (negative) {
        *this = -*this;
    }
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    bool negative = this->is_negative;
    this->abs();
    big_integer b(rhs);
    b.abs();
    *this = divide(*this, b);
    if (negative) {
        *this = -*this;
    }
    return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    bin_operator(rhs, [](uint32_t a, uint32_t b){return a & b;});
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    bin_operator(rhs, [](uint32_t a, uint32_t b){return a | b;});
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    bin_operator(rhs, [](uint32_t a, uint32_t b){return a ^ b;});
    return *this;
}

big_integer& big_integer::operator<<=(int rhs) {
    size_t shift = rhs / block;
    size_t left = rhs % block;
    size_t right = block - left;
    size_t n = number.size();
    size_t ind = n + shift + 1;
    number.resize(ind - (left == 0));
    number[ind - 1] = get_mask() << left;
    for (size_t i = n; i > 0; i--) {
        number[ind - 1] += number[i - 1] >> right;
        number[ind - 2] = number[i - 1] << left;
        ind--;
    }
    ind--;
    while (ind > 0) {
        number[ind - 1] = 0;
        ind--;
    }
    min_length(*this);
    return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
    size_t start = rhs / block;
    rhs %= block;
    uint32_t mask = get_mask();
    size_t n = number.size();
    if (start >= n) {
        number.clear();
        number.push_back(mask);
        return *this;
    }
    number.push_back(mask);
    number.push_back(mask);
    n += 2;
    uint64_t result = 0;
    number[0] = number[start] >> rhs;
    size_t ind = 1;
    for (size_t i = start + 1; i < n; i++) {
        result = static_cast<uint64_t>(number[i]) << (block - rhs);
        number[ind - 1] += result % base;
        number[ind] = result >> block;
        ind++;
    }
    for (size_t i = ind - 1; i < n; i++) {
        number.pop_back();
    }
    min_length(*this);
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    return invert_add(*this, 1);
}

big_integer big_integer::operator~() const {
    return invert_add(*this, 0);
}

big_integer big_integer::invert_add(big_integer tmp, uint64_t carry) {
    tmp.number.push_back(tmp.get_mask());
    size_t n = tmp.number.size();
    for (size_t i = 0; i < n; i++) {
        carry += ~tmp.number[i];
        tmp.number[i] = carry % base;
        carry >>= block;
    }
    if (carry == 1) {
        tmp.is_negative = false;
    } else {
        tmp.is_negative ^= 1;
    }
    min_length(tmp);
    return tmp;
}

big_integer& big_integer::operator++() {
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    big_integer tmp(*this);
    *this += 1;
    return tmp;
}

big_integer& big_integer::operator--() {
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    big_integer tmp(*this);
    *this -= 1;
    return tmp;
}

big_integer operator+(big_integer a, big_integer const& b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b) {
    return a.is_negative == b.is_negative && a.number == b.number;
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
    if (a.is_negative != b.is_negative) {
        return a.is_negative;
    }
    if (a.number.size() != b.number.size()) {
        return a.is_negative^(a.number.size() < b.number.size());
    }
    size_t n = a.number.size();
    for (size_t i = n; i > 0;) {
        i--;
        if (a.number[i] != b.number[i]) {
            return a.number[i] < b.number[i];
        }
    }
    return false;
}

bool operator>(big_integer const& a, big_integer const& b) {
    return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b) {
    return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b) {
    return !(a < b);
}

std::string to_string(big_integer const& a) {
    if (a == zero) {
        return "0";
    }
    std::string result;
    if (a.is_negative) {
        result += '-';
    }
    std::vector<uint32_t> blocks;
    big_integer tmp(a);
    tmp.abs();
    while (tmp > zero) {
        uint32_t num = big_integer::divide_short(tmp, billion);
        blocks.push_back(num);
    }
    size_t n = blocks.size();
    result += std::to_string(blocks[n - 1]);
    for (size_t i = n - 1; i > 0;) {
        i--;
        std::string s = std::to_string(blocks[i]);
        result += std::string(len - s.length(), '0') + s;
    }
    return result;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}

uint32_t big_integer::get_mask() {
    return this->is_negative ? static_cast<uint32_t>(base - 1) : 0;
}

uint32_t big_integer::get_mask() const {
    return this->is_negative ? static_cast<uint32_t>(base - 1) : 0;
}

void big_integer::min_length(big_integer& a) {
    uint32_t mask = a.get_mask();
    while (a.number.size() > 1 && a.number[a.number.size() - 1] == mask) {
        a.number.pop_back();
    }
}

void big_integer::resize(big_integer& a, size_t sz) {
    uint32_t mask = a.get_mask();
    size_t n = sz - a.number.size();
    while (n > 0) {
        a.number.push_back(mask);
        n--;
    }
}

void big_integer::bin_operator(big_integer const& rhs, const std::function<uint32_t (uint32_t, uint32_t)>& f) {
    size_t n = std::max(number.size(), rhs.number.size());
    big_integer::resize(*this, n);
    for (size_t i = 0; i < n; i++) {
        number[i] = f((*this)[i], rhs[i]);
    }
    is_negative = f(is_negative, rhs.is_negative);
    min_length(*this);
}

uint32_t big_integer::divide_short(big_integer& a, uint32_t const b) {
    size_t n = a.number.size();
    uint64_t res = 0;
    while (n > 0) {
        n--;
        res += a.number[n];
        a.number[n] = res / b;
        res %= b;
        res <<= block;
    }
    res >>= block;
    min_length(a);
    return static_cast<uint32_t>(res);
}

big_integer big_integer::divide(big_integer& a, big_integer& b) {
    if (a < b) {
        big_integer old(a);
        a.number.clear();
        a.number.push_back(0);
        return old;
    }
    if (b.number.size() == 1) {
        return static_cast<big_integer>(divide_short(a, b[0]));
    }
    return long_divide(a, b);
}

uint32_t trial(std::vector<uint32_t> &a, uint32_t b) {
    auto qt = static_cast<uint64_t>(a.back());
    qt = ((qt << block) + a[a.size() - 2]) / b;
    qt = std::min(qt, base - 1);
    return static_cast<uint32_t>(qt);
}

bool smaller(std::vector<uint32_t> &a, std::vector<uint32_t> &b) {
    for (size_t i = a.size(); i > 0; i--) {
        if (a[i - 1] != b[i - 1]) {
            return a[i - 1] < b[i - 1];
        }
    }
    return false;
}

void difference(std::vector<uint32_t> &a, std::vector<uint32_t> &b) {
    uint64_t res = static_cast<uint64_t>(~b[0]) + a[0] + 1LL;
    uint64_t carry = res >> block;
    a[0] = static_cast<uint32_t>(res);
    for (size_t i = 1; i < b.size(); i++) {
        res = static_cast<uint64_t>(~b[i]) + a[i] + carry;
        a[i] = static_cast<uint32_t>(res);
        carry = res >> block;
    }
}

void product(std::vector<uint32_t> & result, std::vector<uint32_t> &a, uint32_t b) {
    size_t n = a.size();
    result.resize(n + 1);
    uint64_t carry = 0;
    uint64_t tmp;
    uint64_t res;
    for (size_t i = 0; i < n; i++) {
        tmp = static_cast<uint64_t>(a[i]) * b;
        res = (tmp << block >> block) + carry;
        result[i] = static_cast<uint32_t>(res);
        carry = (tmp >> block) + (res >> block);
    }
    result[n] = static_cast<uint32_t>(carry);
}

void shift(std::vector<uint32_t> &a) {
    for (size_t j = a.size() - 1; j > 0; j--) {
        a[j] = a[j - 1];
    }
}

big_integer big_integer::long_divide(big_integer& a, big_integer& b) {
    auto last = static_cast<uint64_t>(b.number.back());
    auto f = static_cast<uint32_t>((base / (last + 1)));
    size_t n = a.number.size();
    size_t m = b.number.size();
    std::vector<uint32_t> result(n - m + 1);
    std::vector<uint32_t> buffer(m + 1);
    std::vector<uint32_t> dq(m + 1, 0);
    big_integer new_a = a;
    big_integer new_b = b;
    product(new_a.number, new_a.number, f);
    product(new_b.number, new_b.number, f);
    min_length(new_a);
    min_length(new_b);
    for (size_t i = 0; i <= m; i++) {
        buffer[i] = new_a[n + i - m];
    }
    for (size_t i = n - m + 1; i > 0; i--) {
        buffer[0] = new_a[i - 1];
        uint32_t qt = trial(buffer, new_b.number.back());
        product(dq, new_b.number, qt);
        while (qt >= 0 && smaller(buffer, dq)) {
            qt--;
            product(dq, new_b.number, qt);
        }
        difference(buffer, dq);
        shift(buffer);
        result[i - 1] = qt;
    }
    a.number = result;
    min_length(a);
    big_integer remainder(0);
    for (size_t i = 1; i < buffer.size(); i++) {
        buffer[i - 1] = buffer[i];
    }
    buffer.pop_back();
    remainder.number = buffer;
    min_length(remainder);
    divide_short(remainder, f);
    return remainder;
}
