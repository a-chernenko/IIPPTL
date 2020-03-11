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

#ifndef IIPPTL_FFT_INIT_HPP
#define IIPPTL_FFT_INIT_HPP

#include <memory>

#include "iipptl_allocator.hpp"
#include "iipptl_fft_param.hpp"

namespace IntelIppTL {

namespace Detail {

// FIXME: это HACK от которого НУЖНО уйти!!!
template <typename IppType>
class CIppFFTSpec {
  void *ptr{};

 public:
  constexpr CIppFFTSpec() = default;
  constexpr auto pointer() {
    if constexpr (std::is_same_v<IppType, Ipp64f>) {
      return static_cast<IppsFFTSpec_C_64f *>(ptr);
    } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
      return static_cast<IppsFFTSpec_C_64fc *>(ptr);
    } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
      return static_cast<IppsFFTSpec_C_32f *>(ptr);
    } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
      return static_cast<IppsFFTSpec_C_32fc *>(ptr);
    }
  }
};

template <typename IppType>
class CIppFFTInit {
  using IppFFTParam = CIppFFTParam<IppType>;
  using CustomDeleter = std::function<decltype(CIppAllocator::deallocate)>;
  using UniquePtr = std::unique_ptr<Ipp8u[], CustomDeleter>;

 public:
  using ValueType = IppType;
  using FFTSpec = decltype(CIppFFTSpec<ValueType>().pointer());
  using MemBuf = Ipp8u *;
  using SizeType = typename IppFFTParam::SizeType;
  CIppFFTInit() = default;
  CIppFFTInit(IppFFTParam const &param) { reinit(param); }
  CIppFFTInit(CIppFFTInit const &other) { operator=(other); }
  CIppFFTInit(CIppFFTInit &&) = default;
  CIppFFTInit &operator=(CIppFFTInit const &other) {
    reinit(other.m_Parameters);
    return *this;
  }
  CIppFFTInit &operator=(CIppFFTInit &&) = default;
  virtual ~CIppFFTInit() noexcept = default;
  void reinit(CIppFFTParam<IppType> const &param) {
    m_Parameters = param;
    auto memSpecTmp{UniquePtr{CIppAllocator::allocate<Ipp8u>(param.specSize()),
                              CIppAllocator::deallocate}};
    m_MemSpec.swap(memSpecTmp);
    auto memInitTmp{UniquePtr{CIppAllocator::allocate<Ipp8u>(param.initSize()),
                              CIppAllocator::deallocate}};
    m_MemInit.swap(memInitTmp);
    auto memBufferTmp{
        UniquePtr{CIppAllocator::allocate<Ipp8u>(param.bufferSize()),
                  CIppAllocator::deallocate}};
    m_MemBuffer.swap(memBufferTmp);
    auto status = [](auto **ppSpec, int order, int flag, IppHintAlgorithm hint,
                     Ipp8u *pSpec, Ipp8u *pSpecBuffer) -> IppStatus {
      IppStatus status{ippStsErr};
      if constexpr (std::is_same_v<IppType, Ipp64f>) {
        status =
            ippsFFTInit_C_64f(ppSpec, order, flag, hint, pSpec, pSpecBuffer);
      } else if constexpr (std::is_same_v<IppType, Ipp64fc>) {
        status =
            ippsFFTInit_C_64fc(ppSpec, order, flag, hint, pSpec, pSpecBuffer);
      } else if constexpr (std::is_same_v<IppType, Ipp32f>) {
        status =
            ippsFFTInit_C_32f(ppSpec, order, flag, hint, pSpec, pSpecBuffer);
      } else if constexpr (std::is_same_v<IppType, Ipp32fc>) {
        status =
            ippsFFTInit_C_32fc(ppSpec, order, flag, hint, pSpec, pSpecBuffer);
      }
      return status;
    }(&m_pFFTSpec, param.order(), int(param.flag()), param.hint(),
                                                       m_MemSpec.get(),
                                                       m_MemInit.get());
    IPP_CHECK_STATUS(status);
  }
  inline FFTSpec const fftSpec() const noexcept { return m_pFFTSpec; }
  inline MemBuf memBuf() const noexcept { return m_MemBuffer.get(); }

 private:
  FFTSpec m_pFFTSpec{};
  IppFFTParam m_Parameters{};
  UniquePtr m_MemSpec{};
  UniquePtr m_MemInit{};
  UniquePtr m_MemBuffer{};
};

using CIppFFTInit64f = CIppFFTInit<Ipp64f>;
using CIppFFTInit64fc = CIppFFTInit<Ipp64fc>;
using CIppFFTInit32f = CIppFFTInit<Ipp32f>;
using CIppFFTInit32fc = CIppFFTInit<Ipp32fc>;

}  // namespace Detail

}  // namespace IntelIppTL

#endif  // IIPPTL_FFT_INIT_HPP
