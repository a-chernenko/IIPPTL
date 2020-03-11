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

#ifndef IIPPTL_MATH_HPP
#define IIPPTL_MATH_HPP

#include "iipptl_accuracy.hpp"
#include "iipptl_vector.hpp"

namespace IntelIppTL {

template <typename IppType>
void ippLn(CIppVector<IppType> const &source, CIppVector<IppType> &dest) {
  Private::ippVectorIsEqual(source, dest);
  std::function<IppStatus(IppType const *, IppType *, int)> ippsLn{};
  if constexpr (std::is_same_v<IppType, Ipp32f>) {
    ippsLn = ippsLn_32f;
  } else if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsLn = ippsLn_64f;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsLn(source.begin(), dest.begin(), dest.size()));
}

template <typename IppType>
void ippLn(CIppVector<IppType> &vector) {
  std::function<IppStatus(IppType *, int)> ippsLn{};
  if constexpr (std::is_same_v<IppType, Ipp32f>) {
    ippsLn = ippsLn_32f_I;
  } else if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsLn = ippsLn_64f_I;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsLn(vector.begin(), vector.size()));
}

template <AccuracyLevels::SinglePrecision const accuracy =
              AccuracyLevels::SinglePrecision::A11,
          typename IppType>
void ippLog10(CIppVector<IppType> const &source, CIppVector<IppType> &dest,
              std::enable_if_t<std::is_same_v<IppType, Ipp32f> ||
                                   std::is_same_v<IppType, Ipp32fc>,
                               int> = 0) {
  Private::ippVectorIsEqual(source, dest);
  std::function<IppStatus(IppType const *, IppType *, int)> ippsLog10{};
  if constexpr (accuracy == AccuracyLevels::SinglePrecision::A11) {
    if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsLog10 = ippsLog10_32f_A11;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsLog10 = ippsLog10_32fc_A11;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
  } else if constexpr (accuracy == AccuracyLevels::SinglePrecision::A21) {
    if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsLog10 = ippsLog10_32f_A21;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsLog10 = ippsLog10_32fc_A21;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
  } else if constexpr (accuracy == AccuracyLevels::SinglePrecision::A24) {
    if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsLog10 = ippsLog10_32f_A24;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsLog10 = ippsLog10_32fc_A24;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
  }
  IPP_CHECK_STATUS(ippsLog10(source.begin(), dest.begin(), dest.size()));
}

template <AccuracyLevels::DoublePrecision const accuracy =
              AccuracyLevels::DoublePrecision::A26,
          typename IppType>
void ippLog10(CIppVector<IppType> const &source, CIppVector<IppType> &dest,
              typename std::enable_if_t<std::is_same_v<IppType, Ipp64f> ||
                                            std::is_same_v<IppType, Ipp64fc>,
                                        int> = 0) {
  Private::ippVectorIsEqual(source, dest);
  std::function<IppStatus(IppType const *, IppType *, int)> ippsLog10{};
  if constexpr (accuracy == AccuracyLevels::DoublePrecision::A26) {
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsLog10 = ippsLog10_64f_A26;
    } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsLog10 = ippsLog10_64fc_A26;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
  } else if constexpr (accuracy == AccuracyLevels::DoublePrecision::A50) {
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsLog10 = ippsLog10_64f_A50;
    } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsLog10 = ippsLog10_64fc_A50;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
  } else if constexpr (accuracy == AccuracyLevels::DoublePrecision::A53) {
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsLog10 = ippsLog10_64f_A53;
    } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsLog10 = ippsLog10_64fc_A53;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
  }
  IPP_CHECK_STATUS(ippsLog10(source.begin(), dest.begin(), dest.size()));
}

template <typename IppType>
void ippSqr(CIppVector<IppType> &vector) {
  std::function<IppStatus(IppType *, int)> ippsSqr{};
  if constexpr (std::is_same_v<IppType, Ipp32f>) {
    ippsSqr = ippsSqr_32f_I;
  } else if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsSqr = ippsSqr_64f_I;
  } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
    ippsSqr = ippsSqr_32fc_I;
  } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
    ippsSqr = ippsSqr_64fc_I;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsSqr(vector.begin(), vector.size()));
}

template <typename IppType>
void ippSqrt(CIppVector<IppType> &vector) {
  std::function<IppStatus(IppType *, int)> ippsSqrt{};
  if constexpr (std::is_same_v<IppType, Ipp32f>) {
    ippsSqrt = ippsSqrt_32f_I;
  } else if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsSqrt = ippsSqrt_64f_I;
  } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
    ippsSqrt = ippsSqrt_32fc_I;
  } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
    ippsSqrt = ippsSqrt_64fc_I;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsSqrt(vector.begin(), vector.size()));
}

template <AccuracyLevels::SinglePrecision accuracy =
              AccuracyLevels::SinglePrecision::A11>
void ippAtan2(CIppVector32f const &sourceRe, CIppVector32f const &sourceIm,
              CIppVector32f &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  std::function<IppStatus(Ipp32f const *, Ipp32f const *, Ipp32f *, int)>
      ippsAtan2{};
  if constexpr (accuracy == AccuracyLevels::SinglePrecision::A11) {
    ippsAtan2 = ippsAtan2_32f_A11;
  } else if constexpr (accuracy == AccuracyLevels::SinglePrecision::A21) {
    ippsAtan2 = ippsAtan2_32f_A21;
  } else if constexpr (accuracy == AccuracyLevels::SinglePrecision::A24) {
    ippsAtan2 = ippsAtan2_32f_A24;
  }
  IPP_CHECK_STATUS(
      ippsAtan2(sourceRe.begin(), sourceIm.begin(), dest.begin(), dest.size()));
}

template <AccuracyLevels::DoublePrecision accuracy =
              AccuracyLevels::DoublePrecision::A26>
void ippAtan2(CIppVector64f const &sourceRe, CIppVector64f const &sourceIm,
              CIppVector64f &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  std::function<IppStatus(Ipp64f const *, Ipp64f const *, Ipp64f *, int)>
      ippsAtan2{};
  if constexpr (accuracy == AccuracyLevels::DoublePrecision::A26) {
    ippsAtan2 = ippsAtan2_64f_A26;
  } else if constexpr (accuracy == AccuracyLevels::DoublePrecision::A50) {
    ippsAtan2 = ippsAtan2_64f_A50;
  } else if constexpr (accuracy == AccuracyLevels::DoublePrecision::A53) {
    ippsAtan2 = ippsAtan2_64f_A53;
  }
  IPP_CHECK_STATUS(
      ippsAtan2(sourceRe.begin(), sourceIm.begin(), dest.begin(), dest.size()));
}

inline void ippMagnitude(CIppVector32fc const &source, CIppVector32f &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsMagnitude_32fc(source.begin(), dest.begin(), dest.size()));
}

inline void ippMagnitude(CIppVector64fc const &source, CIppVector64f &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsMagnitude_64fc(source.begin(), dest.begin(), dest.size()));
}

inline void ippMagnitude(CIppVector32f const &sourceRe,
                         CIppVector32f const &sourceIm, CIppVector32f &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  IPP_CHECK_STATUS(ippsMagnitude_32f(sourceRe.begin(), sourceIm.begin(),
                                     dest.begin(), dest.size()));
}

inline void ippMagnitude(CIppVector64f const &sourceRe,
                         CIppVector64f const &sourceIm, CIppVector64f &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  IPP_CHECK_STATUS(ippsMagnitude_64f(sourceRe.begin(), sourceIm.begin(),
                                     dest.begin(), dest.size()));
}

inline void ippPowerSpectr(CIppVector32fc const &source, CIppVector32f &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsPowerSpectr_32fc(source.begin(), dest.begin(), dest.size()));
}

inline void ippPowerSpectr(CIppVector64fc const &source, CIppVector64f &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsPowerSpectr_64fc(source.begin(), dest.begin(), dest.size()));
}

inline void ippPowerSpectr(CIppVector32f const &sourceRe,
                           CIppVector32f const &sourceIm, CIppVector32f &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  IPP_CHECK_STATUS(ippsPowerSpectr_32f(sourceRe.begin(), sourceIm.begin(),
                                       dest.begin(), dest.size()));
}

inline void ippPowerSpectr(CIppVector64f const &sourceRe,
                           CIppVector64f const &sourceIm, CIppVector64f &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  IPP_CHECK_STATUS(ippsPowerSpectr_64f(sourceRe.begin(), sourceIm.begin(),
                                       dest.begin(), dest.size()));
}

template <typename IppType>
void ippConj(CIppVector<IppType> const &source, CIppVector<IppType> &dest) {
  Private::ippVectorIsEqual(source, dest);
  std::function<IppStatus(IppType const *, IppType *, int)> ippsConj{};
  if constexpr (std::is_same_v<IppType, Ipp64fc>) {
    ippsConj = ippsConj_64fc;
  } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
    ippsConj = ippsConj_32fc;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsConj(source.begin(), dest.begin(), dest.size()));
}

template <typename IppType>
void ippMaxEvery(CIppVector<IppType> const &sourceA,
                 CIppVector<IppType> const &sourceB,
                 CIppVector<IppType> &dest) {
  Private::ippVectorIsEqual(sourceA, dest);
  Private::ippVectorIsEqual(sourceB, dest);
  std::function<IppStatus(IppType const *, IppType const *, IppType *, int)>
      ippsMaxEvery{};
  if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsMaxEvery = ippsMaxEvery_64f;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsMaxEvery(sourceA.begin(), sourceB.begin(), dest.begin(),
                                dest.size()));
}

template <typename IppType>
void ippMinEvery(CIppVector<IppType> const &sourceA,
                 CIppVector<IppType> const &sourceB,
                 CIppVector<IppType> &dest) {
  Private::ippVectorIsEqual(sourceA, dest);
  Private::ippVectorIsEqual(sourceB, dest);
  std::function<IppStatus(IppType const *, IppType const *, IppType *, int)>
      ippsMinEvery{};
  if constexpr (std::is_same_v<IppType, Ipp64f>) {
    ippsMinEvery = ippsMinEvery_64f;
  } else {
    static_assert(std::is_void_v<IppType>, "unsupported type");
  }
  IPP_CHECK_STATUS(ippsMinEvery(sourceA.begin(), sourceB.begin(), dest.begin(),
                                dest.size()));
}

}  // namespace IntelIppTL

#endif  // IIPPTL_MATH_HPP
