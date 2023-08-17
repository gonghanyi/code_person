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

#include "prometheus/registry.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

TEST(RegistryTest, collect_single_metric_family) {
  Registry registry{};
  auto& counter_family =
      BuildCounter().Name("test").Help("a test").Register(&registry);
  counter_family.Add({{"name", "counter1"}});
  counter_family.Add({{"name", "counter2"}});
  auto collected = registry.Collect();
  ASSERT_EQ(collected.size(), 1U);
  EXPECT_EQ(collected[0].name, "test");
  EXPECT_EQ(collected[0].help, "a test");
  ASSERT_EQ(collected[0].metric.size(), 2U);
  ASSERT_EQ(collected[0].metric.at(0).label.size(), 1U);
  EXPECT_EQ(collected[0].metric.at(0).label.at(0).name, "name");
  ASSERT_EQ(collected[0].metric.at(1).label.size(), 1U);
  EXPECT_EQ(collected[0].metric.at(1).label.at(0).name, "name");
}

TEST(RegistryTest, build_histogram_family) {
  Registry registry{};
  auto& histogram_family =
      BuildHistogram().Name("hist").Help("Test Histogram").Register(&registry);
  auto& histogram = histogram_family.Add({{"name", "test_histogram_1"}},
                                         Histogram::BucketBoundaries{0, 1, 2});
  histogram.Observe(1.1);
  auto collected = registry.Collect();
  ASSERT_EQ(collected.size(), 1U);
}

TEST(RegistryTest, reject_different_type_than_counter) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildCounter().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildGauge().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildHistogram().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildSummary().Name(same_name).Register(&registry));
}

TEST(RegistryTest, reject_different_type_than_gauge) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildGauge().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildHistogram().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildSummary().Name(same_name).Register(&registry));
}

TEST(RegistryTest, reject_different_type_than_histogram) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildHistogram().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildGauge().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildSummary().Name(same_name).Register(&registry));
}

TEST(RegistryTest, reject_different_type_than_summary) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildSummary().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildGauge().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildHistogram().Name(same_name).Register(&registry));
}

TEST(RegistryTest, append_same_families) {
  Registry registry{Registry::InsertBehavior::NonStandardAppend};

  std::size_t loops = 4;

  while (loops-- > 0) {
    BuildCounter()
        .Name("counter")
        .Help("Test Counter")
        .Register(&registry)
        .Add({{"name", "test_counter"}});
  }

  auto collected = registry.Collect();
  EXPECT_EQ(4U, collected.size());
}

TEST(RegistryTest, throw_for_same_family_name) {
  const auto same_name = std::string{"same_name"};
  Registry registry{Registry::InsertBehavior::Throw};

  EXPECT_NO_THROW(BuildCounter().Name(same_name).Register(&registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(&registry));
}

TEST(RegistryTest, merge_same_families) {
  Registry registry{Registry::InsertBehavior::Merge};

  std::size_t loops = 4;

  while (loops-- > 0) {
    BuildCounter()
        .Name("counter")
        .Help("Test Counter")
        .Register(&registry)
        .Add({{"name", "test_counter"}});
  }

  auto collected = registry.Collect();
  EXPECT_EQ(1U, collected.size());
}

TEST(RegistryTest, do_not_merge_families_with_different_labels) {
  Registry registry{Registry::InsertBehavior::Merge};

  EXPECT_NO_THROW(BuildCounter()
                      .Name("counter")
                      .Help("Test Counter")
                      .Labels({{"a", "A"}})
                      .Register(&registry));

  EXPECT_ANY_THROW(BuildCounter()
                       .Name("counter")
                       .Help("Test Counter")
                       .Labels({{"b", "B"}})
                       .Register(&registry));
}

}  // namespace
}  // namespace prometheus
