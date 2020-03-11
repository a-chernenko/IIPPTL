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

#include "iipptl_fft.hpp"

using namespace IntelIppTL;

template <typename IppType>
class CIppFFTTest : public ::testing::Test {
 public:
};

template <typename IppType>
using CIppFFTComplexTest = CIppFFTTest<IppType>;

TYPED_TEST_CASE_P(CIppFFTTest);
TYPED_TEST_CASE_P(CIppFFTComplexTest);

TYPED_TEST_P(CIppFFTTest, TEST) {
  auto const fftVecSize{1024 * 1024};
  auto constexpr fftOrder = CIppFFTOrder::calculate(fftVecSize);
  auto cmplxvec_in_re{CIppVector<TypeParam>(fftVecSize)};
  auto cmplxvec_in_im{CIppVector<TypeParam>(fftVecSize)};
  auto cmplxvec_out_re{CIppVector<TypeParam>(fftVecSize)};
  auto cmplxvec_out_im{CIppVector<TypeParam>(fftVecSize)};
  auto fft{CIppFFT<TypeParam>(fftOrder, IppFFTFlags::ForwardTransformByN)};
  fft.forward(cmplxvec_in_re, cmplxvec_in_im, cmplxvec_out_re, cmplxvec_out_im);
  fft.forward(cmplxvec_in_re, cmplxvec_in_im);
}

TYPED_TEST_P(CIppFFTComplexTest, TEST) {
  auto const fftVecSize{1024 * 1024};
  auto constexpr fftOrder = CIppFFTOrder::calculate(fftVecSize);
  auto vec_in{CIppVector<TypeParam>(fftVecSize)};
  auto vec_out{CIppVector<TypeParam>(fftVecSize)};
  auto fft{CIppFFT<TypeParam>(fftOrder, IppFFTFlags::ForwardTransformByN)};
  fft.forward(vec_in, vec_out);
  fft.forward(vec_in);
}

REGISTER_TYPED_TEST_CASE_P(CIppFFTTest, TEST);
using IppTypes = ::testing::Types<Ipp32f, Ipp64f>;
INSTANTIATE_TYPED_TEST_CASE_P(AllTypes, CIppFFTTest, IppTypes);

REGISTER_TYPED_TEST_CASE_P(CIppFFTComplexTest, TEST);
using IppComplexTypes = ::testing::Types<Ipp32fc, Ipp64fc>;
INSTANTIATE_TYPED_TEST_CASE_P(IppComplexTypes, CIppFFTComplexTest,
                              IppComplexTypes);
