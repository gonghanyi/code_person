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

#include "prometheus/detail/utils.h"

#include <gtest/gtest.h>

#include <map>
#include <utility>

namespace prometheus {

namespace {

TEST(UtilsTest, hash_labels_1) {
  std::map<std::string, std::string> labels;
  labels.insert(std::pair<std::string, std::string>("key1", "value1"));
  labels.insert(std::pair<std::string, std::string>("key2", "vaule2"));
  auto value1 = detail::hash_labels(labels);
  auto value2 = detail::hash_labels(labels);

  EXPECT_EQ(value1, value2);
}

TEST(UtilsTest, hash_labels_2) {
  std::map<std::string, std::string> labels1{{"aa", "bb"}};
  std::map<std::string, std::string> labels2{{"a", "abb"}};
  EXPECT_NE(detail::hash_labels(labels1), detail::hash_labels(labels2));
}

TEST(UtilsTest, hash_label_3) {
  std::map<std::string, std::string> labels1{{"a", "a"}};
  std::map<std::string, std::string> labels2{{"aa", ""}};
  EXPECT_NE(detail::hash_labels(labels1), detail::hash_labels(labels2));
}

}  // namespace

}  // namespace prometheus
