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

#include "prometheus/detail/ckms_quantiles.h"  // IWYU pragma: export

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <iostream>

namespace prometheus {
namespace detail {

CKMSQuantiles::Quantile::Quantile(double quantile, double error)
    : quantile(quantile),
      error(error),
      u(2.0 * error / (1.0 - quantile)),
      v(2.0 * error / quantile) {}

CKMSQuantiles::Item::Item(double value, int lower_delta, int delta)
    : value(value), g(lower_delta), delta(delta) {}

CKMSQuantiles::CKMSQuantiles(const std::vector<Quantile>& quantiles)
    : quantiles_(quantiles), count_(0), buffer_{}, buffer_count_(0) {}

void CKMSQuantiles::insert(double value) {
  buffer_[buffer_count_] = value;
  ++buffer_count_;
  if (buffer_count_ == buffer_.size()) {
    insertBatch();
    compress();
  }
}

void CKMSQuantiles::printSample() {
  std::cout << std::endl << "-----------------print sample begin------------------" << std::endl;
  auto it = sample_.begin();
  while (it != sample_.end()) {
    std::cout << "value = " << it->value << ", g = " << it->g << ", delta = " << it->delta << std::endl;
    it++;
  }
  std::cout << std::endl << "-----------------print sample end------------------" << std::endl;
}

double CKMSQuantiles::get(double q) {
  insertBatch();
  compress();

  if (sample_.empty()) {
    return std::numeric_limits<double>::quiet_NaN();
  }

  int rankMin = 0;
  const auto desired = static_cast<int>(q * count_);
  const auto bound = desired + (allowableError(desired) / 2);

  std::cout << "CKMSQuantiles::get: q = " << q << ", disired = " << desired
            << ", bound = " << bound << std::endl;

  auto it = sample_.begin();
  decltype(it) prev;
  auto cur = it++;

  while (it != sample_.end()) {
    prev = cur;
    cur = it++;

    rankMin += prev->g;

    // std::cout << "rankMin = " << rankMin << ", cur->g = " << cur->g
    //           << ", cur->delta = " << cur->delta << std::endl; 

    if (rankMin + cur->g + cur->delta > bound) {
      return prev->value;
    }
  }

  return sample_.back().value;
}

void CKMSQuantiles::reset() {
  count_ = 0;
  sample_.clear();
  buffer_count_ = 0;
}

double CKMSQuantiles::allowableError(int rank) {
  auto size = sample_.size();
  double minError = size + 1;

  for (const auto& q : quantiles_.get()) {
    double error;
    if (rank <= q.quantile * size) {
      error = q.u * (size - rank);
    } else {
      error = q.v * rank;
    }
    if (error < minError) {
      minError = error;
    }
  }

  return minError;
}

bool CKMSQuantiles::insertBatch() {
  if (buffer_count_ == 0) {
    return false;
  }
  Clock::time_point time1 = Clock::now();
  std::sort(buffer_.begin(), buffer_.begin() + buffer_count_);
  Clock::time_point time2 = Clock::now();
  auto delta = time2 - time1;
  int cnt = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
  if (cnt > 1000000) {
    std::cout << "insertBatch-sort(time) = " << cnt << std::endl;
  }

  std::size_t start = 0;
  if (sample_.empty()) {
    sample_.emplace_back(buffer_[0], 1, 0);
    ++start;
    ++count_;
  }

  std::size_t idx = 0;
  std::size_t item = idx++;

  for (std::size_t i = start; i < buffer_count_; ++i) {
    double v = buffer_[i];
    while (idx < sample_.size() && sample_[item].value < v) {
      item = idx++;
    }

    if (sample_[item].value > v) {
      --idx;
    }

    int delta;
    if (idx - 1 == 0 || idx + 1 == sample_.size()) {
      delta = 0;
    } else {
      delta = static_cast<int>(std::floor(allowableError(idx + 1))) + 1;
    }

    sample_.emplace(sample_.begin() + idx, v, 1, delta);
    count_++;
    item = idx++;
  }
  delta = Clock::now() - time2;
  cnt = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
  if (cnt > 1000000) {
    std::cout << "insertBatch-buffer_count(time) = " << cnt << std::endl;
  }

  buffer_count_ = 0;
  return true;
}

void CKMSQuantiles::compress() {
  if (sample_.size() < 2) {
    return;
  }

  std::size_t idx = 0;
  std::size_t prev;
  std::size_t next = idx++;
  std::size_t roll_cnt = 0;

  Clock::time_point time1 = Clock::now();
  while (idx < sample_.size()) {
    prev = next;
    next = idx++;

    if (sample_[prev].g + sample_[next].g + sample_[next].delta <=
        allowableError(idx - 1)) {
      sample_[next].g += sample_[prev].g;
      sample_.erase(sample_.begin() + prev);
    }
    roll_cnt++;
  }
  auto delta = Clock::now() - time1;
  int cnt = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
  if (cnt > 1000000) {
    std::cout << "internal compress(time) = " << cnt
              << ", roll cnt = " << roll_cnt
              << ", sample size = " << sample_.size() << std::endl;
  }

}

}  // namespace detail
}  // namespace prometheus
