#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

namespace sjtu {

class int2048 {
private:
  static constexpr int base = 1000000000;
  static constexpr int base_width = 9;
  static constexpr int fft_base = 1000;
  static constexpr std::size_t fft_parts = 3;
  static constexpr std::size_t fft_threshold = 64;

  std::vector<int> digits;
  bool negative;

  void normalize();

  static bool is_zero(const std::vector<int> &value);
  static int compare_abs(const std::vector<int> &lhs,
                         const std::vector<int> &rhs);

  void add_abs(const std::vector<int> &rhs);
  void subtract_abs(const std::vector<int> &rhs);

  static std::vector<int>
  multiply_abs(const std::vector<int> &lhs,
               const std::vector<int> &rhs);

  static std::vector<int>
  multiply_abs_naive(const std::vector<int> &lhs,
                     const std::vector<int> &rhs);

  static std::vector<int>
  multiply_abs_fft(const std::vector<int> &lhs,
                   const std::vector<int> &rhs);

  static void fft(std::vector<std::complex<double>> &values,
                  bool inverse);

  static void divmod_abs(const std::vector<int> &dividend,
                         const std::vector<int> &divisor,
                         std::vector<int> &quotient,
                         std::vector<int> &remainder);

public:
  int2048();
  int2048(long long value);
  int2048(const std::string &value);
  int2048(const int2048 &other);

  void read(const std::string &value);
  void print();

  int2048 &add(const int2048 &rhs);
  friend int2048 add(int2048 lhs, const int2048 &rhs);

  int2048 &minus(const int2048 &rhs);
  friend int2048 minus(int2048 lhs, const int2048 &rhs);

  int2048 operator+() const;
  int2048 operator-() const;

  int2048 &operator=(const int2048 &rhs);

  int2048 &operator+=(const int2048 &rhs);
  friend int2048 operator+(int2048 lhs, const int2048 &rhs);

  int2048 &operator-=(const int2048 &rhs);
  friend int2048 operator-(int2048 lhs, const int2048 &rhs);

  int2048 &operator*=(const int2048 &rhs);
  friend int2048 operator*(int2048 lhs, const int2048 &rhs);

  int2048 &operator/=(const int2048 &rhs);
  friend int2048 operator/(int2048 lhs, const int2048 &rhs);

  int2048 &operator%=(const int2048 &rhs);
  friend int2048 operator%(int2048 lhs, const int2048 &rhs);

  friend std::istream &operator>>(std::istream &input,
                                  int2048 &value);
  friend std::ostream &operator<<(std::ostream &output,
                                  const int2048 &value);

  friend bool operator==(const int2048 &lhs,
                         const int2048 &rhs);
  friend bool operator!=(const int2048 &lhs,
                         const int2048 &rhs);
  friend bool operator<(const int2048 &lhs,
                        const int2048 &rhs);
  friend bool operator>(const int2048 &lhs,
                         const int2048 &rhs);
  friend bool operator<=(const int2048 &lhs,
                         const int2048 &rhs);
  friend bool operator>=(const int2048 &lhs,
                         const int2048 &rhs);
};

int2048::int2048() : digits(1, 0), negative(false) {}

int2048::int2048(long long value) : digits(), negative(value < 0) {
  unsigned long long magnitude;

  if (value < 0) {
    magnitude = static_cast<unsigned long long>(-(value + 1));
    ++magnitude;
  } else {
    magnitude = static_cast<unsigned long long>(value);
  }

  if (magnitude == 0) {
    digits.push_back(0);
    negative = false;
    return;
  }

  while (magnitude != 0) {
    digits.push_back(static_cast<int>(magnitude % base));
    magnitude /= base;
  }
}

int2048::int2048(const std::string &value)
    : digits(1, 0), negative(false) {
  read(value);
}

int2048::int2048(const int2048 &other)
    : digits(other.digits), negative(other.negative) {}

void int2048::normalize() {
  while (digits.size() > 1 && digits.back() == 0) {
    digits.pop_back();
  }

  if (is_zero(digits)) {
    negative = false;
  }
}

bool int2048::is_zero(const std::vector<int> &value) {
  return value.size() == 1 && value[0] == 0;
}

int int2048::compare_abs(const std::vector<int> &lhs,
                         const std::vector<int> &rhs) {
  if (lhs.size() != rhs.size()) {
    return lhs.size() < rhs.size() ? -1 : 1;
  }

  for (std::size_t i = lhs.size(); i > 0; --i) {
    if (lhs[i - 1] != rhs[i - 1]) {
      return lhs[i - 1] < rhs[i - 1] ? -1 : 1;
    }
  }

  return 0;
}

void int2048::add_abs(const std::vector<int> &rhs) {
  const std::size_t original_size = digits.size();
  const std::size_t rhs_size = rhs.size();
  const std::size_t result_size =
      original_size > rhs_size ? original_size : rhs_size;

  digits.resize(result_size + 1, 0);

  int carry = 0;

  for (std::size_t i = 0; i < result_size; ++i) {
    int current = carry;

    if (i < original_size) {
      current += digits[i];
    }

    if (i < rhs_size) {
      current += rhs[i];
    }

    digits[i] = current % base;
    carry = current / base;
  }

  digits[result_size] = carry;
  normalize();
}

void int2048::subtract_abs(const std::vector<int> &rhs) {
  int borrow = 0;

  for (std::size_t i = 0; i < digits.size(); ++i) {
    int current = digits[i] - borrow;

    if (i < rhs.size()) {
      current -= rhs[i];
    }

    if (current < 0) {
      current += base;
      borrow = 1;
    } else {
      borrow = 0;
    }

    digits[i] = current;
  }

  normalize();
}

void int2048::fft(std::vector<std::complex<double>> &values,
                  bool inverse) {
  const std::size_t size = values.size();

  for (std::size_t i = 1, j = 0; i < size; ++i) {
    std::size_t bit = size >> 1;

    while ((j & bit) != 0) {
      j ^= bit;
      bit >>= 1;
    }

    j ^= bit;

    if (i < j) {
      const std::complex<double> temporary = values[i];
      values[i] = values[j];
      values[j] = temporary;
    }
  }

  const double pi = std::acos(-1.0);

  for (std::size_t length = 2; length <= size; length <<= 1) {
    const double angle =
        2.0 * pi / static_cast<double>(length) *
        (inverse ? -1.0 : 1.0);

    const std::complex<double> root(std::cos(angle),
                                    std::sin(angle));

    for (std::size_t start = 0; start < size; start += length) {
      std::complex<double> factor(1.0, 0.0);
      const std::size_t half = length >> 1;

      for (std::size_t offset = 0; offset < half; ++offset) {
        const std::complex<double> even =
            values[start + offset];
        const std::complex<double> odd =
            values[start + offset + half] * factor;

        values[start + offset] = even + odd;
        values[start + offset + half] = even - odd;
        factor *= root;
      }
    }
  }

  if (inverse) {
    for (std::size_t i = 0; i < size; ++i) {
      values[i] /= static_cast<double>(size);
    }
  }
}

std::vector<int>
int2048::multiply_abs_naive(const std::vector<int> &lhs,
                            const std::vector<int> &rhs) {
  if (is_zero(lhs) || is_zero(rhs)) {
    return std::vector<int>(1, 0);
  }

  std::vector<int> result(lhs.size() + rhs.size(), 0);

  for (std::size_t i = 0; i < lhs.size(); ++i) {
    long long carry = 0;

    for (std::size_t j = 0; j < rhs.size(); ++j) {
      const long long current =
          static_cast<long long>(result[i + j]) +
          static_cast<long long>(lhs[i]) * rhs[j] +
          carry;

      result[i + j] = static_cast<int>(current % base);
      carry = current / base;
    }

    result[i + rhs.size()] = static_cast<int>(carry);
  }

  while (result.size() > 1 && result.back() == 0) {
    result.pop_back();
  }

  return result;
}

std::vector<int>
int2048::multiply_abs_fft(const std::vector<int> &lhs,
                          const std::vector<int> &rhs) {
  std::vector<int> left_parts(lhs.size() * fft_parts, 0);
  std::vector<int> right_parts(rhs.size() * fft_parts, 0);

  for (std::size_t i = 0; i < lhs.size(); ++i) {
    int chunk = lhs[i];

    for (std::size_t part = 0; part < fft_parts; ++part) {
      left_parts[i * fft_parts + part] = chunk % fft_base;
      chunk /= fft_base;
    }
  }

  for (std::size_t i = 0; i < rhs.size(); ++i) {
    int chunk = rhs[i];

    for (std::size_t part = 0; part < fft_parts; ++part) {
      right_parts[i * fft_parts + part] = chunk % fft_base;
      chunk /= fft_base;
    }
  }

  while (left_parts.size() > 1 && left_parts.back() == 0) {
    left_parts.pop_back();
  }

  while (right_parts.size() > 1 && right_parts.back() == 0) {
    right_parts.pop_back();
  }

  const std::size_t coefficient_count =
      left_parts.size() + right_parts.size() - 1;

  std::size_t transform_size = 1;

  while (transform_size < coefficient_count) {
    transform_size <<= 1;
  }

  std::vector<std::complex<double>> left(transform_size);
  std::vector<std::complex<double>> right(transform_size);

  for (std::size_t i = 0; i < left_parts.size(); ++i) {
    left[i] = std::complex<double>(
        static_cast<double>(left_parts[i]), 0.0);
  }

  for (std::size_t i = 0; i < right_parts.size(); ++i) {
    right[i] = std::complex<double>(
        static_cast<double>(right_parts[i]), 0.0);
  }

  fft(left, false);
  fft(right, false);

  for (std::size_t i = 0; i < transform_size; ++i) {
    left[i] *= right[i];
  }

  fft(left, true);

  std::vector<int> small_digits;
  small_digits.reserve(coefficient_count + 8);

  long long carry = 0;

  for (std::size_t i = 0; i < coefficient_count; ++i) {
    const long long coefficient =
        static_cast<long long>(left[i].real() + 0.5);
    const long long current = coefficient + carry;

    small_digits.push_back(
        static_cast<int>(current % fft_base));
    carry = current / fft_base;
  }

  while (carry != 0) {
    small_digits.push_back(
        static_cast<int>(carry % fft_base));
    carry /= fft_base;
  }

  while (small_digits.size() > 1 &&
         small_digits.back() == 0) {
    small_digits.pop_back();
  }

  std::vector<int> result;
  result.reserve(
      (small_digits.size() + fft_parts - 1) / fft_parts);

  for (std::size_t i = 0;
       i < small_digits.size();
       i += fft_parts) {
    int chunk = 0;
    int multiplier = 1;

    for (std::size_t part = 0;
         part < fft_parts && i + part < small_digits.size();
         ++part) {
      chunk += small_digits[i + part] * multiplier;
      multiplier *= fft_base;
    }

    result.push_back(chunk);
  }

  while (result.size() > 1 && result.back() == 0) {
    result.pop_back();
  }

  return result;
}

std::vector<int>
int2048::multiply_abs(const std::vector<int> &lhs,
                      const std::vector<int> &rhs) {
  if (is_zero(lhs) || is_zero(rhs)) {
    return std::vector<int>(1, 0);
  }

  if (lhs.size() < fft_threshold ||
      rhs.size() < fft_threshold) {
    return multiply_abs_naive(lhs, rhs);
  }

  return multiply_abs_fft(lhs, rhs);
}

void int2048::divmod_abs(const std::vector<int> &dividend,
                         const std::vector<int> &divisor,
                         std::vector<int> &quotient,
                         std::vector<int> &remainder) {
  if (compare_abs(dividend, divisor) < 0) {
    quotient.assign(1, 0);
    remainder = dividend;
    return;
  }

  if (divisor.size() == 1) {
    quotient.assign(dividend.size(), 0);

    long long current_remainder = 0;

    for (std::size_t i = dividend.size(); i > 0; --i) {
      const long long current =
          current_remainder * base + dividend[i - 1];

      quotient[i - 1] =
          static_cast<int>(current / divisor[0]);
      current_remainder = current % divisor[0];
    }

    while (quotient.size() > 1 && quotient.back() == 0) {
      quotient.pop_back();
    }

    remainder.assign(1, static_cast<int>(current_remainder));
    return;
  }

  const std::size_t dividend_size = dividend.size();
  const std::size_t divisor_size = divisor.size();
  const int normalization = base / (divisor.back() + 1);

  std::vector<int> normalized_dividend = dividend;
  std::vector<int> normalized_divisor = divisor;

  long long carry = 0;

  for (std::size_t i = 0;
       i < normalized_dividend.size();
       ++i) {
    const long long current =
        static_cast<long long>(normalized_dividend[i]) *
            normalization +
        carry;

    normalized_dividend[i] =
        static_cast<int>(current % base);
    carry = current / base;
  }

  if (carry != 0) {
    normalized_dividend.push_back(static_cast<int>(carry));
  } else {
    normalized_dividend.push_back(0);
  }

  carry = 0;

  for (std::size_t i = 0;
       i < normalized_divisor.size();
       ++i) {
    const long long current =
        static_cast<long long>(normalized_divisor[i]) *
            normalization +
        carry;

    normalized_divisor[i] =
        static_cast<int>(current % base);
    carry = current / base;
  }

  const std::size_t quotient_size =
      dividend_size - divisor_size + 1;

  quotient.assign(quotient_size, 0);

  for (std::size_t position = quotient_size;
       position > 0;
       --position) {
    const std::size_t j = position - 1;

    const long long numerator =
        static_cast<long long>(
            normalized_dividend[j + divisor_size]) *
            base +
        normalized_dividend[j + divisor_size - 1];

    long long estimate =
        numerator / normalized_divisor[divisor_size - 1];

    long long estimate_remainder =
        numerator -
        estimate * normalized_divisor[divisor_size - 1];

    if (estimate >= base) {
      estimate = base - 1;
      estimate_remainder =
          numerator -
          estimate * normalized_divisor[divisor_size - 1];
    }

    while (
        estimate * normalized_divisor[divisor_size - 2] >
        estimate_remainder * base +
            normalized_dividend[j + divisor_size - 2]) {
      --estimate;
      estimate_remainder +=
          normalized_divisor[divisor_size - 1];

      if (estimate_remainder >= base) {
        break;
      }
    }

    long long product_carry = 0;
    int borrow = 0;

    for (std::size_t i = 0; i < divisor_size; ++i) {
      const long long product =
          estimate * normalized_divisor[i] + product_carry;

      product_carry = product / base;
      const int low_product =
          static_cast<int>(product % base);

      int current =
          normalized_dividend[j + i] - low_product - borrow;

      if (current < 0) {
        current += base;
        borrow = 1;
      } else {
        borrow = 0;
      }

      normalized_dividend[j + i] = current;
    }

    long long top =
        static_cast<long long>(
            normalized_dividend[j + divisor_size]) -
        product_carry -
        borrow;

    if (top < 0) {
      --estimate;

      int addition_carry = 0;

      for (std::size_t i = 0; i < divisor_size; ++i) {
        int current =
            normalized_dividend[j + i] +
            normalized_divisor[i] +
            addition_carry;

        if (current >= base) {
          current -= base;
          addition_carry = 1;
        } else {
          addition_carry = 0;
        }

        normalized_dividend[j + i] = current;
      }

      top += addition_carry;
    }

    normalized_dividend[j + divisor_size] =
        static_cast<int>(top);
    quotient[j] = static_cast<int>(estimate);
  }

  while (quotient.size() > 1 && quotient.back() == 0) {
    quotient.pop_back();
  }

  remainder.assign(
      normalized_dividend.begin(),
      normalized_dividend.begin() +
          static_cast<std::vector<int>::difference_type>(
              divisor_size));

  long long division_remainder = 0;

  for (std::size_t i = remainder.size(); i > 0; --i) {
    const long long current =
        division_remainder * base + remainder[i - 1];

    remainder[i - 1] =
        static_cast<int>(current / normalization);
    division_remainder = current % normalization;
  }

  while (remainder.size() > 1 && remainder.back() == 0) {
    remainder.pop_back();
  }
}

void int2048::read(const std::string &value) {
  digits.clear();
  negative = false;

  if (value.empty()) {
    digits.push_back(0);
    return;
  }

  std::string::size_type position = 0;

  if (value[0] == '-' || value[0] == '+') {
    negative = value[0] == '-';
    position = 1;
  }

  while (position < value.size() &&
         value[position] == '0') {
    ++position;
  }

  if (position == value.size()) {
    digits.push_back(0);
    negative = false;
    return;
  }

  std::string::size_type end = value.size();

  while (end > position) {
    std::string::size_type start;

    if (end - position > base_width) {
      start = end - base_width;
    } else {
      start = position;
    }

    int chunk = 0;

    for (std::string::size_type i = start; i < end; ++i) {
      chunk = chunk * 10 + (value[i] - '0');
    }

    digits.push_back(chunk);
    end = start;
  }

  normalize();
}

void int2048::print() {
  std::cout << *this;
}

int2048 &int2048::add(const int2048 &rhs) {
  return *this += rhs;
}

int2048 add(int2048 lhs, const int2048 &rhs) {
  lhs += rhs;
  return lhs;
}

int2048 &int2048::minus(const int2048 &rhs) {
  return *this -= rhs;
}

int2048 minus(int2048 lhs, const int2048 &rhs) {
  lhs -= rhs;
  return lhs;
}

int2048 int2048::operator+() const {
  return *this;
}

int2048 int2048::operator-() const {
  int2048 result(*this);

  if (!is_zero(result.digits)) {
    result.negative = !result.negative;
  }

  return result;
}

int2048 &int2048::operator=(const int2048 &rhs) {
  if (this != &rhs) {
    digits = rhs.digits;
    negative = rhs.negative;
  }

  return *this;
}

int2048 &int2048::operator+=(const int2048 &rhs) {
  if (negative == rhs.negative) {
    add_abs(rhs.digits);
    return *this;
  }

  const int comparison = compare_abs(digits, rhs.digits);

  if (comparison == 0) {
    digits.assign(1, 0);
    negative = false;
    return *this;
  }

  if (comparison > 0) {
    subtract_abs(rhs.digits);
    return *this;
  }

  const std::vector<int> original = digits;
  digits = rhs.digits;
  subtract_abs(original);
  negative = rhs.negative;

  return *this;
}

int2048 operator+(int2048 lhs, const int2048 &rhs) {
  lhs += rhs;
  return lhs;
}

int2048 &int2048::operator-=(const int2048 &rhs) {
  if (negative != rhs.negative) {
    add_abs(rhs.digits);
    return *this;
  }

  const int comparison = compare_abs(digits, rhs.digits);

  if (comparison == 0) {
    digits.assign(1, 0);
    negative = false;
    return *this;
  }

  if (comparison > 0) {
    subtract_abs(rhs.digits);
    return *this;
  }

  const std::vector<int> original = digits;
  digits = rhs.digits;
  subtract_abs(original);
  negative = !negative;

  return *this;
}

int2048 operator-(int2048 lhs, const int2048 &rhs) {
  lhs -= rhs;
  return lhs;
}

int2048 &int2048::operator*=(const int2048 &rhs) {
  const bool result_negative = negative != rhs.negative;

  digits = multiply_abs(digits, rhs.digits);
  negative = result_negative && !is_zero(digits);

  return *this;
}

int2048 operator*(int2048 lhs, const int2048 &rhs) {
  lhs *= rhs;
  return lhs;
}

int2048 &int2048::operator/=(const int2048 &rhs) {
  const bool result_negative = negative != rhs.negative;

  std::vector<int> quotient;
  std::vector<int> remainder;

  divmod_abs(digits, rhs.digits, quotient, remainder);

  if (result_negative && !is_zero(remainder)) {
    int carry = 1;

    for (std::size_t i = 0;
         i < quotient.size() && carry != 0;
         ++i) {
      const int current = quotient[i] + carry;
      quotient[i] = current % base;
      carry = current / base;
    }

    if (carry != 0) {
      quotient.push_back(carry);
    }
  }

  digits = quotient;
  negative = result_negative && !is_zero(digits);
  normalize();

  return *this;
}

int2048 operator/(int2048 lhs, const int2048 &rhs) {
  lhs /= rhs;
  return lhs;
}

int2048 &int2048::operator%=(const int2048 &rhs) {
  const bool dividend_negative = negative;
  const bool divisor_negative = rhs.negative;

  std::vector<int> quotient;
  std::vector<int> remainder;

  divmod_abs(digits, rhs.digits, quotient, remainder);

  if (is_zero(remainder)) {
    digits.assign(1, 0);
    negative = false;
    return *this;
  }

  if (dividend_negative == divisor_negative) {
    digits = remainder;
    negative = dividend_negative;
    normalize();
    return *this;
  }

  digits = rhs.digits;
  subtract_abs(remainder);
  negative = divisor_negative;
  normalize();

  return *this;
}

int2048 operator%(int2048 lhs, const int2048 &rhs) {
  lhs %= rhs;
  return lhs;
}

std::istream &operator>>(std::istream &input,
                         int2048 &value) {
  std::string text;
  input >> text;

  if (input) {
    value.read(text);
  }

  return input;
}

std::ostream &operator<<(std::ostream &output,
                         const int2048 &value) {
  if (value.negative) {
    output.put('-');
  }

  output << value.digits.back();

  for (std::size_t i = value.digits.size() - 1;
       i > 0;
       --i) {
    const int chunk = value.digits[i - 1];

    for (int divisor = int2048::base / 10;
         divisor != 0;
         divisor /= 10) {
      output.put(static_cast<char>(
          '0' + (chunk / divisor) % 10));
    }
  }

  return output;
}

bool operator==(const int2048 &lhs,
                const int2048 &rhs) {
  return lhs.negative == rhs.negative &&
         lhs.digits == rhs.digits;
}

bool operator!=(const int2048 &lhs,
                const int2048 &rhs) {
  return !(lhs == rhs);
}

bool operator<(const int2048 &lhs,
               const int2048 &rhs) {
  if (lhs.negative != rhs.negative) {
    return lhs.negative;
  }

  const int comparison =
      int2048::compare_abs(lhs.digits, rhs.digits);

  if (lhs.negative) {
    return comparison > 0;
  }

  return comparison < 0;
}

bool operator>(const int2048 &lhs,
               const int2048 &rhs) {
  return rhs < lhs;
}

bool operator<=(const int2048 &lhs,
                const int2048 &rhs) {
  return !(rhs < lhs);
}

bool operator>=(const int2048 &lhs,
                const int2048 &rhs) {
  return !(lhs < rhs);
}

} // namespace sjtu

#endif
