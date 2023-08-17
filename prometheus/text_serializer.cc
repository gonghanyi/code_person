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

#include <cmath>
#include <limits>
#include <locale>
#include <ostream>
#include <string>

#include "prometheus/client_metric.h"
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"

namespace prometheus {

namespace {

// Write a double as a string, with proper formatting for infinity and NaN
void WriteValue(double value, std::ostream* out) {
  if (std::isnan(value)) {
    *out << "Nan";
  } else if (std::isinf(value)) {
    *out << (value < 0 ? "-Inf" : "+Inf");
  } else {
    *out << value;
  }
}

void WriteValue(const std::string& value, std::ostream* out) {
  for (auto c : value) {
    switch (c) {
      case '\n':
        *out << '\\' << 'n';
        break;

      case '\\':
        *out << '\\' << c;
        break;

      case '"':
        *out << '\\' << c;
        break;

      default:
        *out << c;
        break;
    }
  }
}

// Write a line header: metric name and labels
template <typename T = std::string>
void WriteHead(const MetricFamily& family,
               const ClientMetric& metric,
               std::ostream* out,
               const std::string& suffix = "",
               const std::string& extraLabelName = "",
               const T& extraLabelValue = T()) {
  *out << family.name << suffix;
  if (!metric.label.empty() || !extraLabelName.empty()) {
    *out << "{";
    const char* prefix = "";

    for (auto& lp : metric.label) {
      *out << prefix << lp.name << "=\"";
      WriteValue(lp.value, out);
      *out << "\"";
      prefix = ",";
    }
    if (!extraLabelName.empty()) {
      *out << prefix << extraLabelName << "=\"";
      WriteValue(extraLabelValue, out);
      *out << "\"";
    }
    *out << "}";
  }
  *out << " ";
}

// Write a line trailer: timestamp
void WriteTail(const ClientMetric& metric, std::ostream* out) {
  if (metric.timestamp_ms != 0) {
    *out << " " << metric.timestamp_ms;
  }
  *out << "\n";
}

void SerializeCounter(const MetricFamily& family,
                      const ClientMetric& metric, std::ostream* out) {
  WriteHead(family, metric, out);
  WriteValue(metric.counter.value, out);
  WriteTail(metric, out);
}

void SerializeGauge(const MetricFamily& family,
                    const ClientMetric& metric, std::ostream* out) {
  WriteHead(family, metric, out);
  WriteValue(metric.gauge.value, out);
  WriteTail(metric, out);
}

void SerializeSummary(const MetricFamily& family,
                      const ClientMetric& metric, std::ostream* out) {
  auto& sum = metric.summary;
  WriteHead(family, metric, out, "_count");
  *out << sum.sample_count;
  WriteTail(metric, out);

  WriteHead(family, metric, out, "_sum");
  WriteValue(sum.sample_sum, out);
  WriteTail(metric, out);

  for (auto& q : sum.quantile) {
    WriteHead(family, metric, out, "", "quantile", q.quantile);
    WriteValue(q.value, out);
    WriteTail(metric, out);
  }
}

void SerializeUntyped(const MetricFamily& family,
                      const ClientMetric& metric, std::ostream* out) {
  WriteHead(family, metric, out);
  WriteValue(metric.untyped.value, out);
  WriteTail(metric, out);
}

void SerializeHistogram(const MetricFamily& family,
                        const ClientMetric& metric, std::ostream* out) {
  auto& hist = metric.histogram;
  WriteHead(family, metric, out, "_count");
  *out << hist.sample_count;
  WriteTail(metric, out);

  WriteHead(family, metric, out, "_sum");
  WriteValue(hist.sample_sum, out);
  WriteTail(metric, out);

  double last = -std::numeric_limits<double>::infinity();
  for (auto& b : hist.bucket) {
    WriteHead(family, metric, out, "_bucket", "le", b.upper_bound);
    last = b.upper_bound;
    *out << b.cumulative_count;
    WriteTail(metric, out);
  }

  if (last != std::numeric_limits<double>::infinity()) {
    WriteHead(family, metric, out, "_bucket", "le", "+Inf");
    *out << hist.sample_count;
    WriteTail(metric, out);
  }
}

void SerializeFamily(const MetricFamily& family, std::ostream* out) {
  if (!family.help.empty()) {
    *out << "# HELP " << family.name << " " << family.help << "\n";
  }
  switch (family.type) {
    case MetricType::Counter:
      *out << "# TYPE " << family.name << " counter\n";
      for (auto& metric : family.metric) {
        SerializeCounter(family, metric, out);
      }
      break;
    case MetricType::Gauge:
      *out << "# TYPE " << family.name << " gauge\n";
      for (auto& metric : family.metric) {
        SerializeGauge(family, metric, out);
      }
      break;
    case MetricType::Summary:
      *out << "# TYPE " << family.name << " summary\n";
      for (auto& metric : family.metric) {
        SerializeSummary(family, metric, out);
      }
      break;
    case MetricType::Untyped:
      *out << "# TYPE " << family.name << " untyped\n";
      for (auto& metric : family.metric) {
        SerializeUntyped(family, metric, out);
      }
      break;
    case MetricType::Histogram:
      *out << "# TYPE " << family.name << " histogram\n";
      for (auto& metric : family.metric) {
        SerializeHistogram(family, metric, out);
      }
      break;
  }
}
}  // namespace

void TextSerializer::Serialize(const std::vector<MetricFamily>& metrics, std::ostream* out) const {
  auto saved_locale = out->getloc();
  auto saved_precision = out->precision();

  out->imbue(std::locale::classic());
  out->precision(std::numeric_limits<double>::max_digits10 - 1);

  for (auto& family : metrics) {
    SerializeFamily(family, out);
  }

  out->imbue(saved_locale);
  out->precision(saved_precision);
}
}  // namespace prometheus
