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

#ifndef IIPPTL_ALLOCATOR_HPP
#define IIPPTL_ALLOCATOR_HPP

#if __cplusplus < 201703L
#error \
    "This file requires compiler and library support for the ISO C++ 2017 standard."
#endif

#include <functional>
#include <type_traits>

#include "ipp.h"

namespace IntelIppTL {

class CIppAllocator final {
 public:
  using SizeType = int;
  CIppAllocator() = delete;
  template <typename IppType>
  [[nodiscard]] static IppType *allocate(SizeType size) {
    if (size <= 0) {
      return nullptr;
    }
    std::function<IppType *(int)> ippsMalloc{};
    IppType *ptr{};
    if constexpr (std::is_same_v<IppType, Ipp8u>) {
      ippsMalloc = ippsMalloc_8u;
    } else if constexpr (std::is_same_v<IppType, Ipp8s>) {
      ippsMalloc = ippsMalloc_8s;
    } else if constexpr (std::is_same_v<IppType, Ipp16u>) {
      ippsMalloc = ippsMalloc_16u;
    } else if constexpr (std::is_same_v<IppType, Ipp16s>) {
      ippsMalloc = ippsMalloc_16s;
    } else if constexpr (std::is_same_v<IppType, Ipp32u>) {
      ippsMalloc = ippsMalloc_32u;
    } else if constexpr (std::is_same_v<IppType, Ipp32s>) {
      ippsMalloc = ippsMalloc_32s;
    } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsMalloc = ippsMalloc_32f;
    } else if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsMalloc = ippsMalloc_64f;
    } else if constexpr (std::is_same_v<IppType, Ipp8sc>) {
      ippsMalloc = ippsMalloc_8sc;
    } else if constexpr (std::is_same_v<IppType, Ipp16sc>) {
      ippsMalloc = ippsMalloc_16sc;
    } else if constexpr (std::is_same_v<IppType, Ipp16uc>) {
      static_assert(std::is_same_v<IppType, Ipp16uc>, "not implemented");
    } else if constexpr (std::is_same_v<IppType, Ipp32sc>) {
      ippsMalloc = ippsMalloc_32sc;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsMalloc = ippsMalloc_32fc;
    } else if constexpr (std::is_same_v<IppType, Ipp64sc>) {
      ippsMalloc = ippsMalloc_64sc;
    } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsMalloc = ippsMalloc_64fc;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    ptr = ippsMalloc(size);
    if (ptr == nullptr) {
      throw std::bad_alloc();
    }
    return ptr;
  }
  static void deallocate(void *ptr) {
    if (ptr != nullptr) {
      ippsFree(ptr);
      ptr = nullptr;
    }
  }
};

}  // namespace IntelIppTL

#endif  // IIPPTL_ALLOCATOR_HPP
