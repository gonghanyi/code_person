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

#include "prometheus/histogram.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace prometheus {

Histogram::Histogram(const BucketBoundaries& buckets)
    : bucket_boundaries_{buckets}, bucket_counts_{buckets.size() + 1}, sum_{} {
  assert(std::is_sorted(std::begin(bucket_boundaries_),
                        std::end(bucket_boundaries_)));
}

void Histogram::Observe(const double value) {
  // TODO(none): determine bucket list size at which binary search would be faster
  const auto bucket_index = static_cast<std::size_t>(std::distance(
      bucket_boundaries_.begin(),
      std::find_if(
          std::begin(bucket_boundaries_), std::end(bucket_boundaries_),
          [value](const double boundary) { return boundary >= value; })));
  sum_.Increment(value);
  bucket_counts_[bucket_index].Increment();
}

void Histogram::ObserveMultiple(const std::vector<double>& bucket_increments,
                                const double sum_of_values) {
  if (bucket_increments.size() != bucket_counts_.size()) {
    throw std::length_error(
        "The size of bucket_increments was not equal to"
        "the number of buckets in the histogram.");
  }

  sum_.Increment(sum_of_values);

  for (std::size_t i{0}; i < bucket_counts_.size(); ++i) {
    bucket_counts_[i].Increment(bucket_increments[i]);
  }
}

ClientMetric Histogram::Collect() const {
  auto metric = ClientMetric{};

  auto cumulative_count = 0ULL;
  metric.histogram.bucket.reserve(bucket_counts_.size());
  for (std::size_t i{0}; i < bucket_counts_.size(); ++i) {
    cumulative_count += bucket_counts_[i].Value();
    auto bucket = ClientMetric::Bucket{};
    bucket.cumulative_count = cumulative_count;
    bucket.upper_bound = (i == bucket_boundaries_.size()
                              ? std::numeric_limits<double>::infinity()
                              : bucket_boundaries_[i]);
    metric.histogram.bucket.push_back(std::move(bucket));
  }
  metric.histogram.sample_count = cumulative_count;
  metric.histogram.sample_sum = sum_.Value();

  return metric;
}

}  // namespace prometheus
