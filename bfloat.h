#include <cstdint>
#include <limits>
#include <ostream>
#include <cstring>

template <unsigned NumSignificandBits = 11>
class BFloat {
public:
  /**
   * @brief default constructor does not initialize the value
   */
  BFloat() : value_(0) {}

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

    bool halfwayAndEven = (bits & 0x1FFFF) == halfValue_; // FIX THIS
    bool signChange = ((bits ^ bitsNext) & (1 << 31)) > 0;

    if (halfwayAndEven || signChange) {
      bitsNext = bits;
    }

    std::uint32_t resultBits = bitsNext >> numFlushBits_; // FIX THIS
    
    float result;
    std::memcpy(&result, &resultBits, sizeof(result));

    return result;
  }

  static constexpr unsigned maxSignificandBits_ = 23; // FP32
  static constexpr unsigned numSignBits_ = 1;
  static constexpr unsigned numExponentBits_ = 8;

  static constexpr unsigned halfValue_ = 1 << (maxSignificandBits_ - NumSignificandBits - 1);
  static constexpr unsigned numFlushBits_ = std::numeric_limits<float>::digits - numSignBits_ + numExponentBits_ + NumSignificandBits;
};

int main(){
    return 0;
}
