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

#ifndef IIPPTL_ACCURACY_HPP
#define IIPPTL_ACCURACY_HPP

namespace IntelIppTL {

namespace AccuracyLevels {
enum class SinglePrecision { A11, A21, A24 };
enum class DoublePrecision { A26, A50, A53 };
}  // namespace AccuracyLevels

}  // namespace IntelIppTL

#endif  // IIPPTL_ACCURACY_HPP
