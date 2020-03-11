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

#ifndef IIPPTL_CONVERT_HPP
#define IIPPTL_CONVERT_HPP

#include "iipptl_vector.hpp"

namespace IntelIppTL {

inline void ippConvert(CIppVector<Ipp32f> const &source,
                       CIppVector<Ipp64f> &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsConvert_32f64f(source.begin(), dest.begin(), dest.size()));
}

inline void ippConvert(CIppVector<Ipp64f> const &source,
                       CIppVector<Ipp32f> &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsConvert_64f32f(source.begin(), dest.begin(), dest.size()));
}

inline void ippConvert(CIppVector<Ipp32s> const &source,
                       CIppVector<Ipp32f> &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsConvert_32s32f(source.begin(), dest.begin(), dest.size()));
}

inline void ippConvert(CIppVector<Ipp16s> const &source,
                       CIppVector<Ipp32f> &dest) {
  Private::ippVectorIsEqual(source, dest);
  IPP_CHECK_STATUS(
      ippsConvert_16s32f(source.begin(), dest.begin(), dest.size()));
}

inline void ippConvert(CIppVector<Ipp32f> const &sourceRe,
                       CIppVector<Ipp32f> const &sourceIm,
                       CIppVector<Ipp32fc> &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  IPP_CHECK_STATUS(ippsRealToCplx_32f(sourceRe.begin(), sourceIm.begin(),
                                      dest.begin(), dest.size()));
}

inline void ippConvert(CIppVector<Ipp64f> const &sourceRe,
                       CIppVector<Ipp64f> const &sourceIm,
                       CIppVector<Ipp64fc> &dest) {
  Private::ippVectorIsEqual(sourceRe, dest);
  Private::ippVectorIsEqual(sourceIm, dest);
  IPP_CHECK_STATUS(ippsRealToCplx_64f(sourceRe.begin(), sourceIm.begin(),
                                      dest.begin(), dest.size()));
}

inline void ippConvert(CIppVector<Ipp32fc> const &source,
                       CIppVector<Ipp32f> &destRe, CIppVector<Ipp32f> &destIm) {
  Private::ippVectorIsEqual(source, destRe);
  Private::ippVectorIsEqual(source, destIm);
  IPP_CHECK_STATUS(ippsCplxToReal_32fc(source.begin(), destRe.begin(),
                                       destIm.begin(), source.size()));
}

inline void ippConvert(CIppVector<Ipp64fc> const &source,
                       CIppVector<Ipp64f> &destRe, CIppVector<Ipp64f> &destIm) {
  Private::ippVectorIsEqual(source, destRe);
  Private::ippVectorIsEqual(source, destIm);
  IPP_CHECK_STATUS(ippsCplxToReal_64fc(source.begin(), destRe.begin(),
                                       destIm.begin(), source.size()));
}

inline void ippConvertReal(CIppVector<Ipp32fc> const &source,
                           CIppVector<Ipp32f> &destRe) {
  Private::ippVectorIsEqual(source, destRe);
  IPP_CHECK_STATUS(
      ippsReal_32fc(source.begin(), destRe.begin(), destRe.size()));
}

inline void ippConvertReal(CIppVector<Ipp64fc> const &source,
                           CIppVector<Ipp64f> &destRe) {
  Private::ippVectorIsEqual(source, destRe);
  IPP_CHECK_STATUS(
      ippsReal_64fc(source.begin(), destRe.begin(), destRe.size()));
}

inline void ippConvertImag(CIppVector<Ipp32fc> const &source,
                           CIppVector<Ipp32f> &destIm) {
  Private::ippVectorIsEqual(source, destIm);
  IPP_CHECK_STATUS(
      ippsImag_32fc(source.begin(), destIm.begin(), destIm.size()));
}

inline void ippConvertImag(CIppVector<Ipp64fc> const &source,
                           CIppVector<Ipp64f> &destIm) {
  Private::ippVectorIsEqual(source, destIm);
  IPP_CHECK_STATUS(
      ippsImag_64fc(source.begin(), destIm.begin(), destIm.size()));
}

}  // namespace IntelIppTL

#endif  // IIPPTL_CONVERT_HPP
