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

#include "prometheus/family.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "prometheus/check_names.h"
#include "prometheus/counter.h"
#include "prometheus/detail/utils.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

namespace prometheus {

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  const std::map<std::string, std::string>& constant_labels)
    : name_(name), help_(help), constant_labels_(constant_labels) {
  if (!CheckMetricName(name_)) {
    throw std::invalid_argument("Invalid metric name");
  }
  for (auto& label_pair : constant_labels_) {
    auto& label_name = label_pair.first;
    if (!CheckLabelName(label_name)) {
      throw std::invalid_argument("Invalid label name");
    }
  }
}

template <typename T>
T& Family<T>::Add(const std::map<std::string, std::string>& labels,
                  std::unique_ptr<T> object) {
  const auto hash = detail::hash_labels(labels);
  std::lock_guard<std::mutex> lock{mutex_};
  auto metrics_iter = metrics_.find(hash);

  if (metrics_iter != metrics_.end()) {
#ifndef NDEBUG
    auto labels_iter = labels_.find(hash);
    assert(labels_iter != labels_.end());
    const auto& old_labels = labels_iter->second;
    assert(labels == old_labels);
#endif
    return *metrics_iter->second;
  }

  for (auto& label_pair : labels) {
    const auto& label_name = label_pair.first;
    if (!CheckLabelName(label_name)) {
      throw std::invalid_argument("Invalid label name");
    }
    if (constant_labels_.count(label_name)) {
      throw std::invalid_argument("Duplicate label name");
    }
  }

  const auto metric = metrics_.insert(std::make_pair(hash, std::move(object)));
  assert(metric.second);
  labels_.insert({hash, labels});
  labels_reverse_lookup_.insert({metric.first->second.get(), hash});
  return *(metric.first->second);
}

template <typename T>
void Family<T>::Remove(T* metric) {
  std::lock_guard<std::mutex> lock{mutex_};
  if (labels_reverse_lookup_.count(metric) == 0) {
    return;
  }

  const auto hash = labels_reverse_lookup_.at(metric);
  metrics_.erase(hash);
  labels_.erase(hash);
  labels_reverse_lookup_.erase(metric);
}

template <typename T>
bool Family<T>::Has(const std::map<std::string, std::string>& labels) const {
  const auto hash = detail::hash_labels(labels);
  std::lock_guard<std::mutex> lock{mutex_};
  return metrics_.find(hash) != metrics_.end();
}

template <typename T>
const std::string& Family<T>::GetName() const {
  return name_;
}

template <typename T>
const std::map<std::string, std::string> Family<T>::GetConstantLabels() const {
  return constant_labels_;
}

template <typename T>
std::vector<MetricFamily> Family<T>::Collect() const {
  std::lock_guard<std::mutex> lock{mutex_};

  if (metrics_.empty()) {
    return {};
  }

  auto family = MetricFamily{};
  family.name = name_;
  family.help = help_;
  family.type = T::metric_type;
  family.metric.reserve(metrics_.size());
  for (const auto& m : metrics_) {
    family.metric.push_back(std::move(CollectMetric(m.first, m.second.get())));
  }
  return {family};
}

template <typename T>
ClientMetric Family<T>::CollectMetric(std::size_t hash, T* metric) const {
  auto collected = metric->Collect();
  const auto add_label =
      [&collected](const std::pair<std::string, std::string>& label_pair) {
        auto label = ClientMetric::Label{};
        label.name = label_pair.first;
        label.value = label_pair.second;
        collected.label.push_back(std::move(label));
      };
  std::for_each(constant_labels_.cbegin(), constant_labels_.cend(), add_label);
  const auto& metric_labels = labels_.at(hash);
  std::for_each(metric_labels.cbegin(), metric_labels.cend(), add_label);
  return collected;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-template-vtables"
template class Family<Counter>;
template class Family<Gauge>;
template class Family<Histogram>;
template class Family<Summary>;
#pragma clang diagnostic pop

}  // namespace prometheus
