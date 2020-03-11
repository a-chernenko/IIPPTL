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

#ifndef IIPPTL_QUEUE_HPP
#define IIPPTL_QUEUE_HPP

#include "iipptl_vector.hpp"

namespace IntelIppTL {

template <typename IppType>
class CIppQueue {
 public:
  using ValueType = IppType;
  using IppVector = CIppVector<IppType>;
  using SizeType = typename IppVector::SizeType;
  CIppQueue() = default;
  CIppQueue(SizeType vectorSize, SizeType queueSize = 1)
      : m_QueueVectorSize{vectorSize},
        m_RingBuffer(queueSize, IppVector(vectorSize)) {
    if (vectorSize <= 0 || queueSize <= 0) {
      throw std::invalid_argument("CIppQueue: invalid ctor argument");
    }
    reset();
  }
  CIppQueue(CIppQueue const &) = default;
  CIppQueue(CIppQueue &&) noexcept = default;
  CIppQueue &operator=(CIppQueue const &) = default;
  CIppQueue &operator=(CIppQueue &&) noexcept = default;
  virtual ~CIppQueue() noexcept = default;
  void reset() noexcept {
    for (auto &vector : m_RingBuffer) {
      vector = 0;
    }
    m_QueueCount = 0;
  }
  inline IppVector const &front() const noexcept {
    return m_RingBuffer[m_QueueCount];
  }
  inline IppVector &front() noexcept { return m_RingBuffer[m_QueueCount]; }
  void push() noexcept {
    if (++m_QueueHead == m_RingBuffer.size()) {
      m_QueueHead = 0;
    }
    if (m_QueueCount < m_RingBuffer.size()) {
      ++m_QueueCount;
    }
  }
  void push(IppVector const &vector) noexcept {
    auto &ref{front()};
    ref = vector;
    push();
  }
  void pop() noexcept {
    if (++m_QueueTail == m_RingBuffer.size()) {
      m_QueueTail = 0;
    }
    if (m_QueueCount > 0) {
      --m_QueueCount;
    }
  }
  void pop(IppVector &vector) noexcept {
    vector = back();
    pop();
  }
  inline IppVector &back() noexcept { return m_RingBuffer[m_QueueTail]; }
  inline IppVector const &back() const noexcept {
    return m_RingBuffer[m_QueueTail];
  }
  inline bool empty() const noexcept { return (m_QueueCount == 0); }
  inline bool full() const noexcept {
    return (m_QueueCount == m_RingBuffer.size());
  }
  inline SizeType size() const noexcept { return m_QueueCount; }
  inline SizeType max_size() const noexcept { return int(m_RingBuffer.size()); }
  void reinit(SizeType vectorSize, SizeType queueSize) {
    if (vectorSize <= 0 || queueSize <= 0) {
      throw std::invalid_argument("CIppQueue: invalid reinit argument");
    }
    if (m_RingBuffer.size() != queueSize || m_QueueVectorSize != vectorSize) {
      auto tmp{CIppQueue(vectorSize, queueSize)};
      std::swap(*this, tmp);
      reset();
      // TODO: оптимизировать, вместо очистки сделать копирование.
    }
  }
  void resize(SizeType queueSize) {
    if (queueSize <= 0) {
      throw std::invalid_argument("CIppQueue: invalid resize argument");
    }
    if (m_RingBuffer.size() != queueSize) {
      m_RingBuffer.resize(queueSize, IppVector(m_QueueVectorSize));
      reset();
    }
  }

 private:
  SizeType m_QueueHead{};
  SizeType m_QueueTail{};
  SizeType m_QueueCount{};
  SizeType m_QueueVectorSize{};
  std::vector<IppVector> m_RingBuffer{};
};

using CIppQueue64f = CIppQueue<Ipp64f>;
using CIppQueue64fc = CIppQueue<Ipp64fc>;
using CIppQueue32f = CIppQueue<Ipp32f>;
using CIppQueue32fc = CIppQueue<Ipp32fc>;

}  // namespace IntelIppTL

#endif  // IIPPTL_QUEUE_HPP
