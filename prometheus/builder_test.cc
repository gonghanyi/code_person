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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/family.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/registry.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

class BuilderTest : public testing::Test {
 protected:
  std::vector<ClientMetric::Label> getExpectedLabels() {
    std::vector<ClientMetric::Label> labels;

    auto gen = [](std::pair<const std::string, std::string> p) {
      return ClientMetric::Label{p.first, p.second};
    };

    std::transform(std::begin(const_labels), std::end(const_labels),
                   std::back_inserter(labels), gen);
    std::transform(std::begin(more_labels), std::end(more_labels),
                   std::back_inserter(labels), gen);

    return labels;
  }

  void verifyCollectedLabels() {
    const auto collected = registry.Collect();

    ASSERT_EQ(1U, collected.size());
    EXPECT_EQ(name, collected.at(0).name);
    EXPECT_EQ(help, collected.at(0).help);
    ASSERT_EQ(1U, collected.at(0).metric.size());

    EXPECT_THAT(collected.at(0).metric.at(0).label,
                testing::ElementsAreArray(expected_labels.data(), expected_labels.size()));
  }

  Registry registry;

  const std::string name = "some_name";
  const std::string help = "Additional description.";
  const std::map<std::string, std::string> const_labels = {{"key", "value"}};
  const std::map<std::string, std::string> more_labels = {{"name", "test"}};
  const std::vector<ClientMetric::Label> expected_labels = getExpectedLabels();
};

TEST_F(BuilderTest, build_counter) {
  auto& family = BuildCounter()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(&registry);
  family.Add(more_labels);

  verifyCollectedLabels();
}

TEST_F(BuilderTest, build_gauge) {
  auto& family = BuildGauge()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(&registry);
  family.Add(more_labels);

  verifyCollectedLabels();
}

TEST_F(BuilderTest, build_histogram) {
  auto& family = BuildHistogram()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(&registry);
  family.Add(more_labels, Histogram::BucketBoundaries{1, 2});

  verifyCollectedLabels();
}

TEST_F(BuilderTest, build_summary) {
  auto& family = BuildSummary()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(&registry);
  family.Add(more_labels, Summary::Quantiles{});

  verifyCollectedLabels();
}

}  // namespace
}  // namespace prometheus
