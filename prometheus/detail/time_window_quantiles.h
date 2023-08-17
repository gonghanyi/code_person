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

#ifndef IMPALA_PROMETHEUS_DETAIL_TIME_WINDOW_QUANTILES_H_
#define IMPALA_PROMETHEUS_DETAIL_TIME_WINDOW_QUANTILES_H_
#pragma once

#include <chrono>  // NOLINT
#include <cstddef>
#include <vector>

#include "prometheus/detail/ckms_quantiles.h"  // IWYU pragma: export

// IWYU pragma: private, include "prometheus/summary.h"

namespace prometheus {
namespace detail {

class TimeWindowQuantiles {
 public:  
  using Clock = std::chrono::steady_clock;
  TimeWindowQuantiles(const std::vector<CKMSQuantiles::Quantile>& quantiles,
                      Clock::duration max_age_seconds, int age_buckets);

  double get(double q) const;
  void insert(double value);

 private:
  CKMSQuantiles& rotate() const;

  const std::vector<CKMSQuantiles::Quantile>& quantiles_;
  mutable std::vector<CKMSQuantiles> ckms_quantiles_;
  mutable std::size_t current_bucket_;

  mutable Clock::time_point last_rotation_;
  const Clock::duration rotation_interval_;
};

}  // namespace detail
}  // namespace prometheus

#endif  // IMPALA_PROMETHEUS_DETAIL_TIME_WINDOW_QUANTILES_H_
