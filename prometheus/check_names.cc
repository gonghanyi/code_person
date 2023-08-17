// MIT License

// Copyright (c) 2016-2019 Jupp Mueller
// Copyright (c) 2017-2019 Gregor Jasny

// And many contributors, see
// https://github.com/jupp0r/prometheus-cpp/graphs/contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//   SOFTWARE.
//
// Mover: Qianqiong Zhang <zhangqianqiong@sensorsdata.cn>

#include "prometheus/check_names.h"

#include <algorithm>
#include <iterator>

namespace prometheus {

namespace {
bool isLocaleIndependentAlphaNumeric(char c) {
  return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z');
}

bool isLocaleIndependentDigit(char c) { return '0' <= c && c <= '9'; }

bool nameStartsValid(const std::string& name) {
  // must not be empty
  if (name.empty()) {
    return false;
  }

  // must not start with a digit
  if (isLocaleIndependentDigit(name.front())) {
    return false;
  }

  // must not start with "__"
  auto reserved_for_internal_purposes = name.compare(0, 2, "__") == 0;
  if (reserved_for_internal_purposes) return false;

  return true;
}
}  // anonymous namespace

/// \brief Check if the metric name is valid
///
/// The metric name regex is "[a-zA-Z_:][a-zA-Z0-9_:]*"
///
/// \see https://prometheus.io/docs/concepts/data_model/
///
/// \param name metric name
/// \return true is valid, false otherwise
bool CheckMetricName(const std::string& name) {
  if (!nameStartsValid(name)) {
    return false;
  }

  auto validMetricCharacters = [](char c) {
    return isLocaleIndependentAlphaNumeric(c) || c == '_' || c == ':';
  };

  auto mismatch =
      std::find_if_not(std::begin(name), std::end(name), validMetricCharacters);
  return mismatch == std::end(name);
}

/// \brief Check if the label name is valid
///
/// The label name regex is "[a-zA-Z_][a-zA-Z0-9_]*"
///
/// \see https://prometheus.io/docs/concepts/data_model/
///
/// \param name label name
/// \return true is valid, false otherwise
bool CheckLabelName(const std::string& name) {
  if (!nameStartsValid(name)) {
    return false;
  }

  auto validLabelCharacters = [](char c) {
    return isLocaleIndependentAlphaNumeric(c) || c == '_';
  };

  auto mismatch =
      std::find_if_not(std::begin(name), std::end(name), validLabelCharacters);
  return mismatch == std::end(name);
}
}  // namespace prometheus
