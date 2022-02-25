#include <cstring>
#include <limits>
#include <iostream>
#include <ostream>

template <unsigned NumSignificandBits = 11>
class BFloat {
public:
  /**
   * @brief default constructor does not initialize the value
   */
  BFloat() : value_(0) {}

  explicit BFloat(float input) : value_(bfloatFromFloat(input)) {}

  explicit BFloat(unsigned input) : value_(bfloatFromFloat(static_cast<float>(input))) {}

  explicit BFloat(std::int32_t input) : value_(bfloatFromFloat(static_cast<float>(input))) {}

  std::uint32_t getBits() {
    std::uint32_t bits;
    std::memcpy(&bits, &value_, sizeof(bits));
    return bits;
  }

  ~BFloat() = default;

  BFloat(const BFloat &other) { this->value_ = other.value_; };

  BFloat &operator=(const BFloat &other) {
    this->value_ = other.value_;
    return *this;
  }

  BFloat &operator=(BFloat &&other) noexcept = default;
  BFloat(BFloat &&other) noexcept = default;

  BFloat &operator=(float other) {
    *this = BFloat(other);
    return *this;
  }

  bool operator==(const BFloat &other) const { return this->value_ == other.value_; }

  bool operator!=(const BFloat &other) const { return !(*this == other); }

  bool operator<(const BFloat &other) const { return this->value_ < other.value_; }

  bool operator<=(const BFloat &other) const { return this->value_ <= other.value_; }

  bool operator>(const BFloat &other) const { return this->value_ > other.value; }

  bool operator>=(const BFloat &other) const { return this->value_ >= other.value; }

  BFloat operator+(const BFloat &other) const { return BFloat(this->value_ + other.value_); }

  BFloat &operator+=(const BFloat &other) {
    *this = (*this) + other;
    return *this;
  }

  friend std::ostream& operator<<(std::ostream& os, const BFloat& input) {
      os << input.value_;
      return os;
  }

private:
  float value_;

  std::uint32_t flushBits(std::uint32_t input) { return input & flushMask_; }

  // Rounds the input to the nearest bfloat representable
  float bfloatFromFloat(float other) {
    std::uint32_t bits;
    std::memcpy(&bits, &other, sizeof(bits));

    std::uint32_t bitsNext = bits + halfValue_;

    bool halfwayAndEven = (bits & halfwayEvenMask_) == halfValue_;
    bool signChange = ((bits ^ bitsNext) & (1 << 31)) > 0;

    // If it's even and half way then choose the even value
    // Also, if the rounding causes sign change do not use the next bigger number
    if (halfwayAndEven || signChange) {
      bitsNext = bits;
    }

    std::uint32_t resultBits = flushBits(bitsNext);

    float result;
    std::memcpy(&result, &resultBits, sizeof(result));

    return result;
  }

  static constexpr unsigned maxSignificandBits_ = 23; // FP32
  static constexpr unsigned numSignBits_ = 1;
  static constexpr unsigned numExponentBits_ = 8;

  static constexpr unsigned halfValue_ = 1 << (maxSignificandBits_ - NumSignificandBits - 1);
  static constexpr unsigned numFlushBits_ =
      std::numeric_limits<std::uint32_t>::digits - (numSignBits_ + numExponentBits_ + NumSignificandBits);
  static constexpr unsigned flushMask_ = ~((1 << (numFlushBits_)) - 1);
  static constexpr unsigned halfwayEvenMask_ = (1 << (numFlushBits_ + 1)) - 1;
};


int main(){
    BFloat<15> bf(2.f);
    BFloat<15> bf1(2.00003051758f);
    BFloat<15> bf2(2.00009155273f);

    std::cout << bf << std::endl;
    std::cout << bf1 << std::endl;
    std::cout << bf2 << std::endl;
    return 0;
}
