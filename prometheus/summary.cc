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

#include "prometheus/summary.h"

#include <utility>
#include <iostream>

namespace prometheus {

Summary::Summary(const Quantiles& quantiles,
                 const std::chrono::milliseconds max_age, const int age_buckets)
    : quantiles_{quantiles},
      count_{0},
      sum_{0},
      quantile_values_{quantiles_, max_age, age_buckets} {}

void Summary::Observe(const double value) {
  Clock::time_point time1 = Clock::now();
  std::lock_guard<std::mutex> lock(mutex_);
  auto delta = Clock::now() - time1;
  int cnt = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
  if (cnt > 1000000) {
    std::cout << "Summary::Observe,lock(time) = " << cnt  << ", value = " << value << std::endl;
  }

  count_ += 1;
  sum_ += value;
  quantile_values_.insert(value);
}

ClientMetric Summary::Collect() const {
  auto metric = ClientMetric{};

  std::lock_guard<std::mutex> lock(mutex_);

  metric.summary.quantile.reserve(quantiles_.size());
  int index = 0;
  for (const auto& quantile : quantiles_) {
    std::cout << "index = " << index++ << ", ";
    auto metricQuantile = ClientMetric::Quantile{};
    metricQuantile.quantile = quantile.quantile;
    std::cout << "first: quantile.quantile = " << quantile.quantile << ", ";
    metricQuantile.value = quantile_values_.get(quantile.quantile);
    std::cout << ", second: quantile_value = " << metricQuantile.value << std::endl;
    metric.summary.quantile.push_back(std::move(metricQuantile));
  }
  metric.summary.sample_count = count_;
  metric.summary.sample_sum = sum_;

  return metric;
}

}  // namespace prometheus
