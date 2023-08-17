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

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <utility>

#include "prometheus/counter.h"
#include "prometheus/detail/future_std.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

namespace prometheus {

namespace {
template <typename T>
void CollectAll(const T& families, std::vector<MetricFamily>* results) {
  for (auto&& collectable : families) { //NOLINT
    auto metrics = collectable->Collect();
    results->insert(results->end(), std::make_move_iterator(metrics.begin()),
                   std::make_move_iterator(metrics.end()));
  }
}

bool FamilyNameExists(const std::string& /* name */) { return false; }

template <typename T, typename... Args>
bool FamilyNameExists(const std::string& name, const T& families,
                      Args&&... args) {
  auto sameName = [&name](const typename T::value_type& entry) {
    return name == entry->GetName();
  };
  auto exists = std::find_if(std::begin(families), std::end(families),
                             sameName) != std::end(families);
  return exists || FamilyNameExists(name, args...);
}
}  // namespace

Registry::Registry(InsertBehavior insert_behavior)
    : insert_behavior_{insert_behavior} {}

Registry::~Registry() = default;

std::vector<MetricFamily> Registry::Collect() const {
  std::lock_guard<std::mutex> lock{mutex_};
  auto results = std::vector<MetricFamily>{};

  CollectAll(counters_, &results);
  CollectAll(gauges_, &results);
  CollectAll(histograms_, &results);
  CollectAll(summaries_, &results);

  return results;
}

template <>
std::vector<std::unique_ptr<Family<Counter>>>& Registry::GetFamilies() {
  return counters_;
}

template <>
std::vector<std::unique_ptr<Family<Gauge>>>& Registry::GetFamilies() {
  return gauges_;
}

template <>
std::vector<std::unique_ptr<Family<Histogram>>>& Registry::GetFamilies() {
  return histograms_;
}

template <>
std::vector<std::unique_ptr<Family<Summary>>>& Registry::GetFamilies() {
  return summaries_;
}

template <>
bool Registry::NameExistsInOtherType<Counter>(const std::string& name) const {
  return FamilyNameExists(name, gauges_, histograms_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Gauge>(const std::string& name) const {
  return FamilyNameExists(name, counters_, histograms_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Histogram>(const std::string& name) const {
  return FamilyNameExists(name, counters_, gauges_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Summary>(const std::string& name) const {
  return FamilyNameExists(name, counters_, gauges_, histograms_);
}

template <typename T>
Family<T>& Registry::Add(const std::string& name, const std::string& help,
                         const std::map<std::string, std::string>& labels) {
  std::lock_guard<std::mutex> lock{mutex_};

  if (NameExistsInOtherType<T>(name)) {
    throw std::invalid_argument(
        "Family name already exists with different type");
  }

  auto& families = GetFamilies<T>();

  if (insert_behavior_ == InsertBehavior::Merge) {
    auto same_name_and_labels =
        [&name, &labels](const std::unique_ptr<Family<T>>& family) {
          return std::tie(name, labels) ==
                 std::tie(family->GetName(), family->GetConstantLabels());
        };

    auto it =
        std::find_if(families.begin(), families.end(), same_name_and_labels);
    if (it != families.end()) {
      return **it;
    }
  }

  if (insert_behavior_ != InsertBehavior::NonStandardAppend) {
    auto same_name = [&name](const std::unique_ptr<Family<T>>& family) {
      return name == family->GetName();
    };

    auto it = std::find_if(families.begin(), families.end(), same_name);
    if (it != families.end()) {
      throw std::invalid_argument("Family name already exists");
    }
  }

  auto family = detail::make_unique<Family<T>>(name, help, labels);
  auto& ref = *family;
  families.push_back(std::move(family));
  return ref;
}

template Family<Counter>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels);

template Family<Gauge>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels);

template Family<Summary>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels);

template Family<Histogram>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels);

}  // namespace prometheus
