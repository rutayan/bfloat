#include <cstdint>
#include <limits>
#include <iostream>
#include <cstring>

template <unsigned NumSignificandBits = 11>
class BFloat {
public:
  /**
   * @brief default constructor does not initialize the value
   */
  BFloat() : value_(0) {}

  BFloat(float input): value_(bfloatFromFloat(input)) {}

  std::uint32_t getBits() {
      std::uint32_t bits;
      std::memcpy(&bits, &value_, sizeof(bits));
      return bits;
  }

private:
  float value_;

  // Rounds the input to the nearest bfloat representable
  float bfloatFromFloat(float other) {
    std::uint32_t bits;
    std::uint32_t bitsNext = bits + halfValue_;

    bool halfwayAndEven = (bits & halfwayEvenMask_) == halfValue_;
    bool signChange = ((bits ^ bitsNext) & (1 << 31)) > 0;

    if (halfwayAndEven || signChange) {
      bitsNext = bits;
    }

    std::uint32_t resultBits = bitsNext >> numFlushBits_;
    
    float result;
    std::memcpy(&result, &resultBits, sizeof(result));

    return result;
  }

  static constexpr unsigned maxSignificandBits_ = 23; // FP32
  static constexpr unsigned numSignBits_ = 1;
  static constexpr unsigned numExponentBits_ = 8;

  static constexpr unsigned halfValue_ = 1 << (maxSignificandBits_ - NumSignificandBits - 1);
  static constexpr unsigned numFlushBits_ = std::numeric_limits<std::uint32_t>::digits - (numSignBits_ + numExponentBits_ + NumSignificandBits);
  static constexpr unsigned halfwayEvenMask_ = (1 << (numFlushBits_ + 1)) - 1;
};

int main(){
    BFloat<19> bf(2.f);

    return 0;
}
