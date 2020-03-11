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

#ifndef IIPPTL_FFT_HPP
#define IIPPTL_FFT_HPP

#include "iipptl_fft_init.hpp"
#include "iipptl_vector.hpp"

namespace IntelIppTL {

template <typename IppType>
class CIppFFT {
 public:
  using ValueType = IppType;
  using IppVector = CIppVector<IppType>;
  using IppFFTInit = Detail::CIppFFTInit<IppType>;
  using IppFFTParam = Detail::CIppFFTParam<IppType>;
  using IppFFTSpec = const typename IppFFTInit::FFTSpec;
  using IppFFTMemBuf = const typename IppFFTInit::MemBuf;
  CIppFFT() = default;
  CIppFFT(int order, IppFFTFlags flag = IppFFTFlags::TransformWithoutAny)
      : m_FFTInit{IppFFTParam(order, flag)} {}
  CIppFFT(CIppFFT const &) = default;
  CIppFFT &operator=(CIppFFT const &) = default;
  CIppFFT(CIppFFT &&) noexcept = default;
  CIppFFT &operator=(CIppFFT &&) = default;
  virtual ~CIppFFT() noexcept = default;
  void reinit(int order, IppFFTFlags flag = IppFFTFlags::TransformWithoutAny) {
    m_FFTInit.reinit(IppFFTParam(order, flag));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64fc> ||
                            std::is_same_v<T, Ipp32fc>>
  forward(IppVector const &source, IppVector &dest) {
    std::function<IppStatus(IppType const *, IppType *, IppFFTSpec,
                            IppFFTMemBuf)>
        ippsFFTFwd{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsFFTFwd = ippsFFTFwd_CToC_64fc;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsFFTFwd = ippsFFTFwd_CToC_32fc;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsFFTFwd(source.begin(), dest.begin(),
                                m_FFTInit.fftSpec(), m_FFTInit.memBuf()));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64f> ||
                            std::is_same_v<T, Ipp32f>>
  forward(IppVector const &sourceRe, IppVector const &sourceIm,
          IppVector &destRe, IppVector &destIm) {
    std::function<IppStatus(IppType const *, IppType const *, IppType *,
                            IppType *, IppFFTSpec, IppFFTMemBuf)>
        ippsFFTFwd{};
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsFFTFwd = ippsFFTFwd_CToC_64f;
    } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsFFTFwd = ippsFFTFwd_CToC_32f;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    auto pSpec{m_FFTInit.fftSpec()};
    auto pMemBuf{m_FFTInit.memBuf()};
    IPP_CHECK_STATUS(ippsFFTFwd(sourceRe.begin(), sourceIm.begin(),
                                destRe.begin(), destIm.begin(), pSpec,
                                pMemBuf));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64f> ||
                            std::is_same_v<T, Ipp32f>>
  forward(IppVector &dataRe, IppVector &dataIm) {
    std::function<IppStatus(IppType *, IppType *, IppFFTSpec, IppFFTMemBuf)>
        ippsFFTFwd{};
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsFFTFwd = ippsFFTFwd_CToC_64f_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsFFTFwd = ippsFFTFwd_CToC_32f_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    auto pSpec{m_FFTInit.fftSpec()};
    auto pMemBuf{m_FFTInit.memBuf()};
    IPP_CHECK_STATUS(
        ippsFFTFwd(dataRe.begin(), dataIm.begin(), pSpec, pMemBuf));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64fc> ||
                            std::is_same_v<T, Ipp32fc>>
  forward(IppVector &data) {
    std::function<IppStatus(IppType *, IppFFTSpec, IppFFTMemBuf)> ippsFFTFwd{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsFFTFwd = ippsFFTFwd_CToC_64fc_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsFFTFwd = ippsFFTFwd_CToC_32fc_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    auto pSpec{m_FFTInit.fftSpec()};
    auto pMemBuf{m_FFTInit.memBuf()};
    IPP_CHECK_STATUS(ippsFFTFwd(data.begin(), pSpec, pMemBuf));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64fc> ||
                            std::is_same_v<T, Ipp32fc>>
  inverse(IppVector const &source, IppVector &dest) {
    std::function<IppStatus(IppType const *, IppType *, IppFFTSpec,
                            IppFFTMemBuf)>
        ippsFFTInv{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsFFTInv = ippsFFTInv_CToC_64fc;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsFFTInv = ippsFFTInv_CToC_32fc;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsFFTInv(source.begin(), dest.begin(),
                                m_FFTInit.fftSpec(), m_FFTInit.memBuf()));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64f> ||
                            std::is_same_v<T, Ipp32f>>
  inverse(IppVector const &sourceRe, IppVector const &sourceIm,
          IppVector &destRe, IppVector &destIm) {
    std::function<IppStatus(IppType const *, IppType const *, IppType *,
                            IppType *, IppFFTSpec, IppFFTMemBuf)>
        ippsFFTInv{};
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsFFTInv = ippsFFTInv_CToC_64f;
    } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsFFTInv = ippsFFTInv_CToC_32f;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    auto pSpec{m_FFTInit.fftSpec()};
    auto pMemBuf{m_FFTInit.memBuf()};
    IPP_CHECK_STATUS(ippsFFTInv(sourceRe.begin(), sourceIm.begin(),
                                destRe.begin(), destIm.begin(), pSpec,
                                pMemBuf));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64f> ||
                            std::is_same_v<T, Ipp32f>>
  inverse(IppVector &dataRe, IppVector &dataIm) {
    std::function<IppStatus(IppType *, IppType *, IppFFTSpec, IppFFTMemBuf)>
        ippsFFTInv{};
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      ippsFFTInv = ippsFFTInv_CToC_64f_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
      ippsFFTInv = ippsFFTInv_CToC_32f_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    auto pSpec{m_FFTInit.fftSpec()};
    auto pMemBuf{m_FFTInit.memBuf()};
    IPP_CHECK_STATUS(
        ippsFFTInv(dataRe.begin(), dataIm.begin(), pSpec, pMemBuf));
  }
  template <typename T = IppType>
  typename std::enable_if_t<std::is_same_v<T, Ipp64fc> ||
                            std::is_same_v<T, Ipp32fc>>
  inverse(IppVector &data) {
    std::function<IppStatus(IppType *, IppFFTSpec, IppFFTMemBuf)> ippsFFTInv{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsFFTInv = ippsFFTInv_CToC_64fc_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsFFTInv = ippsFFTInv_CToC_32fc_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    auto pSpec{m_FFTInit.fftSpec()};
    auto pMemBuf{m_FFTInit.memBuf()};
    IPP_CHECK_STATUS(ippsFFTInv(data.begin(), pSpec, pMemBuf));
  }

 private:
  Detail::CIppFFTInit<IppType> m_FFTInit{};
};

using CIppFFT64f = CIppFFT<Ipp64f>;
using CIppFFT64fc = CIppFFT<Ipp64fc>;
using CIppFFT32f = CIppFFT<Ipp32f>;
using CIppFFT32fc = CIppFFT<Ipp32fc>;

template <typename IppType>
class CIppFFTWindow {
 public:
  using ValueType = IppType;
  using IppVector = CIppVector<IppType>;
  using SizeType = typename IppVector::SizeType;
  static void Hann(IppVector &data) {
    std::function<IppStatus(IppType *, SizeType)> ippsWinHann{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsWinHann = ippsWinHann_64fc_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsWinHann = ippsWinHann_32fc_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsWinHann(data.begin(), data.size()));
  }
  static void Hamming(IppVector &data) {
    std::function<IppStatus(IppType *, SizeType)> ippsWinHamming{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsWinHamming = ippsWinHamming_64fc_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsWinHamming = ippsWinHamming_32fc_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsWinHamming(data.begin(), data.size()));
  }
  static void Bartlett(IppVector &data) {
    std::function<IppStatus(IppType *, SizeType)> ippsWinBartlett{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsWinBartlett = ippsWinBartlett_64fc_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsWinBartlett = ippsWinBartlett_32fc_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsWinBartlett(data.begin(), data.size()));
  }
  static void BlackmanStd(IppVector &data) {
    std::function<IppStatus(IppType *, SizeType)> ippsWinBlackmanStd{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsWinBlackmanStd = ippsWinBlackmanStd_64fc_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsWinBlackmanStd = ippsWinBlackmanStd_32fc_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsWinBlackmanStd(data.begin(), data.size()));
  }
  static void BlackmanOpt(IppVector &data) {
    std::function<IppStatus(IppType *, SizeType)> ippsWinBlackmanOpt{};
    if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      ippsWinBlackmanOpt = ippsWinBlackmanOpt_64fc_I;
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      ippsWinBlackmanOpt = ippsWinBlackmanOpt_32fc_I;
    } else {
      static_assert(std::is_void_v<IppType>, "unsupported type");
    }
    IPP_CHECK_STATUS(ippsWinBlackmanOpt(data.begin(), data.size()));
  }
};

}  // namespace IntelIppTL

#endif  // IIPPTL_FFT_HPP
