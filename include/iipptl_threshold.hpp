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

#ifndef IIPPTL_THRESHOLD_HPP
#define IIPPTL_THRESHOLD_HPP

#include "iipptl_vector.hpp"

namespace IntelIppTL {

template <typename IppType>
void ippThreshold(CIppVector<IppType> const &source, CIppVector<IppType> &dest,
                  IppType levelLT, IppType valueLT, IppType levelGT,
                  IppType valueGT) {
  Private::ippVectorIsEqual(source, dest);
  std::function<IppStatus(IppType const *, IppType *, int, IppType, IppType,
                          IppType, IppType)>
      ippsThreshold_LTValGTVal{};
  if constexpr (std::is_same_v<IppType, Ipp16s>) {
    ippsThreshold_LTValGTVal = ippsThreshold_LTValGTVal_16s;
  } else if constexpr (std::is_same_v<IppType, Ipp32s>) {
    ippsThreshold_LTValGTVal = ippsThreshold_LTValGTVal_32s;
  } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
    ippsThreshold_LTValGTVal = ippsThreshold_LTValGTVal_32f;
  } else if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsThreshold_LTValGTVal = ippsThreshold_LTValGTVal_64f;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsThreshold_LTValGTVal(source.begin(), dest.begin(),
                                            dest.size(), levelLT, valueLT,
                                            levelGT, valueGT));
}

template <typename IppType>
void ippThreshold(CIppVector<IppType> &vector, IppType level, IppType value) {
  std::function<IppStatus(IppType *, int, IppType, IppType)>
      ippsThreshold_LTVal{};
  if constexpr (std::is_same_v<IppType, Ipp16s>) {
    ippsThreshold_LTVal = ippsThreshold_LTVal_16s_I;
  } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
    ippsThreshold_LTVal = ippsThreshold_LTVal_32f_I;
  } else if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsThreshold_LTVal = ippsThreshold_LTVal_64f_I;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(
      ippsThreshold_LTVal(vector.begin(), vector.size(), level, value));
}

}  // namespace IntelIppTL

#endif  // IIPPTL_THRESHOLD_HPP
