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

#include <gtest/gtest.h>

namespace prometheus {
namespace {

TEST(CheckNamesTest, empty_metric_name) { EXPECT_FALSE(CheckMetricName("")); }
TEST(CheckNamesTest, good_metric_name) {
  EXPECT_TRUE(CheckMetricName("prometheus_notifications_total"));
}
TEST(CheckNamesTest, reserved_metric_name) {
  EXPECT_FALSE(CheckMetricName("__some_reserved_metric"));
}
TEST(CheckNamesTest, malformed_metric_name) {
  EXPECT_FALSE(CheckMetricName("fa mi ly with space in name or |"));
}
TEST(CheckNamesTest, empty_label_name) { EXPECT_FALSE(CheckLabelName("")); }
TEST(CheckNamesTest, invalid_label_name) {
  EXPECT_FALSE(CheckLabelName("log-level"));
}
TEST(CheckNamesTest, leading_invalid_label_name) {
  EXPECT_FALSE(CheckLabelName("-abcd"));
}
TEST(CheckNamesTest, trailing_invalid_label_name) {
  EXPECT_FALSE(CheckLabelName("abcd-"));
}
TEST(CheckNamesTest, good_label_name) { EXPECT_TRUE(CheckLabelName("type")); }
TEST(CheckNamesTest, reserved_label_name) {
  EXPECT_FALSE(CheckMetricName("__some_reserved_label"));
}

}  // namespace
}  // namespace prometheus
