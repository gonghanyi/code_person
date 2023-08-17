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

#include "prometheus/gauge.h"

#include <gtest/gtest.h>

namespace prometheus {
namespace {

TEST(GaugeTest, initialize_with_zero) {
  Gauge gauge;
  EXPECT_EQ(gauge.Value(), 0);
}

TEST(GaugeTest, inc) {
  Gauge gauge;
  gauge.Increment();
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, inc_number) {
  Gauge gauge;
  gauge.Increment(4);
  EXPECT_EQ(gauge.Value(), 4.0);
}

TEST(GaugeTest, inc_multiple) {
  Gauge gauge;
  gauge.Increment();
  gauge.Increment();
  gauge.Increment(5);
  EXPECT_EQ(gauge.Value(), 7.0);
}

TEST(GaugeTest, inc_negative_value) {
  Gauge gauge;
  gauge.Increment(-1.0);
  EXPECT_EQ(gauge.Value(), -1.0);
}

TEST(GaugeTest, dec) {
  Gauge gauge;
  gauge.Set(5.0);
  gauge.Decrement();
  EXPECT_EQ(gauge.Value(), 4.0);
}

TEST(GaugeTest, dec_negative_value) {
  Gauge gauge;
  gauge.Decrement(-1.0);
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, dec_number) {
  Gauge gauge;
  gauge.Set(5.0);
  gauge.Decrement(3.0);
  EXPECT_EQ(gauge.Value(), 2.0);
}

TEST(GaugeTest, set) {
  Gauge gauge;
  gauge.Set(3.0);
  EXPECT_EQ(gauge.Value(), 3.0);
}

TEST(GaugeTest, set_multiple) {
  Gauge gauge;
  gauge.Set(3.0);
  gauge.Set(8.0);
  gauge.Set(1.0);
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, set_to_current_time) {
  Gauge gauge;
  gauge.SetToCurrentTime();
  EXPECT_GT(gauge.Value(), 0.0);
}

}  // namespace
}  // namespace prometheus
