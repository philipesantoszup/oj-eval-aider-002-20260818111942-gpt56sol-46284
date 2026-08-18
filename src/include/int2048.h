#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

// Do not use any header files other than the following
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

// Do not use "using namespace std;"

namespace sjtu {

class int2048 {
private:
  static constexpr int base = 1000000000;
  static constexpr int base_width = 9;
  static constexpr int fft_base = 1000;
  static constexpr std::size_t fft_parts = 3;
  static constexpr std::size_t fft_threshold = 64;
  static constexpr std::size_t division_threshold = 64;

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

  static void divmod_abs_knuth(
      const std::vector<int> &dividend,
      const std::vector<int> &divisor,
      std::vector<int> &quotient,
      std::vector<int> &remainder);

  static void divmod_abs_burnikel(
      const std::vector<int> &dividend,
      const std::vector<int> &divisor,
      std::vector<int> &quotient,
      std::vector<int> &remainder);

  static void divide_2n_1n(
      const std::vector<int> &dividend,
      const std::vector<int> &divisor,
      std::size_t block_size,
      std::vector<int> &quotient,
      std::vector<int> &remainder);

  static void divide_3n_2n(
      const std::vector<int> &dividend,
      const std::vector<int> &divisor,
      std::size_t half_size,
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

} // namespace sjtu

#endif
