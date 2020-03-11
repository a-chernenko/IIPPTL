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

#include "pch.h"

#include "iipptl_allocator.hpp"

using namespace IntelIppTL;

auto operator"" _B(unsigned long long int value) { return value; }

auto operator"" _kB(unsigned long long int value) { return value * 1024; }

auto operator"" _MB(unsigned long long int value) {
  return value * 1024 * 1024;
}

auto operator"" _GB(unsigned long long int value) {
  return value * 1024 * 1024 * 1024;
}

template <typename IppType>
class CIppAllocatorTest : public ::testing::Test {
 public:
  inline static const auto k_AllocatedSize{128_kB};
  static const auto k_AllocatedNum{32 * 192};
};

TYPED_TEST_CASE_P(CIppAllocatorTest);

TYPED_TEST_P(CIppAllocatorTest, Allocate) {
  auto size{k_AllocatedSize / sizeof TypeParam};
  TypeParam *ptr[k_AllocatedNum];
  int allocatedError{};
  for (int i{}; i < k_AllocatedNum; ++i) {
    ptr[i] = CIppAllocator::allocate<TypeParam>(size);
    if (ptr[i] == nullptr) {
      ++allocatedError;
    }
  }
  for (int i{}; i < k_AllocatedNum; ++i) {
    if (ptr[i]) {
      CIppAllocator::deallocate(ptr[i]);
    }
  }
  if (allocatedError > 0) {
    FAIL();
  }
  SUCCEED();
}

REGISTER_TYPED_TEST_CASE_P(CIppAllocatorTest, Allocate);

using IppTypes = ::testing::Types<Ipp8u, Ipp8s, Ipp16u, Ipp16s, Ipp32u, Ipp32s,
                                  Ipp32f, Ipp64f, Ipp8sc, Ipp16sc, Ipp32sc,
                                  Ipp32fc, Ipp64sc, Ipp64fc>;
INSTANTIATE_TYPED_TEST_CASE_P(Allocator, CIppAllocatorTest, IppTypes);
