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

#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <stdexcept>

namespace prometheus {
namespace {

TEST(HistogramTest, initialize_with_zero) {
  Histogram histogram{{}};
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.sample_count, 0U);
  EXPECT_EQ(h.sample_sum, 0);
}

TEST(HistogramTest, sample_count) {
  Histogram histogram{{1}};
  histogram.Observe(0);
  histogram.Observe(200);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.sample_count, 2U);
}

TEST(HistogramTest, sample_sum) {
  Histogram histogram{{1}};
  histogram.Observe(0);
  histogram.Observe(1);
  histogram.Observe(101);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.sample_sum, 102);
}

TEST(HistogramTest, bucket_size) {
  Histogram histogram{{1, 2}};
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.bucket.size(), 3U);
}

TEST(HistogramTest, bucket_bounds) {
  Histogram histogram{{1, 2}};
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.bucket.at(0).upper_bound, 1);
  EXPECT_EQ(h.bucket.at(1).upper_bound, 2);
  EXPECT_EQ(h.bucket.at(2).upper_bound,
            std::numeric_limits<double>::infinity());
}

TEST(HistogramTest, bucket_counts_not_reset_by_collection) {
  Histogram histogram{{1, 2}};
  histogram.Observe(1.5);
  histogram.Collect();
  histogram.Observe(1.5);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  ASSERT_EQ(h.bucket.size(), 3U);
  EXPECT_EQ(h.bucket.at(1).cumulative_count, 2U);
}

TEST(HistogramTest, cumulative_bucket_count) {
  Histogram histogram{{1, 2}};
  histogram.Observe(0);
  histogram.Observe(0.5);
  histogram.Observe(1);
  histogram.Observe(1.5);
  histogram.Observe(1.5);
  histogram.Observe(2);
  histogram.Observe(3);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  ASSERT_EQ(h.bucket.size(), 3U);
  EXPECT_EQ(h.bucket.at(0).cumulative_count, 3U);
  EXPECT_EQ(h.bucket.at(1).cumulative_count, 6U);
  EXPECT_EQ(h.bucket.at(2).cumulative_count, 7U);
}

TEST(HistogramTest, observe_multiple_test_bucket_counts) {
  Histogram histogram{{1, 2}};
  histogram.ObserveMultiple({5, 9, 3}, 20);
  histogram.ObserveMultiple({0, 20, 6}, 34);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  ASSERT_EQ(h.bucket.size(), 3U);
  EXPECT_EQ(h.bucket.at(0).cumulative_count, 5U);
  EXPECT_EQ(h.bucket.at(1).cumulative_count, 34U);
  EXPECT_EQ(h.bucket.at(2).cumulative_count, 43U);
}

TEST(HistogramTest, observe_multiple_test_total_sum) {
  Histogram histogram{{1, 2}};
  histogram.ObserveMultiple({5, 9, 3}, 20);
  histogram.ObserveMultiple({0, 20, 6}, 34);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.sample_count, 43U);
  EXPECT_EQ(h.sample_sum, 54);
}

TEST(HistogramTest, observe_multiple_test_length_error) {
  Histogram histogram{{1, 2}};
  // 2 bucket boundaries means there are 3 buckets, so giving just 2 bucket
  // increments should result in a length_error.
  ASSERT_THROW(histogram.ObserveMultiple({5, 9}, 20), std::length_error);
}

TEST(HistogramTest, sum_can_go_down) {
  Histogram histogram{{1}};
  auto metric1 = histogram.Collect();
  histogram.Observe(-10);
  auto metric2 = histogram.Collect();
  EXPECT_LT(metric2.histogram.sample_sum, metric1.histogram.sample_sum);
}

}  // namespace
}  // namespace prometheus
