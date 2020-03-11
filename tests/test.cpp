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

#include "iipptl_accuracy.hpp"
#include "iipptl_allocator.hpp"
#include "iipptl_average.hpp"
#include "iipptl_convert.hpp"
#include "iipptl_delay.hpp"
#include "iipptl_fft.hpp"
#include "iipptl_fft_init.hpp"
#include "iipptl_fft_param.hpp"
#include "iipptl_math.hpp"
#include "iipptl_queue.hpp"
#include "iipptl_sampling.hpp"
#include "iipptl_threshold.hpp"
#include "iipptl_vector.hpp"

using namespace IntelIppTL;

TEST(TEST, TEST) {
  auto const vecSize{10};
  CIppVector64f vec64f_in{vecSize};
  vec64f_in =100;
  CIppVector64f vec64f_out{vecSize};
  ippLog10(vec64f_in, vec64f_out);

  CIppVector32f vec32f_in{vecSize};
  vec32f_in = 100;
  CIppVector32f vec32f_out{vecSize};
  ippLog10<AccuracyLevels::SinglePrecision::A24>(vec32f_in, vec32f_out);
  ippLog10(vec32f_in, vec32f_out);
  SUCCEED();
}
