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

#ifndef IIPPTL_FFT_PARAM_HPP
#define IIPPTL_FFT_PARAM_HPP

#include <functional>
#include <type_traits>

#include "iipptl_vector.hpp"

namespace IntelIppTL {

enum class IppFFTFlags : int {
  ForwardTransformByN = IPP_FFT_DIV_FWD_BY_N,
  InverseTransformByN = IPP_FFT_DIV_INV_BY_N,
  TransformBySqrtN = IPP_FFT_DIV_BY_SQRTN,
  TransformWithoutAny = IPP_FFT_NODIV_BY_ANY
};

struct CIppFFTOrder {
  static constexpr int calculate(int value) {
    int order{};
    --value;
    while (value != 0) {
      value >>= 1;
      ++order;
    }
    return order;
  }
};

namespace Detail {

template <typename IppType>
class CIppFFTParam {
 public:
  using ValueType = IppType;
  using SizeType = int;
  CIppFFTParam() = default;
  CIppFFTParam(SizeType order,
               IppFFTFlags flag = IppFFTFlags::TransformWithoutAny)
      : m_Hint{ippAlgHintNone} {
    update(order, flag);
  }
  CIppFFTParam(CIppFFTParam const &other) { operator=(other); }
  CIppFFTParam(CIppFFTParam &&) noexcept = default;
  CIppFFTParam &operator=(CIppFFTParam const &other) {
    if (this != &other) {
      m_SpecSize = other.m_SpecSize;
      m_InitSize = other.m_InitSize;
      m_BufferSize = other.m_BufferSize;
      m_Order = other.m_Order;
      m_Flag = other.m_Flag;
    }
    return *this;
  }
  CIppFFTParam &operator=(CIppFFTParam &&) noexcept = default;
  virtual ~CIppFFTParam() noexcept = default;
  void update(SizeType order, IppFFTFlags flag) {
    std::function<IppStatus(int, int, IppHintAlgorithm, int *, int *, int *)>
        ippsFFTGetSize{};
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsFFTGetSize = ippsFFTGetSize_C_64f;
    } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsFFTGetSize = ippsFFTGetSize_C_64fc;
    } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsFFTGetSize = ippsFFTGetSize_C_32f;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsFFTGetSize = ippsFFTGetSize_C_32fc;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsFFTGetSize(order, static_cast<int>(flag), m_Hint,
                                    &m_SpecSize, &m_InitSize, &m_BufferSize));
    m_Order = order;
    m_Flag = flag;
  }
  inline SizeType specSize() const noexcept { return m_SpecSize; }
  inline SizeType initSize() const noexcept { return m_InitSize; }
  inline SizeType bufferSize() const noexcept { return m_BufferSize; }
  inline int order() const noexcept { return m_Order; }
  inline IppFFTFlags flag() const noexcept { return m_Flag; }
  inline IppHintAlgorithm hint() const noexcept { return m_Hint; }

 private:
  SizeType m_SpecSize{};
  SizeType m_InitSize{};
  SizeType m_BufferSize{};
  SizeType m_Order{};
  IppFFTFlags m_Flag{};
  IppHintAlgorithm const m_Hint{};
};

using CIppFFTParam64f = CIppFFTParam<Ipp64f>;
using CIppFFTParam64fc = CIppFFTParam<Ipp64fc>;
using CIppFFTParam32f = CIppFFTParam<Ipp32f>;
using CIppFFTParam32fc = CIppFFTParam<Ipp32fc>;

}  // namespace Detail

}  // namespace IntelIppTL

#endif  // IIPPTL_FFT_PARAM_HPP
