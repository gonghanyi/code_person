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

#include "prometheus/detail/builder.h"

#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/registry.h"
#include "prometheus/summary.h"

namespace prometheus {

namespace detail {

template <typename T>
Builder<T>& Builder<T>::Labels(
    const std::map<std::string, std::string>& labels) {
  labels_ = labels;
  return *this;
}

template <typename T>
Builder<T>& Builder<T>::Name(const std::string& name) {
  name_ = name;
  return *this;
}

template <typename T>
Builder<T>& Builder<T>::Help(const std::string& help) {
  help_ = help;
  return *this;
}

template <typename T>
Family<T>& Builder<T>::Register(Registry* registry) {
  return registry->Add<T>(name_, help_, labels_);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-template-vtables"
template class Builder<Counter>;
template class Builder<Gauge>;
template class Builder<Histogram>;
template class Builder<Summary>;
#pragma clang diagnostic pop

}  // namespace detail

detail::Builder<Counter> BuildCounter() { return {}; }
detail::Builder<Gauge> BuildGauge() { return {}; }
detail::Builder<Histogram> BuildHistogram() { return {}; }
detail::Builder<Summary> BuildSummary() { return {}; }

}  // namespace prometheus
