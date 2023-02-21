/*
 *  Copyright 2017 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "string_to_number.h"

#include <ctype.h>
#include <cerrno>
#include <cstdlib>

#include "checks.h"

namespace rtc {
namespace string_to_number_internal {

signed_type ParseSigned(const char* str, int base) {
  RTC_DCHECK(str);
  if (isdigit(str[0]) || str[0] == '-') {
    char* end = nullptr;
    errno = 0;
    const signed_type value = std::strtoll(str, &end, base);
    if (end && *end == '\0' && errno == 0) {
      return value;
    }
  }
  return 0;
}

unsigned_type ParseUnsigned(const char* str, int base) {
  RTC_DCHECK(str);
  if (isdigit(str[0]) || str[0] == '-') {
    // Explicitly discard negative values. std::strtoull parsing causes unsigned
    // wraparound. We cannot just reject values that start with -, though, since
    // -0 is perfectly fine, as is -0000000000000000000000000000000.
    const bool is_negative = str[0] == '-';
    char* end = nullptr;
    errno = 0;
    const unsigned_type value = std::strtoull(str, &end, base);
    if (end && *end == '\0' && errno == 0 && (value == 0 || !is_negative)) {
      return value;
    }
  }
  return 0;
}

template <typename T>
T StrToT(const char* str, char** str_end);

template <>
inline float StrToT(const char* str, char** str_end) {
  return std::strtof(str, str_end);
}

template <>
inline double StrToT(const char* str, char** str_end) {
  return std::strtod(str, str_end);
}

template <>
inline long double StrToT(const char* str, char** str_end) {
  return std::strtold(str, str_end);
}

template <typename T>
T ParseFloatingPoint(const char* str) {
  RTC_DCHECK(str);
  if (*str == '\0')
    return 0.0;
  char* end = nullptr;
  errno = 0;
  const T value = StrToT<T>(str, &end);
  if (end && *end == '\0' && errno == 0) {
    return value;
  }
  return 0.0;
}

template float ParseFloatingPoint(const char* str);
template double ParseFloatingPoint(const char* str);
template long double ParseFloatingPoint(const char* str);

}  // namespace string_to_number_internal
}  // namespace rtc
