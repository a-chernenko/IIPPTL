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

#include "iipptl_vector.hpp"

using namespace IntelIppTL;

template <typename IppType>
class CIppVectorTest : public ::testing::Test {
 public:
  using IppVector = CIppVector<IppType>;
  static auto const m_VecSize{100};
};

TYPED_TEST_CASE_P(CIppVectorTest);

TYPED_TEST_P(CIppVectorTest, Compare) {
  auto const value1{TypeParam{1}};
  auto vector1{CIppVector<TypeParam>(m_VecSize)};
  auto const value2{TypeParam{1}};
  auto vector2{CIppVector<TypeParam>(m_VecSize)};
  vector1 = value1;
  vector2 = value2;
  ASSERT_EQ(vector1, vector2);
}

REGISTER_TYPED_TEST_CASE_P(CIppVectorTest, Compare);

using IppTypes = ::testing::Types<Ipp32f, Ipp64f, Ipp32fc, Ipp64fc>;
INSTANTIATE_TYPED_TEST_CASE_P(Vector, CIppVectorTest, IppTypes);
