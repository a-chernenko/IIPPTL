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

#ifndef IIPPTL_VECTOR_HPP
#define IIPPTL_VECTOR_HPP

#include <limits>
#include <memory>
#include <stdexcept>

#include "iipptl_allocator.hpp"

namespace IntelIppTL {

namespace Private {

template <typename IppTypeA, typename IppTypeB>
inline void ippVectorIsEqual(IppTypeA const &veca, IppTypeB const &vecb) {
  if (veca.size() != vecb.size()) {
    throw std::invalid_argument("vectors size is not equal");
  }
}

inline void ippCheckStatus(IppStatus status, char const *const file, int line,
                           char const *const func) {
  if (status != ippStsNoErr) {
    std::string message{"IPP ERROR #"};
    message += std::to_string(status);
    message += ". ";
    message += ippGetStatusString(status);
    message += ".\t";
    if (file) {
      message += file;
      message += ", ";
    }
    message += std::to_string(line);
    if (func) {
      message += ", ";
      message += func;
    }
    throw std::runtime_error(message);
  }
}

}  // namespace Private

#define IPP_CHECK_STATUS(status) \
  Private::ippCheckStatus(status, __FILE__, __LINE__, __FUNCTION__)

template <typename IppType>
class CIppVector {
 public:
  using ElementType = IppType;
  using SizeType = CIppAllocator::SizeType;
  using CustomDeleter = std::function<decltype(CIppAllocator::deallocate)>;
  using UniquePtr = std::unique_ptr<ElementType[], CustomDeleter>;
  using Pointer = IppType *;
  CIppVector(SizeType size = 0)
      : m_VectorSize{size},
        m_VectorCapacity{size},
        m_Vector{CIppAllocator::allocate<ElementType>(size),
                 CIppAllocator::deallocate} {
    clear();
  }
  CIppVector(SizeType size, ElementType value) : CIppVector(size) {
    assign(elementTypeCast(value));
  }
  CIppVector(CIppVector const &other) { operator=(other); }
  CIppVector(CIppVector &&other) noexcept { operator=(std::move(other)); }
  virtual ~CIppVector() noexcept = default;
  CIppVector &operator=(CIppVector const &other) {
    if (this != &other) {
      if (size() == 0 && m_Vector == nullptr) {
        m_Vector = UniquePtr{CIppAllocator::allocate<ElementType>(other.size()),
                             CIppAllocator::deallocate};
        m_VectorSize = other.size();
        m_VectorCapacity = other.capacity();
      } else {
        Private::ippVectorIsEqual(other, *this);
      }
      std::function<IppStatus(ElementType const *, ElementType *, int)>
          ippsCopy{};
      if constexpr (std::is_same_v<ElementType, Ipp64f>) {
        ippsCopy = ippsCopy_64f;
      } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
        ippsCopy = ippsCopy_64fc;
      } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
        ippsCopy = ippsCopy_32f;
      } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
        ippsCopy = ippsCopy_32fc;
      } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
        ippsCopy = ippsCopy_32s;
      } else if constexpr (std::is_same_v<ElementType, Ipp32sc>) {
        ippsCopy = ippsCopy_32sc;
      } else if constexpr (std::is_same_v<ElementType, Ipp16s>) {
        ippsCopy = ippsCopy_16s;
      } else if constexpr (std::is_same_v<ElementType, Ipp16sc>) {
        ippsCopy = ippsCopy_16sc;
      } else {
        static_assert(std::is_void_v<ElementType>, "unsupported type");
      }
      IPP_CHECK_STATUS(ippsCopy(other.begin(), m_Vector.get(), size()));
    }
    return *this;
  }
  CIppVector &operator=(CIppVector &&other) {
    if (this != &other) {
      m_Vector = std::exchange(other.m_Vector, nullptr);
      m_VectorSize = std::exchange(other.m_VectorSize, 0);
      m_VectorCapacity = std::exchange(other.m_VectorCapacity, 0);
    }
    return *this;
  }
  void reinit(SizeType size) {
    CIppVector tmp(size);
    if (this->size() > 0) {
      tmp.assign(*this);
    }
    m_Vector.swap(tmp.m_Vector);
    m_VectorSize = std::exchange(tmp.m_VectorSize, 0);
    m_VectorCapacity = std::exchange(tmp.m_VectorCapacity, 0);
  }
  void resize(SizeType size) {
    if (size > m_VectorCapacity) {
      reinit(size);
    } else {
      m_VectorSize = size;
    }
  }
  void assign(ElementType value, SizeType pos = 0,
              SizeType count = std::numeric_limits<SizeType>::max()) {
    if (pos > size() - 1) {
      throw std::out_of_range("assign argument out of range");
    }
    // TODO: проверить корректность работы при разных pos и size
    SizeType size{this->size()};
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    std::function<IppStatus(ElementType, ElementType *, int)> ippsSet{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsSet = ippsSet_64f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsSet = ippsSet_64fc;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsSet = ippsSet_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsSet = ippsSet_32fc;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsSet(value, m_Vector.get() + pos, size));
  }
  void assign(ElementType const *const data, SizeType dataSize,
              SizeType pos = 0,
              SizeType count = std::numeric_limits<SizeType>::max()) {
    if (data == nullptr) {
      throw std::invalid_argument("assign argument nullptr");
    }
    if (pos > size() - 1) {
      throw std::out_of_range("assign argument out of range");
    }
    // TODO: проверить корректность работы при разных pos и size
    SizeType size{(dataSize > this->size()) ? this->size() : dataSize};
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    std::function<IppStatus(ElementType const *, ElementType *, int)>
        ippsCopy{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsCopy = ippsCopy_64f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsCopy = ippsCopy_64fc;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsCopy = ippsCopy_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsCopy = ippsCopy_32fc;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsCopy = ippsCopy_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32sc>) {
      ippsCopy = ippsCopy_32sc;
    } else if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsCopy = ippsCopy_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp16sc>) {
      ippsCopy = ippsCopy_16sc;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsCopy(data, m_Vector.get() + pos, size));
  }
  void assign(CIppVector const &vector, SizeType pos = 0,
              SizeType count = std::numeric_limits<SizeType>::max()) {
    assign(vector.begin(), vector.size(), pos, count);
  }
  template <typename Type>
  inline CIppVector &operator=(Type rhs) {
    assign(elementTypeCast(rhs));
    return *this;
  }
  friend inline CIppVector operator+(CIppVector lhs, CIppVector const &rhs) {
    lhs += rhs;
    return lhs;
  }
  friend inline CIppVector operator+(CIppVector lhs, ElementType rhs) {
    lhs += rhs;
    return lhs;
  }
  CIppVector &operator+=(CIppVector const &rhs) {
    Private::ippVectorIsEqual(rhs, *this);
    std::function<IppStatus(ElementType const *, ElementType *, int)> ippsAdd{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsAdd = ippsAdd_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsAdd = ippsAdd_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsAdd = ippsAdd_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsAdd = ippsAdd_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsAdd(rhs.begin(), m_Vector.get(), size()));
    return *this;
  }
  template <typename Type>
  CIppVector &operator+=(Type rhs) {
    std::function<IppStatus(ElementType, ElementType *, int)> ippsAdd{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsAdd = ippsAddC_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsAdd = ippsAddC_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsAdd = ippsAddC_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsAdd = ippsAddC_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsAdd(elementTypeCast(rhs), m_Vector.get(), size()));
    return *this;
  }
  inline friend CIppVector operator-(CIppVector lhs, CIppVector const &rhs) {
    lhs -= rhs;
    return lhs;
  }
  inline friend CIppVector operator-(CIppVector lhs, ElementType rhs) {
    lhs -= rhs;
    return lhs;
  }
  CIppVector &operator-=(CIppVector const &rhs) {
    Private::ippVectorIsEqual(rhs, *this);
    std::function<IppStatus(ElementType const *, ElementType *, int)> ippsSub{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsSub = ippsSub_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsSub = ippsSub_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsSub = ippsSub_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsSub = ippsSub_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsSub(rhs.begin(), m_Vector.get(), size()));
    return *this;
  }
  template <typename Type>
  CIppVector &operator-=(Type rhs) {
    std::function<IppStatus(ElementType, ElementType *, int)> ippsSub{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsSub = ippsSubC_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsSub = ippsSubC_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsSub = ippsSubC_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsSub = ippsSubC_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsSub(elementTypeCast(rhs), m_Vector.get(), size()));
    return *this;
  }
  inline friend CIppVector operator/(CIppVector lhs, CIppVector const &rhs) {
    lhs /= rhs;
    return lhs;
  }
  inline friend CIppVector operator/(CIppVector lhs, ElementType rhs) {
    lhs /= rhs;
    return lhs;
  }
  CIppVector &operator/=(CIppVector const &rhs) {
    Private::ippVectorIsEqual(rhs, *this);
    std::function<IppStatus(ElementType const *, ElementType *, int)> ippsDiv{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsDiv = ippsDiv_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsDiv = ippsDiv_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsDiv = ippsDiv_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsDiv = ippsDiv_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsDiv(rhs.begin(), m_Vector.get(), size()));
    return *this;
  }
  template <typename Type>
  CIppVector &operator/=(Type rhs) {
    std::function<IppStatus(ElementType, ElementType *, int)> ippsDiv{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsDiv = ippsDivC_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsDiv = ippsDivC_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsDiv = ippsDivC_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsDiv = ippsDivC_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsDiv(elementTypeCast(rhs), m_Vector.get(), size()));
    return *this;
  }
  inline friend CIppVector operator*(CIppVector lhs, CIppVector const &rhs) {
    lhs *= rhs;
    return lhs;
  }
  inline friend CIppVector operator*(CIppVector lhs, ElementType rhs) {
    lhs *= rhs;
    return lhs;
  }
  CIppVector &operator*=(CIppVector const &rhs) {
    Private::ippVectorIsEqual(rhs, *this);
    std::function<IppStatus(ElementType const *, ElementType *, int)> ippsMul{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsMul = ippsMul_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsMul = ippsMul_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsMul = ippsMul_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsMul = ippsMul_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsMul(rhs.begin(), m_Vector.get(), size()));
    return *this;
  }
  template <typename Type>
  CIppVector &operator*=(Type rhs) {
    std::function<IppStatus(ElementType, ElementType *, int)> ippsMul{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsMul = ippsMulC_64f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsMul = ippsMulC_64fc_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsMul = ippsMulC_32f_I;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsMul = ippsMulC_32fc_I;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsMul(elementTypeCast(rhs), m_Vector.get(), size()));
    return *this;
  }
  inline CIppVector &operator++() {
    int const val{1};
    this += elementTypeCast(val);
    return *this;
  }
  CIppVector operator++(int) {
    CIppVector tmp(*this);
    ++(*this);
    return tmp;
  }
  inline CIppVector &operator--() {
    static_assert(std::negation_v<std::is_floating_point<ElementType>>,
                  "not allowed for this type");
    int const val{1};
    *this -= elementTypeCast(val);
    return *this;
  }
  CIppVector operator--(int) {
    static_assert(std::negation_v<std::is_floating_point<ElementType>>,
                  "not allowed for this type");
    CIppVector tmp(*this);
    --(*this);
    return tmp;
  }
  inline ElementType &operator[](size_t idx) const noexcept {
    ElementType *vector{m_Vector.get()};
    return vector[idx];
  }
  inline SizeType size() const noexcept { return m_VectorSize; }
  inline bool empty() const noexcept { return size() == 0; }
  inline SizeType capacity() const noexcept { return m_VectorCapacity; }
  inline SizeType bytes() const noexcept { return size() * sizeof(IppType); }
  auto max(SizeType pos = 0,
           SizeType count = std::numeric_limits<SizeType>::max()) const {
    if (pos > size() - 1) {
      throw std::out_of_range("subvec argument out of range");
    }
    SizeType size{this->size()};
    // TODO: проверить корректность работы при разных pos и size
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    ElementType maxValue{};
    std::function<IppStatus(ElementType const *, int, ElementType *)> ippsMax{};
    if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsMax = ippsMax_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsMax = ippsMax_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsMax = ippsMax_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsMax = ippsMax_64f;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsMax(m_Vector.get() + pos, size, &maxValue));
    return maxValue;
  }
  auto min(SizeType pos = 0,
           SizeType count = std::numeric_limits<SizeType>::max()) const {
    if (pos > size() - 1) {
      throw std::out_of_range("subvec argument out of range");
    }
    SizeType size{this->size()};
    // TODO: проверить корректность работы при разных pos и size
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    ElementType minValue{};
    std::function<IppStatus(ElementType const *, int, ElementType *)> ippsMin{};
    if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsMin = ippsMin_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsMin = ippsMin_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsMin = ippsMin_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsMin = ippsMin_64f;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsMin(m_Vector.get() + pos, size, &minValue));
    return minValue;
  }
  auto maxIndex(SizeType pos = 0,
                SizeType count = std::numeric_limits<SizeType>::max()) const {
    if (pos > size() - 1) {
      throw std::out_of_range("subvec argument out of range");
    }
    SizeType size{this->size()};
    // TODO: проверить корректность работы при разных pos и size
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    ElementType maxValue{};
    int maxIndex{};
    std::function<IppStatus(ElementType const *, int, ElementType *, int *)>
        ippsMaxIndx{};
    if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsMaxIndx = ippsMaxIndx_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsMaxIndx = ippsMaxIndx_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsMaxIndx = ippsMaxIndx_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsMaxIndx = ippsMaxIndx_64f;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(
        ippsMaxIndx(m_Vector.get() + pos, size, &maxValue, &maxIndex));
    return std::pair(maxValue, maxIndex + pos);
  }
  auto minIndex(SizeType pos = 0,
                SizeType count = std::numeric_limits<SizeType>::max()) const {
    if (pos > size() - 1) {
      throw std::out_of_range("subvec argument out of range");
    }
    SizeType size{this->size()};
    // TODO: проверить корректность работы при разных pos и size
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    ElementType minValue{};
    int minIndex{};
    std::function<IppStatus(ElementType const *, int, ElementType *, int *)>
        ippsMinIndx{};
    if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsMinIndx = ippsMinIndx_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsMinIndx = ippsMinIndx_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsMinIndx = ippsMinIndx_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsMinIndx = ippsMinIndx_64f;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(
        ippsMinIndx(m_Vector.get() + pos, size, &minValue, &minIndex));
    return std::pair(minValue, minIndex + pos);
  }
  auto minMaxIndex(
      SizeType pos = 0,
      SizeType count = std::numeric_limits<SizeType>::max()) const {
    if (pos > size() - 1) {
      throw std::out_of_range("subvec argument out of range");
    }
    SizeType size{this->size()};
    // TODO: проверить корректность работы при разных pos и size
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    ElementType minValue{}, maxValue{};
    int minIndex{}, maxIndex{};
    std::function<IppStatus(ElementType const *, int, ElementType *, int *,
                            ElementType *, int *)>
        ippsMinMaxIndx{};
    if constexpr (std::is_same_v<ElementType, Ipp8u>) {
      ippsMinMaxIndx = ippsMinMaxIndx_8u;
    } else if constexpr (std::is_same_v<ElementType, Ipp16u>) {
      ippsMinMaxIndx = ippsMinMaxIndx_16u;
    } else if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsMinMaxIndx = ippsMinMaxIndx_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32u>) {
      ippsMinMaxIndx = ippsMinMaxIndx_32u;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsMinMaxIndx = ippsMinMaxIndx_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsMinMaxIndx = ippsMinMaxIndx_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsMinMaxIndx = ippsMinMaxIndx_64f;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsMinMaxIndx(m_Vector.get() + pos, size, &minValue,
                                    &minIndex, &maxValue, &maxIndex));
    return std::make_tuple(minValue, minIndex + pos, maxValue, maxIndex + pos);
  }
  auto sum() {
    ElementType sumValue{};
    if constexpr (std::is_same_v<ElementType, Ipp64f> ||
                  std::is_same_v<ElementType, Ipp64fc>) {
      std::function<IppStatus(ElementType *, int, ElementType *)> ippsSum{};
      if constexpr (std::is_same_v<ElementType, Ipp64f>) {
        ippsSum = ippsSum_64f;
      } else {
        ippsSum = ippsSum_64fc;
      }
      IPP_CHECK_STATUS(ippsSum(m_Vector.get(), size(), &sumValue));
    } else if constexpr (std::is_same_v<ElementType, Ipp32f> ||
                         std::is_same_v<ElementType, Ipp32fc>) {
      std::function<IppStatus(ElementType *, int, ElementType *,
                              IppHintAlgorithm)>
          ippsSum{};
      if constexpr (std::is_same_v<ElementType, Ipp32f>) {
        ippsSum = ippsSum_32f;
      } else {
        ippsSum = ippsSum_32fc;
      }
      IPP_CHECK_STATUS(
          ippsSum(m_Vector.get(), size(), &sumValue, ippAlgHintNone));
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    return sumValue;
  }
  void clear() {
    std::function<IppStatus(ElementType *, int)> ippsZero{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsZero = ippsZero_64f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsZero = ippsZero_64fc;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsZero = ippsZero_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsZero = ippsZero_32fc;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsZero = ippsZero_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32sc>) {
      ippsZero = ippsZero_32sc;
    } else if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsZero = ippsZero_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp16sc>) {
      ippsZero = ippsZero_16sc;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    if (!empty()) {
      IPP_CHECK_STATUS(ippsZero(m_Vector.get(), size()));
    }
  }
  inline auto begin() noexcept { return m_Vector.get(); }
  inline auto end() noexcept { return m_Vector.get() + size(); }
  inline auto const begin() const noexcept { return m_Vector.get(); }
  inline auto const end() const noexcept { return m_Vector.get() + size(); }
  void subvec(CIppVector &vector, SizeType pos = 0,
              SizeType count = std::numeric_limits<SizeType>::max()) {
    if (pos > size() - 1) {
      throw std::out_of_range("subvec argument out of range");
    }
    SizeType size{(vector.size() > this->size()) ? this->size()
                                                 : vector.size()};
    // TODO: проверить корректность работы при разных pos и size
    if (count < size) {
      size = count;
    }
    if (pos + size > this->size()) {
      size -= pos;
    }
    std::function<IppStatus(ElementType const *, ElementType *, SizeType)>
        ippsCopy{};
    if constexpr (std::is_same_v<ElementType, Ipp64f>) {
      ippsCopy = ippsCopy_64f;
    } else if constexpr (std::is_same_v<ElementType, Ipp64fc>) {
      ippsCopy = ippsCopy_64fc;
    } else if constexpr (std::is_same_v<ElementType, Ipp32f>) {
      ippsCopy = ippsCopy_32f;
    } else if constexpr (std::is_same_v<ElementType, Ipp32fc>) {
      ippsCopy = ippsCopy_32fc;
    } else if constexpr (std::is_same_v<ElementType, Ipp32s>) {
      ippsCopy = ippsCopy_32s;
    } else if constexpr (std::is_same_v<ElementType, Ipp32sc>) {
      ippsCopy = ippsCopy_32sc;
    } else if constexpr (std::is_same_v<ElementType, Ipp16s>) {
      ippsCopy = ippsCopy_16s;
    } else if constexpr (std::is_same_v<ElementType, Ipp16sc>) {
      ippsCopy = ippsCopy_16sc;
    } else {
      static_assert(std::is_void_v<ElementType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsCopy(m_Vector.get() + pos, vector.begin(), size));
  }
  friend bool operator==(CIppVector const &lhs, CIppVector const &rhs) {
    if (lhs.size() != rhs.size()) {
      return false;
    }
    int result{};
    IPP_CHECK_STATUS(ippsEqual_8u((Ipp8u const *)(lhs.begin()),
                                  (Ipp8u const *)(rhs.begin()), lhs.size(),
                                  &result));
    return bool(result);
  }
  inline friend bool operator!=(CIppVector const &lhs, CIppVector const &rhs) {
    return !(lhs == rhs);
  }
  // friend bool operator==(CIppVector const &lhs, ElementType const &rhs) {
  //  // TODO: реализовать
  //  return true;
  //}
  inline friend bool operator!=(CIppVector const &lhs, ElementType const &rhs) {
    return !(lhs == rhs);
  }
  friend bool operator==(ElementType const &lhs, CIppVector const &rhs) {
    return rhs == lhs;
  }
  inline friend bool operator!=(ElementType const &lhs, CIppVector const &rhs) {
    return !(lhs == rhs);
  }

 private:
  template <typename Type>
  ElementType elementTypeCast(Type value) {
    if constexpr (std::is_same_v<ElementType, Type>) {
      return value;
    } else if constexpr (std::is_arithmetic_v<ElementType>) {
      return ElementType(value);
    } else if constexpr (std::is_trivial_v<ElementType>) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#endif
      ElementType res{value, value};
#ifdef __clang__
#pragma clang diagnostic pop
#endif
      return res;
    }
  }
  SizeType m_VectorSize{};
  SizeType m_VectorCapacity{};
  UniquePtr m_Vector{};
};

using CIppVector64f = CIppVector<Ipp64f>;
using CIppVector64fc = CIppVector<Ipp64fc>;
using CIppVector32f = CIppVector<Ipp32f>;
using CIppVector32fc = CIppVector<Ipp32fc>;
using CIppVector32s = CIppVector<Ipp32s>;
using CIppVector32sc = CIppVector<Ipp32sc>;

}  // namespace IntelIppTL

#endif  // IIPPTL_VECTOR_HPP
