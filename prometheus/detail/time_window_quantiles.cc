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

#include "prometheus/detail/time_window_quantiles.h"  // IWYU pragma: export

#include <memory>
#include <ratio>  // NOLINT
#include <iostream>

namespace prometheus {
namespace detail {

TimeWindowQuantiles::TimeWindowQuantiles(
    const std::vector<CKMSQuantiles::Quantile>& quantiles,
    const Clock::duration max_age, const int age_buckets)
    : quantiles_(quantiles),
      ckms_quantiles_(age_buckets, CKMSQuantiles(quantiles_)),
      current_bucket_(0),
      last_rotation_(Clock::now()),
      rotation_interval_(max_age / age_buckets) {}

double TimeWindowQuantiles::get(double q) const {
  CKMSQuantiles& current_bucket = rotate();
  return current_bucket.get(q);
}

void TimeWindowQuantiles::insert(double value) {
  rotate();
  for (auto& bucket : ckms_quantiles_) {
    bucket.insert(value);
  }
}

CKMSQuantiles& TimeWindowQuantiles::rotate() const {
  auto delta = Clock::now() - last_rotation_;
  // std::cout << "rotate: delta = " << std::chrono::duration_cast<std::chrono::microseconds>(delta).count()
  //           << ", rotation_interval_ = " << std::chrono::duration_cast<std::chrono::microseconds>(rotation_interval_).count() << std::endl;
  while (delta > rotation_interval_) {
    ckms_quantiles_[current_bucket_].reset();
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60] = { 0 };
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
    (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
    (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    std::cout << "当前时间: " << std::string(date)
              << ", rotate: delta = " << std::chrono::duration_cast<std::chrono::microseconds>(delta).count()
              << ", last_rotation_ = " <<  last_rotation_.time_since_epoch().count() << std::endl;

    // std::cout << "current_bucket_ = " << current_bucket_
    //           << ", last_rotation_ = " <<  last_rotation_.time_since_epoch().count() << std::endl;
    if (++current_bucket_ >= ckms_quantiles_.size()) {
      current_bucket_ = 0;
    }

    delta -= rotation_interval_;
    last_rotation_ += rotation_interval_;
  }
  // std::cout << "current_bucket_ = " << current_bucket_ << std::endl;
  return ckms_quantiles_[current_bucket_];
}

}  // namespace detail
}  // namespace prometheus
