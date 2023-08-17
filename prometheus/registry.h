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

#ifndef IMPALA_PROMETHEUS_REGISTRY_H_
#define IMPALA_PROMETHEUS_REGISTRY_H_
#pragma once

#include <map>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <vector>

#include "prometheus/collectable.h"
#include "prometheus/family.h"
#include "prometheus/metric_family.h"

namespace prometheus {

class Counter;
class Gauge;
class Histogram;
class Summary;

namespace detail {

template <typename T>
class Builder;  // IWYU pragma: keep

}
/// \brief Manages the collection of a number of metrics.
///
/// The Registry is responsible to expose data to a class/method/function
/// "bridge", which returns the metrics in a format Prometheus supports.
///
/// The key class is the Collectable. This has a method - called Collect() -
/// that returns zero or more metrics and their samples. The metrics are
/// represented by the class Family<>, which implements the Collectable
/// interface. A new metric is registered with BuildCounter(), BuildGauge(),
/// BuildHistogram() or BuildSummary().
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class Registry : public Collectable {
 public:
  /// \brief How to deal with repeatedly added family names for a type.
  ///
  /// Adding a family with the same name but different types is always an error
  /// and will lead to an exception.
  enum class InsertBehavior {
    /// \brief If a family with the same name and labels already exists return
    /// the existing one. If no family with that name exists create it.
    /// Otherwise throw.
    Merge,
    /// \brief Throws if a family with the same name already exists.
    Throw,
    /// \brief Never merge and always create a new family. This violates the
    /// prometheus specification but was the default behavior in earlier
    /// versions
    NonStandardAppend,
  };

  /// \brief name Create a new registry.
  ///
  /// \param insert_behavior How to handle families with the same name.
  explicit Registry(InsertBehavior insert_behavior = InsertBehavior::Merge);

  /// \brief name Destroys a registry.
  ~Registry();

  /// \brief Returns a list of metrics and their samples.
  ///
  /// Every time the Registry is scraped it calls each of the metrics Collect
  /// function.
  ///
  /// \return Zero or more metrics and their samples.
  std::vector<MetricFamily> Collect() const override;

 private:
  template <typename T>
  friend class detail::Builder;

  template <typename T>
  std::vector<std::unique_ptr<Family<T>>>& GetFamilies();

  template <typename T>
  bool NameExistsInOtherType(const std::string& name) const;

  template <typename T>
  Family<T>& Add(const std::string& name, const std::string& help,
                 const std::map<std::string, std::string>& labels);

  const InsertBehavior insert_behavior_;
  std::vector<std::unique_ptr<Family<Counter>>> counters_;
  std::vector<std::unique_ptr<Family<Gauge>>> gauges_;
  std::vector<std::unique_ptr<Family<Histogram>>> histograms_;
  std::vector<std::unique_ptr<Family<Summary>>> summaries_;
  mutable std::mutex mutex_;
};

}  // namespace prometheus

#endif  // IMPALA_PROMETHEUS_REGISTRY_H_
