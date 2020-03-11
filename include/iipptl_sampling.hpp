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

#ifndef IIPPTL_SAMPLING_HPP
#define IIPPTL_SAMPLING_HPP

#include "iipptl_vector.hpp"

namespace IntelIppTL {

// FIXME: не реализован.

template <typename IppType>
void ippSampleDown(CIppVector<IppType> const &source, CIppVector<IppType> &dest,
                   int factor, int phase) {
  // TODO: Добавить проверку на достаточность размера принимающего вектора.
  //  if (source.size() != dest.size()) {
  //    throw std::invalid_argument("vectors size is not equal");
  //  }
  std::function<IppStatus(IppType const *, int, IppType *, int *, int, int *)>
      ippsSampleUp{};
  if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsSampleDown = ippsSampleDown_64f;
  } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
    ippsSampleDown = ippsSampleDown_64fc;
  } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
    ippsSampleDown = ippsSampleDown_32f;
  } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
    ippsSampleDown = ippsSampleDown_32fc;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  int destSize{};
  IPP_CHECK_STATUS(ippsSampleDown(source.begin(), source.size(), dest.begin(),
                                  &destSize, factor, &phase));
}

}  // namespace IntelIppTL

#endif  // IIPPTL_SAMPLING_HPP
