/*

   Copyright 2019 Alexander Chernenko (achernenko@mail.ru)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#ifndef IIPPTL_DELAY_HPP
#define IIPPTL_DELAY_HPP

#include "iipptl_vector.hpp"

namespace IntelIppTL {

template <typename IppType>
class CIppDelay {
 public:
  using ValueType = IppType;
  using IppVector = CIppVector<IppType>;
  using SizeType = typename IppVector::SizeType;
  CIppDelay() = default;
  CIppDelay(SizeType vectorSize, SizeType delaySize = 0)
      : m_DelayVectorSize{vectorSize},
        m_RingBuffer((delaySize + SizeType(1)), IppVector(vectorSize)) {
    if (vectorSize <= 0 || delaySize < 0) {
      throw std::invalid_argument("CIppDelay: invalid ctor argument");
    }
    reset();
  }
  CIppDelay(CIppDelay const &) = default;
  CIppDelay(CIppDelay &&) noexcept = default;
  CIppDelay &operator=(CIppDelay const &) = default;
  CIppDelay &operator=(CIppDelay &&) noexcept = default;
  virtual ~CIppDelay() noexcept = default;
  void reset() noexcept {
    for (auto &vector : m_RingBuffer) {
      vector = 0;
    }
    m_DelayCount = 0;
  }
  inline IppVector const &data() const noexcept {
    return m_RingBuffer[m_DelayCount];
  }
  inline IppVector &data() noexcept { return m_RingBuffer[m_DelayCount]; }
  void push() noexcept {
    ++m_DelayCount;
    if (m_DelayCount == SizeType(m_RingBuffer.size())) {
      m_DelayCount = 0;
    }
  }
  void push(IppVector const &vector) noexcept {
    auto &ref{data()};
    ref = vector;
    push();
  }
  inline void get(IppVector &vector) noexcept { vector = data(); }
  inline SizeType size() const noexcept {
    return SizeType(m_RingBuffer.size());
  }
  void reinit(SizeType vectorSize, SizeType delaySize = 0) {
    if (vectorSize <= 0 || delaySize < 0) {
      throw std::invalid_argument("CIppDelay: invalid reinit argument");
    }
    delaySize += 1;
    if (SizeType(m_RingBuffer.size()) != delaySize ||
        m_DelayVectorSize != delaySize) {
      auto tmp{CIppDelay(vectorSize, delaySize)};
      std::swap(*this, tmp);
      reset();
    }
  }
  void resize(SizeType delaySize) {
    if (delaySize < 0) {
      throw std::invalid_argument("CIppDelay: invalid resize argument");
    }
    delaySize += 1;
    if (SizeType(m_RingBuffer.size()) != delaySize) {
      m_RingBuffer.resize(delaySize, IppVector(m_DelayVectorSize));
      reset();
      // TODO: оптимизировать, вместо очистки сделать копирование.
    }
  }
  CIppDelay &operator<<(IppVector const &rhs) {
    push(rhs);
    return *this;
  }
  friend void operator>>(CIppDelay &lhs, IppVector &rhs) { lhs.get(rhs); }

 private:
  SizeType m_DelayCount{};
  SizeType m_DelayVectorSize{};
  std::vector<IppVector> m_RingBuffer{};
};

using CIppDelay64f = CIppDelay<Ipp64f>;
using CIppDelay64fc = CIppDelay<Ipp64fc>;
using CIppDelay32f = CIppDelay<Ipp32f>;
using CIppDelay32fc = CIppDelay<Ipp32fc>;

}  // namespace IntelIppTL

#endif  // IIPPTL_DELAY_HPP
