#include "int2048.h"

namespace {

constexpr int vector_base = 1000000000;

void trim_vector(std::vector<int> &value) {
  while (value.size() > 1 && value.back() == 0) {
    value.pop_back();
  }

  if (value.empty()) {
    value.push_back(0);
  }
}

bool vector_is_zero(const std::vector<int> &value) {
  return value.size() == 1 && value[0] == 0;
}

int compare_vectors(const std::vector<int> &lhs,
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

std::vector<int> slice_vector(const std::vector<int> &value,
                              std::size_t begin,
                              std::size_t count) {
  if (begin >= value.size() || count == 0) {
    return std::vector<int>(1, 0);
  }

  std::size_t end = begin + count;

  if (end < begin || end > value.size()) {
    end = value.size();
  }

  std::vector<int> result(
      value.begin() +
          static_cast<std::vector<int>::difference_type>(begin),
      value.begin() +
          static_cast<std::vector<int>::difference_type>(end));

  trim_vector(result);
  return result;
}

std::vector<int> shift_vector(const std::vector<int> &value,
                              std::size_t amount) {
  if (vector_is_zero(value)) {
    return std::vector<int>(1, 0);
  }

  std::vector<int> result(amount, 0);
  result.insert(result.end(), value.begin(), value.end());
  return result;
}

std::vector<int> add_vectors(const std::vector<int> &lhs,
                             const std::vector<int> &rhs) {
  const std::size_t result_size =
      lhs.size() > rhs.size() ? lhs.size() : rhs.size();

  std::vector<int> result(result_size + 1, 0);
  int carry = 0;

  for (std::size_t i = 0; i < result_size; ++i) {
    long long current = carry;

    if (i < lhs.size()) {
      current += lhs[i];
    }

    if (i < rhs.size()) {
      current += rhs[i];
    }

    result[i] = static_cast<int>(current % vector_base);
    carry = static_cast<int>(current / vector_base);
  }

  result[result_size] = carry;
  trim_vector(result);
  return result;
}

std::vector<int> subtract_vectors(
    const std::vector<int> &lhs,
    const std::vector<int> &rhs) {
  std::vector<int> result = lhs;
  int borrow = 0;

  for (std::size_t i = 0; i < result.size(); ++i) {
    long long current =
        static_cast<long long>(result[i]) - borrow;

    if (i < rhs.size()) {
      current -= rhs[i];
    }

    if (current < 0) {
      current += vector_base;
      borrow = 1;
    } else {
      borrow = 0;
    }

    result[i] = static_cast<int>(current);
  }

  trim_vector(result);
  return result;
}

void increment_vector(std::vector<int> &value) {
  int carry = 1;

  for (std::size_t i = 0;
       i < value.size() && carry != 0;
       ++i) {
    const int current = value[i] + carry;
    value[i] = current % vector_base;
    carry = current / vector_base;
  }

  if (carry != 0) {
    value.push_back(carry);
  }
}

void decrement_vector(std::vector<int> &value) {
  std::size_t position = 0;

  while (value[position] == 0) {
    value[position] = vector_base - 1;
    ++position;
  }

  --value[position];
  trim_vector(value);
}

void multiply_vector_small(std::vector<int> &value,
                           int multiplier) {
  long long carry = 0;

  for (std::size_t i = 0; i < value.size(); ++i) {
    const long long current =
        static_cast<long long>(value[i]) * multiplier + carry;

    value[i] = static_cast<int>(current % vector_base);
    carry = current / vector_base;
  }

  if (carry != 0) {
    value.push_back(static_cast<int>(carry));
  }

  trim_vector(value);
}

void divide_vector_small(std::vector<int> &value,
                         int divisor) {
  long long remainder = 0;

  for (std::size_t i = value.size(); i > 0; --i) {
    const long long current =
        remainder * vector_base + value[i - 1];

    value[i - 1] = static_cast<int>(current / divisor);
    remainder = current % divisor;
  }

  trim_vector(value);
}

std::vector<int> maximum_block(std::size_t size) {
  return std::vector<int>(size, vector_base - 1);
}

} // namespace

namespace sjtu {

int2048::int2048() : digits(1, 0), negative(false) {}

int2048::int2048(long long value)
    : digits(), negative(value < 0) {
  unsigned long long magnitude;

  if (value < 0) {
    magnitude =
        static_cast<unsigned long long>(-(value + 1));
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
  return compare_vectors(lhs, rhs);
}

void int2048::add_abs(const std::vector<int> &rhs) {
  digits = add_vectors(digits, rhs);
}

void int2048::subtract_abs(const std::vector<int> &rhs) {
  digits = subtract_vectors(digits, rhs);
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

  for (std::size_t length = 2;
       length <= size;
       length <<= 1) {
    const double angle =
        2.0 * pi / static_cast<double>(length) *
        (inverse ? -1.0 : 1.0);

    const std::complex<double> root(std::cos(angle),
                                    std::sin(angle));

    for (std::size_t start = 0;
         start < size;
         start += length) {
      std::complex<double> factor(1.0, 0.0);
      const std::size_t half = length >> 1;

      for (std::size_t offset = 0;
           offset < half;
           ++offset) {
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

  trim_vector(result);
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
      right_parts[i * fft_parts + part] =
          chunk % fft_base;
      chunk /= fft_base;
    }
  }

  trim_vector(left_parts);
  trim_vector(right_parts);

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

  trim_vector(small_digits);

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

  trim_vector(result);
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

void int2048::divmod_abs_knuth(
    const std::vector<int> &dividend,
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

    trim_vector(quotient);
    remainder.assign(1, static_cast<int>(current_remainder));
    return;
  }

  const std::size_t dividend_size = dividend.size();
  const std::size_t divisor_size = divisor.size();
  const int normalization = base / (divisor.back() + 1);

  std::vector<int> normalized_dividend = dividend;
  std::vector<int> normalized_divisor = divisor;

  multiply_vector_small(normalized_dividend, normalization);
  multiply_vector_small(normalized_divisor, normalization);

  if (normalized_dividend.size() == dividend_size) {
    normalized_dividend.push_back(0);
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

  trim_vector(quotient);

  remainder.assign(
      normalized_dividend.begin(),
      normalized_dividend.begin() +
          static_cast<std::vector<int>::difference_type>(
              divisor_size));

  divide_vector_small(remainder, normalization);
}

void int2048::divide_3n_2n(
    const std::vector<int> &dividend,
    const std::vector<int> &divisor,
    std::size_t half_size,
    std::vector<int> &quotient,
    std::vector<int> &remainder) {
  const std::vector<int> low =
      slice_vector(dividend, 0, half_size);
  const std::vector<int> middle =
      slice_vector(dividend, half_size, half_size);
  const std::vector<int> high =
      slice_vector(dividend, half_size * 2, half_size);

  const std::vector<int> divisor_high =
      slice_vector(divisor, half_size, half_size);

  const std::vector<int> high_pair =
      add_vectors(middle, shift_vector(high, half_size));

  if (compare_vectors(high, divisor_high) < 0) {
    std::vector<int> ignored_remainder;

    divide_2n_1n(high_pair, divisor_high, half_size,
                  quotient, ignored_remainder);
  } else {
    quotient = maximum_block(half_size);
  }

  std::vector<int> product =
      multiply_abs(quotient, divisor);

  while (compare_vectors(product, dividend) > 0) {
    decrement_vector(quotient);
    product = subtract_vectors(product, divisor);
  }

  remainder = subtract_vectors(dividend, product);

  while (compare_vectors(remainder, divisor) >= 0) {
    remainder = subtract_vectors(remainder, divisor);
    increment_vector(quotient);
  }

  trim_vector(quotient);
  trim_vector(remainder);
}

void int2048::divide_2n_1n(
    const std::vector<int> &dividend,
    const std::vector<int> &divisor,
    std::size_t block_size,
    std::vector<int> &quotient,
    std::vector<int> &remainder) {
  if (block_size <= division_threshold) {
    divmod_abs_knuth(dividend, divisor, quotient, remainder);
    return;
  }

  const std::size_t half_size = block_size >> 1;

  const std::vector<int> low =
      slice_vector(dividend, 0, half_size);
  const std::vector<int> upper =
      slice_vector(dividend, half_size, half_size * 3);

  std::vector<int> quotient_high;
  std::vector<int> upper_remainder;

  divide_3n_2n(upper, divisor, half_size,
                quotient_high, upper_remainder);

  const std::vector<int> lower_dividend =
      add_vectors(low,
                  shift_vector(upper_remainder, half_size));

  std::vector<int> quotient_low;

  divide_3n_2n(lower_dividend, divisor, half_size,
                quotient_low, remainder);

  quotient =
      add_vectors(quotient_low,
                  shift_vector(quotient_high, half_size));

  trim_vector(quotient);
  trim_vector(remainder);
}

void int2048::divmod_abs_burnikel(
    const std::vector<int> &dividend,
    const std::vector<int> &divisor,
    std::vector<int> &quotient,
    std::vector<int> &remainder) {
  const int normalization = base / (divisor.back() + 1);

  std::vector<int> normalized_dividend = dividend;
  std::vector<int> normalized_divisor = divisor;

  multiply_vector_small(normalized_dividend, normalization);
  multiply_vector_small(normalized_divisor, normalization);

  std::size_t block_size = 1;

  while (block_size < normalized_divisor.size()) {
    block_size <<= 1;
  }

  const std::size_t padding =
      block_size - normalized_divisor.size();

  normalized_divisor =
      shift_vector(normalized_divisor, padding);
  normalized_dividend =
      shift_vector(normalized_dividend, padding);

  const std::size_t block_count =
      (normalized_dividend.size() + block_size - 1) /
      block_size;

  quotient.assign(block_count * block_size, 0);
  remainder.assign(1, 0);

  for (std::size_t block = block_count;
       block > 0;
       --block) {
    const std::size_t block_index = block - 1;

    const std::vector<int> current_block =
        slice_vector(normalized_dividend,
                     block_index * block_size,
                     block_size);

    const std::vector<int> current_dividend =
        add_vectors(current_block,
                    shift_vector(remainder, block_size));

    std::vector<int> quotient_block;

    divide_2n_1n(current_dividend,
                  normalized_divisor,
                  block_size,
                  quotient_block,
                  remainder);

    const std::size_t offset = block_index * block_size;

    if (offset + quotient_block.size() > quotient.size()) {
      quotient.resize(offset + quotient_block.size(), 0);
    }

    for (std::size_t i = 0;
         i < quotient_block.size();
         ++i) {
      quotient[offset + i] = quotient_block[i];
    }
  }

  trim_vector(quotient);

  if (padding != 0) {
    remainder =
        slice_vector(remainder,
                     padding,
                     remainder.size());
  }

  divide_vector_small(remainder, normalization);
  trim_vector(remainder);
}

void int2048::divmod_abs(
    const std::vector<int> &dividend,
    const std::vector<int> &divisor,
    std::vector<int> &quotient,
    std::vector<int> &remainder) {
  if (compare_abs(dividend, divisor) < 0) {
    quotient.assign(1, 0);
    remainder = dividend;
    return;
  }

  const std::size_t quotient_size =
      dividend.size() - divisor.size() + 1;

  if (divisor.size() <= division_threshold ||
      quotient_size <= division_threshold) {
    divmod_abs_knuth(dividend, divisor,
                     quotient, remainder);
    return;
  }

  divmod_abs_burnikel(dividend, divisor,
                      quotient, remainder);
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
    increment_vector(quotient);
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
