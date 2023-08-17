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

#include "prometheus/text_serializer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <string>

#include "prometheus/client_metric.h"
#include "prometheus/histogram.h"
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

class TextSerializerTest : public testing::Test {
 public:
  std::string Serialize(MetricType type) const {
    MetricFamily metricFamily;
    metricFamily.name = name;
    metricFamily.help = "my metric help text";
    metricFamily.type = type;
    metricFamily.metric = std::vector<ClientMetric>{metric};

    std::vector<MetricFamily> families{metricFamily};

    return textSerializer.Serialize(families);
  }

  const std::string name = "my_metric";
  ClientMetric metric;
  TextSerializer textSerializer;
};

TEST_F(TextSerializerTest, shouldSerializeNotANumber) {
  metric.gauge.value = std::nan("");
  EXPECT_THAT(Serialize(MetricType::Gauge), testing::HasSubstr(name + " Nan"));
}

TEST_F(TextSerializerTest, shouldSerializeNegativeInfinity) {
  metric.gauge.value = -std::numeric_limits<double>::infinity();
  EXPECT_THAT(Serialize(MetricType::Gauge), testing::HasSubstr(name + " -Inf"));
}

TEST_F(TextSerializerTest, shouldSerializePositiveInfinity) {
  metric.gauge.value = std::numeric_limits<double>::infinity();
  EXPECT_THAT(Serialize(MetricType::Gauge), testing::HasSubstr(name + " +Inf"));
}

TEST_F(TextSerializerTest, shouldEscapeBackslash) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\\v"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\\\v\"}"));
}

TEST_F(TextSerializerTest, shouldEscapeNewline) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\nv"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\nv\"}"));
}

TEST_F(TextSerializerTest, shouldEscapeDoubleQuote) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\"v"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\\"v\"}"));
}

TEST_F(TextSerializerTest, shouldSerializeUntyped) {
  metric.untyped.value = 64.0;

  const auto serialized = Serialize(MetricType::Untyped);
  EXPECT_THAT(serialized, testing::HasSubstr(name + " 64\n"));
}

TEST_F(TextSerializerTest, shouldSerializeTimestamp) {
  metric.counter.value = 64.0;
  metric.timestamp_ms = 1234;

  const auto serialized = Serialize(MetricType::Counter);
  EXPECT_THAT(serialized, testing::HasSubstr(name + " 64 1234\n"));
}

TEST_F(TextSerializerTest, shouldSerializeHistogramWithNoBuckets) {
  metric.histogram.sample_count = 2;
  metric.histogram.sample_sum = 32.0;

  const auto serialized = Serialize(MetricType::Histogram);
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 32\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_bucket{le=\"+Inf\"} 2"));
}

TEST_F(TextSerializerTest, shouldSerializeHistogram) {
  Histogram histogram{{1}};
  histogram.Observe(0);
  histogram.Observe(200);
  metric = histogram.Collect();

  const auto serialized = Serialize(MetricType::Histogram);
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 200\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_bucket{le=\"1\"} 1\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr(name + "_bucket{le=\"+Inf\"} 2\n"));
}

TEST_F(TextSerializerTest, shouldSerializeSummary) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(200);
  metric = summary.Collect();

  const auto serialized = Serialize(MetricType::Summary);
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 200\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "{quantile=\"0.5\"} 0\n"));
}

}  // namespace
}  // namespace prometheus
