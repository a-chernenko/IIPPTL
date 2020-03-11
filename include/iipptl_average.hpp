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

#ifndef IIPPTL_AVERAGE_HPP
#define IIPPTL_AVERAGE_HPP

#include <vector>

#include "iipptl_vector.hpp"

namespace IntelIppTL {

template <typename IppType>
class CIppAverage {
 public:
  using ValueType = IppType;
  using IppVector = CIppVector<IppType>;
  using SizeType = typename IppVector::SizeType;
  CIppAverage() = default;
  CIppAverage(SizeType vectorSize, SizeType averageSize = 1)
      : m_AverageVector{vectorSize},
        m_RingBuffer(averageSize, IppVector(vectorSize)) {
    if (vectorSize <= 0 || averageSize <= 0) {
      throw std::invalid_argument("CIppAverage: invalid ctor argument");
    }
    reset();
  }
  CIppAverage(CIppAverage const &) = default;
  CIppAverage(CIppAverage &&) noexcept = default;
  CIppAverage &operator=(CIppAverage const &) = default;
  CIppAverage &operator=(CIppAverage &&) noexcept = default;
  virtual ~CIppAverage() noexcept = default;
  CIppAverage &operator+=(IppVector const &rhs) {
    add(rhs);
    return *this;
  }
  void add(IppVector const &vector) {
    if (m_NotFirstCycle) {
      m_AverageVector -= m_RingBuffer[m_AverageCount];
    }
    m_RingBuffer[m_AverageCount] = vector;
    m_AverageVector += m_RingBuffer[m_AverageCount];
    ++m_AverageCount;
    if (m_AverageCount == SizeType(m_RingBuffer.size())) {
      m_AverageCount = 0;
      m_NotFirstCycle = true;
    }
  }
  void get(IppVector &vector) const {
    data(vector);
    normalize(vector);
  }
  inline void normalize(IppVector &vector) const {
    auto norm{(m_NotFirstCycle) ? SizeType(m_RingBuffer.size())
                                : m_AverageCount};
    vector /= norm;
  }
  inline void data(IppVector &vector) const { vector = m_AverageVector; }
  inline IppVector const &data() const noexcept { return m_AverageVector; }
  inline IppVector &data() noexcept { return m_AverageVector; }
  void reset() {
    m_AverageVector.clear();
    for (auto &vector : m_RingBuffer) {
      vector = 0;
    }
    m_AverageCount = 0;
    m_NotFirstCycle = false;
  }
  inline SizeType size() const noexcept {
    return SizeType(m_RingBuffer.size());
  }
  void reinit(SizeType vectorSize, SizeType averageSize = 1) {
    if (vectorSize <= 0 || averageSize <= 0) {
      throw std::invalid_argument("CIppAverage: invalid reinit argument");
    }
    if (SizeType(m_RingBuffer.size()) != averageSize ||
        m_AverageVector.size() != vectorSize) {
      auto tmp{CIppAverage(vectorSize, averageSize)};
      std::swap(*this, tmp);
      reset();
    }
  }
  void resize(SizeType averageSize) {
    if (averageSize <= 0) {
      throw std::invalid_argument("CIppAverage: invalid resize argument");
    }
    if (SizeType(m_RingBuffer.size()) != averageSize) {
      m_RingBuffer.resize(averageSize, IppVector(m_AverageVector.size()));
      reset();
      // TODO: оптимизировать, вместо очистки сделать копирование.
    }
  }

 private:
  SizeType m_AverageCount{};
  bool m_NotFirstCycle{false};
  IppVector m_AverageVector{};
  std::vector<IppVector> m_RingBuffer{};
};

using CIppAverage64f = CIppAverage<Ipp64f>;
using CIppAverage64fc = CIppAverage<Ipp64fc>;
using CIppAverage32f = CIppAverage<Ipp32f>;
using CIppAverage32fc = CIppAverage<Ipp32fc>;

}  // namespace IntelIppTL

#endif  // IIPPTL_AVERAGE_HPP
