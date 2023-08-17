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

#include <gtest/gtest.h>

#include <chrono>  // NOLINT
#include <cmath>
#include <limits>
#include <memory>
#include <thread>  // NOLINT

namespace prometheus {
namespace {

TEST(SummaryTest, initialize_with_zero) {
  Summary summary{Summary::Quantiles{}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 0U);
  EXPECT_EQ(s.sample_sum, 0);
}

TEST(SummaryTest, sample_count) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(200);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 2U);
}

TEST(SummaryTest, sample_sum) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(1);
  summary.Observe(101);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_sum, 102);
}

TEST(SummaryTest, quantile_size) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.90, 0.01}}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.quantile.size(), 2U);
}

TEST(SummaryTest, quantile_bounds) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.90, 0.01}, {0.99, 0.001}}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);
  EXPECT_DOUBLE_EQ(s.quantile.at(0).quantile, 0.5);
  EXPECT_DOUBLE_EQ(s.quantile.at(1).quantile, 0.9);
  EXPECT_DOUBLE_EQ(s.quantile.at(2).quantile, 0.99);
}

TEST(SummaryTest, quantile_values) {
  static const int SAMPLES = 1000000;

  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.9, 0.01}, {0.99, 0.001}}};
  for (int i = 1; i <= SAMPLES; ++i) summary.Observe(i);

  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);

  EXPECT_NEAR(s.quantile.at(0).value, 0.5 * SAMPLES, 0.05 * SAMPLES);
  EXPECT_NEAR(s.quantile.at(1).value, 0.9 * SAMPLES, 0.01 * SAMPLES);
  EXPECT_NEAR(s.quantile.at(2).value, 0.99 * SAMPLES, 0.001 * SAMPLES);
}

TEST(SummaryTest, max_age) {
  Summary summary{Summary::Quantiles{{0.99, 0.001}}, std::chrono::seconds(1),
                  2};
  summary.Observe(8.0);

  const auto test_value = [&summary](double ref) {
    auto metric = summary.Collect();
    auto s = metric.summary;
    ASSERT_EQ(s.quantile.size(), 1U);

    if (std::isnan(ref))
      EXPECT_TRUE(std::isnan(s.quantile.at(0).value));
    else
      EXPECT_DOUBLE_EQ(s.quantile.at(0).value, ref);
  };

  test_value(8.0);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  test_value(8.0);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  test_value(std::numeric_limits<double>::quiet_NaN());
}

TEST(SummaryTest, construction_with_dynamic_quantile_vector) {
  auto quantiles = Summary::Quantiles{{0.99, 0.001}};
  quantiles.push_back({0.5, 0.05});

  Summary summary{quantiles, std::chrono::seconds(1), 2};
  summary.Observe(8.0);
}

}  // namespace
}  // namespace prometheus
