#include "tuple/tuple.h"
#include "tuple/base64.h"
#include "util/lru_cache.h"
#include "scope_exit_trigger.cc"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <float.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "substitute.h"
#include <mutex>
#include <unistd.h>
#include <memory>
#include <functional>
#include <regex>
#include "prometheus/summary.h"
#include "pattern/observer.h"
#include "mutex/mutex_lock.h"

using boost::algorithm::split;
using boost::algorithm::is_any_of;
using boost::algorithm::token_compress_on;
using std::mutex;
using std::regex;

struct Option {
  int id;
  std::string name;
  Option(int id_, std::string name_) : id(id_), name(name_) {}
};

typedef std::map<int, std::shared_ptr<Option>> MapOption;
MapOption map_op;

void TestOption() {
  std::shared_ptr<Option> op1 = std::make_shared<Option>(1, std::string("one"));
  std::shared_ptr<Option> op2 = std::make_shared<Option>(2, std::string("two"));
  map_op.insert(make_pair(1, op1));
  map_op.insert(make_pair(2, op2));
  std::shared_ptr<Option> op3 = map_op[1];
  op3->name = std::string("three");
  op3->id = 3;
  map_op.insert(make_pair(3, op3));
  for(auto& op : map_op) {
    std::cout << op.first << ", " << op.second->id << ", " << op.second->name << std::endl;
  }
}

std::string GetClock()
{
  std::ifstream timezone_file("/data/data/code_person/clock.txt");
  std::string result;
    while (timezone_file) {
      getline(timezone_file, result);
      std::cout << "tmp-result = " << result << std::endl;
      if (result.find("#") != std::string::npos) continue;
      auto p = result.find("ZONE=\"");
      if (p != std::string::npos) {
        std::cout << "p = " << p << std::endl;
        result.erase(p, p + 6);
        std::cout << "final result1 = " << result << std::endl;
        result.erase(result.rfind('"'));
        std::cout << "final result2 = " << result << std::endl;
        // auto end = result.rfind('"');
        // size_t header = strlen("ZONE=\"");
        // result = result.substr(p + header, end - p - header);
        return result;
      }
    }
    return result;
}

const static std::string skv_src = "/Users/gonghanyi/code_person/config.ini";
const static std::string skv_bak = "/Users/gonghanyi/code_person/config_bak.ini";

static std::mutex config_mutex_;
void input_skv_config(const std::string& host) {
  std::unique_lock<mutex> lock(config_mutex_);
  std::ifstream fin;
  std::string line;
  bool need_input = true;
  fin.open(skv_src.c_str(), std::ios::in);
  if (!fin.is_open()) {
    std::cout << "fail to open skv config use std::ios::in\n";
    return;
  }
  while (!fin.eof())
  {
    getline(fin, line);
    boost::trim(line);
    if (0 == line.find("skv_offline")) {
      need_input = false;
      break;
    }
  }
  fin.close();
  if (need_input) {
    std::ofstream config;
    config.open(skv_src.c_str(), std::ios::app);
    if (!config.is_open()) {
      std::cout << "fail to open skv config\n";
      return;
    }
    std::string meta_list = string("skv_offline = ") + host;
    config << meta_list << std::endl;
    config.close();
  }
}

void deal_skv_config(const string& host) {
  std::ifstream fin;
  fin.open(skv_src.c_str());
  if (!fin.is_open()) {
    std::cout << "fail to open file: " << skv_src << std::endl;
    return;
  }
  fin.seekg(-2, std::ios::end);
  int file_len = static_cast<int>(fin.tellg()) + 1;
  std::cout << "file_len = " << file_len << std::endl;
  while (1) {
    char ch;
    fin.get(ch);
    if (static_cast<int>(fin.tellg()) <= 1) {
      fin.seekg(0);
      break;
    } else if ('\n' == ch) {
      break;
    } else {
      fin.seekg(-2, std::ios::cur);
    }
  }
  string last_line;
  getline(fin, last_line);
  std::cout << "last_line[" << last_line << "], len[" << last_line.length() << "]" << std::endl;
  fin.close();
  std::vector<std::string> vec;
  boost::split(vec, last_line, is_any_of("="));
  if (vec.size() <= 1 || boost::trim_copy(vec[1]) != host) {
    std::cout << "need to replace skv_conf\n";
    truncate(skv_src.c_str(), file_len-last_line.length());
    std::ofstream fout;
    fout.open(skv_src.c_str(), std::ios::app);
    if (!fout.is_open()) {
      std::cout << "fail to open skv config, use ios::app\n";
      return;
    }
    std::string meta_list = string("skv_offline = ") + host;
    fout << meta_list << std::endl;
    fout.close();
  } else {
    std::cout << "no need to replace skv_conf\n";
  }
}

void replace_skv_config(const string& host) {
  std::ifstream config_src;
  config_src.open(skv_src.c_str());
  if (!config_src.is_open()) {
    std::cout << "fail to open file: " << skv_src << std::endl;
    return;
  }
  std::ofstream config_bak;
  config_bak.open(skv_bak.c_str(), std::ios::app);
  string line;
  while (!config_src.eof()) {
    getline(config_src, line);
    if (line.find("pegasus.clusters") != string::npos) {
      config_bak << line << "\n";
      string meta_server_list = string("skv_offline = ") + host;
      config_bak << meta_server_list << "\n";
      getline(config_src, line);
    } else {
      config_bak << line;
      if (!config_src.eof()) {
        config_bak << "\n";
      }
    }
  }
  config_src.close();
  config_bak.close();
  remove(skv_src.c_str());
  rename(skv_bak.c_str(), skv_src.c_str());
}

void deal_config_ini() {
  std::string input("skv 10.120.27.197,10.120.238.149,10.120.70.247 8170 skv_offline {3}p_3#DICT_property_$country_ID_MAPPING_VALUE");
  std::vector<std::string> tokens;
  boost::split(tokens, input, is_any_of(" "));
  if (tokens.size() != 5) {
    std::cout << "parse imput for tokens error!" << std::endl;
    return;
  }
  std::vector<std::string> ips;
  boost::split(ips, tokens[1], is_any_of(","));
  if (0 == ips.size()) {
    std::cout << "parse ip failed" << std::endl;
    return;
  }
  std::string src_name = "/Users/gonghanyi/code_person/config.ini";
  std::string dst_name = "/Users/gonghanyi/code_person/config_bak.ini";
  std::ifstream config_src;
  config_src.open(src_name.c_str());
  if (!config_src.is_open()) {
    std::cout << "open config.ini failed!" << std::endl;
    return;
  } 
  std::ofstream config_dst;
  config_dst.open(dst_name.c_str(), std::ios::app);
  std::string res;
  while (!config_src.eof()) {
    getline(config_src, res);
    if (res.find("[pegasus.clusters]") != string::npos) {
      config_dst << res << "\n";
      std::string meta_list = "";
      meta_list += tokens[3] + " = ";
      for (auto ip : ips) {
        meta_list += ip + ":" + tokens[2] + ",";
      }
      meta_list.pop_back();
      std::cout << meta_list << std::endl;
      config_dst << meta_list << "\n";
      getline(config_src, res);
    } else if (res.find("data_dir =") != string::npos) {
      std::string data_dir = "data_dir = /home/sa_cluster/" + tokens[3];
      config_dst << data_dir << "\n";
    } else {
       config_dst << res;
      if (!config_src.eof()) {
        config_dst << "\n";
      }
    }
  }  
  config_src.close();
  config_dst.close();
  //remove(src_name.c_str());
  //rename(dst_name.c_str(), src_name.c_str());
}

int64_t CalcDate(int32_t date)
{
  uint64_t m_seconds = date * 24l * 3600l * 1000l;
  int64_t seconds = (int64_t)m_seconds / 1000;
  return seconds;
}

void point_test(int num, int* output){
  output = new int(num);
}

void double_point_test(int num, int** output){
  *output = new int(num);
}

void Path_Test() {
  std::string file_name = "/Users/gonghanyi/code_person/clock.txt";
  boost::filesystem::path src_path = boost::filesystem::path(file_name);
  std::cout << "src_path = " << src_path.string() << std::endl;
  std::string src_parent = src_path.parent_path().string();
  std::cout << "src_parent = " << src_parent << std::endl;
  boost::filesystem::path final_path = src_path.parent_path() / (".patch." + src_path.filename().native());
  std::cout << "final_path = " << final_path.string() << std::endl;
}

void SubstituteTest() {
  std::string input1 = "Substitute test, $0 is int, $1 is string";
  int num = 15;
  std::string output1 = Substitute(input1, num, "second");
  std::string input2 = "Substitute test, $0 is string, $1 is char"; 
  std::string output2 = "Second test-----";
  SubstituteAndAppend(&output2, input2, "abcd", 's');
  std::cout << output1 << std::endl;
  std::cout << output2 << std::endl;
}

uint64_t cohort[2] = {0};

void set_status(int pos) {
  cohort[pos / 64] |= (1l << pos);
}

// bool is_match(std::string& str) {
//   std::string src = "-[0-9]+(.[0-9]+)?|[0-9]+(.[0-9]+)?";
//   std::regex re(src);
//   return std::regex_match(str, re) ? true : false;
// }

class A
{
public:
    void print(int n1, int n2, int n3)
    {
      std::cout << n1 << " " << n2 << " " << n3 << std::endl;
    }
};

bool Contains32(const string& src, int num) {
  bool ret = false;
  const char* ptr = src.c_str();

  if (src.length() % 4 == 0) {
    for (size_t i = 0; i < src.length(); i += 4) {
      unsigned char c1 = *(ptr + i);
      unsigned char c2 = *(ptr + i + 1);
      unsigned char c3 = *(ptr + i + 2);
      unsigned char c4 = *(ptr + i + 3);
      std::cout << "c1 = " << c1 << ", c2 = " << c2 << ", c3 = " << c3 << ", c4 = " << c4 << std::endl;
      uint32_t val = (c1 << 24) + (c2 << 16) + (c3 << 8) + c4;
      uint16_t val2 = (c1 << 24) + (c2 << 16) + (c3 << 8) + c4;
      std::cout << "val = " << val << ", num = " << num <<  ", val2 = " << val2 <<std::endl;
      if (val == num) {
          ret = true;
          break;
      }
    }
  }
  return ret;
}

void testForEach() {
  std::map<std::string, std::string> collected;
  const auto add_label = 
      [&collected](const std::pair<std::string, std::string>& label_pair) {
        collected.insert(label_pair);
      };
  std::map<std::string, std::string> constant_labels_ = {
    {"fisst", "one"},
    {"second", "two"},
    {"third", "three"}
  };
  std::for_each(constant_labels_.cbegin(), constant_labels_.cend(), add_label);
  for (auto it = collected.begin(); it != collected.end(); it++) {
    std::cout << it->first << ", " << it->second << std::endl;
  }
}

struct CommaIsSpace : std::ctype<char> {
  CommaIsSpace() : std::ctype<char>(get_table()) {}
  static mask const* get_table() {
    static mask rc[table_size];
    rc[static_cast<uint8_t>(',')] = std::ctype_base::space;
    rc[static_cast<uint8_t>(' ')] = std::ctype_base::space;
    rc[static_cast<uint8_t>('\t')] = std::ctype_base::space;
    rc[static_cast<uint8_t>('\n')] = std::ctype_base::space;
    return &rc[0];
  }
};

std::vector<double> StringToDoubleVec(const std::string& str) {
  std::vector<double> vec;
  std::istringstream ist(str);
  ist.imbue(std::locale(ist.getloc(), new CommaIsSpace()));
  std::copy(std::istream_iterator<double>(ist),
            std::istream_iterator<double>(),
            std::back_inserter(vec));
  return vec;
}

void testStringToDoubleVec() {
  std::vector<double> vec;
  const std::string str = "1,2,3.3 4\t5\n 67 8";
  vec = StringToDoubleVec(str);
  const auto printVec =
      [](const double a) {
        std::cout << a << " ";
      };
  std::for_each(vec.begin(), vec.end(), printVec);
}



void summaryTest() {
  using Clock = std::chrono::steady_clock;
  Clock::time_point first_time = Clock::now();
  static const int SAMPLES = 1000000;
  prometheus::Summary summary{prometheus::Summary::Quantiles{{0.5, 0.05}, {0.9, 0.01}, {0.99, 0.001}}};
  for (int i = 1; i <= SAMPLES; ++i) {
    summary.Observe(i);
    //usleep(10);
  }
  auto metric = summary.Collect();
  auto s = metric.summary;

  auto delta = Clock::now() - first_time;
  std::cout << "持续时间(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() << std::endl;

  std::cout << "quantile.at(0).value = " << s.quantile.at(0).value << std::endl;
  std::cout << "quantile.at(1).value = " << s.quantile.at(1).value << std::endl;
  std::cout << "quantile.at(2).value = " << s.quantile.at(2).value << std::endl;
}

std::string strip(const std::string& str) {
  std::string ret = str;
  ret.erase(0, ret.find_first_not_of("\t\n\v\f\r ")); // left trim
  ret.erase(ret.find_last_not_of("\t\n\v\f\r ") + 1); // right trim
  return ret;
}

bool hex2bytes(const std::string& hex_str, std::vector<unsigned char>* out) {
  std::string data = strip(hex_str);
  if (data.length() % 2 != 0) return false;
  out->clear();
  out->reserve(data.length() / 2);

  for (std::size_t i = 0; i < data.size(); i += 2) {
    const char tmp[] = {data[i], data[i+1]};
    char* end_ptr = nullptr;
    unsigned long aa = std::strtoul(tmp, &end_ptr, 16);
    std::cout << aa << " ";
    out->push_back(aa);
    if (*end_ptr != 0) {
      std::cout << "*end_ptr = " << *end_ptr << std::endl;
      return false;
    }
  }
  return true;
}

void testObserver() {
  std::shared_ptr<Subject> pSubject = std::make_shared<ConcreteSubject>(); // 创建被观察者

  // 创建观察者
  std::shared_ptr<Observer> pObserver1_1 = std::make_shared<Observer1>(pSubject, "1_1");
  std::shared_ptr<Observer> pObserver1_2 = std::make_shared<Observer1>(pSubject, "1_2");
  std::shared_ptr<Observer> pObserver1_3 = std::make_shared<Observer1>(pSubject, "1_3");

  std::shared_ptr<Observer> pObserver2_1 = std::make_shared<Observer1>(pSubject, "2_1");
  std::shared_ptr<Observer> pObserver2_2 = std::make_shared<Observer1>(pSubject, "2_2");
  std::shared_ptr<Observer> pObserver2_3 = std::make_shared<Observer1>(pSubject, "2_3");

  // 注册观察者
  pSubject->Attach(pObserver1_1);
  pSubject->Attach(pObserver1_2);
  pSubject->Attach(pObserver1_3);
  pSubject->Attach(pObserver2_1);
  pSubject->Attach(pObserver2_2);
  pSubject->Attach(pObserver2_3);

  pSubject->SetState(1);
  pSubject->Notify();
  
  std::cout << std::string(50, '-') << std::endl;

  //注销观察者
  pSubject->Detach(pObserver1_1);
  pSubject->Detach(pObserver1_2);

  pSubject->SetState(2);
  pSubject->Notify();
}

Mutex mu;
int mu_num = 0;
void* count(void* args) {
  MutexLock lock(&mu);
  mu_num++;
}

void testMutex() {
  pthread_t thread[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    int ret = pthread_create(&thread[i], NULL, count, NULL);
    if (ret) {
      std::cout << "thread create failed, ret = " << ret << std::endl;
    }
  }
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(thread[i], NULL);
  }
  std::cout << "mu_num = " << mu_num << std::endl;
}

void testWeakPtr1() {
  std::shared_ptr<int> sp_a(new int(10));
  std::weak_ptr<int> wp_a = sp_a;
  std::weak_ptr<int> wp_b;
  {
    std::cout << "wk_a.expired() = " << wp_a.expired() << std::endl;
    std::shared_ptr<int> sp_ba = wp_a.lock();
    if (sp_ba) {
      std::cout << "shared_ptr b useCount = " << sp_ba.use_count() << std::endl;
    } else {
      std::cout << "shared_ptr b invalid" << std::endl;
    }
    std::shared_ptr<int> sp_b(new int(10));
    wp_b = sp_b;
  } // 作用域结束，sp_b 释放
  std::cout << "weak_ptr b expired() = " << wp_b.expired() 
            << ", use_count = " << wp_b.use_count() << std::endl;
  std::cout << "weak_ptr lock() = " << wp_b.lock() << std::endl;

  std::shared_ptr<int> sp_f = wp_b.lock();
  if (sp_f) {
    std::cout << "shared_ptr f useCount = " << sp_f.use_count() << std::endl;
  } else {
      std::cout << "shared_ptr f invalid" << std::endl;
  }
}

class A1;
class B1;

class A1 {
public:
  A1() { std::cout << "A1 has been construct" << std::endl; }
  ~A1() {std::cout << "A1 has been destroyed" << std::endl; }
  std::shared_ptr<B1> ptrB;
};

class B1 {
public:
  B1() { std::cout << "B1 has been construct" << std::endl; }
  ~B1() {std::cout << "B1 has been destroyed" << std::endl; }
  std::weak_ptr<A1> ptrA;
};

void testWeakPtr2() {
  std::weak_ptr<A1> wpA;
  std::weak_ptr<B1> wpB;
  {
    std::shared_ptr<A1> ptrA(new A1());
    std::shared_ptr<B1> ptrB(new B1());
    ptrA->ptrB = ptrB;
    ptrB->ptrA = ptrA;
    wpA = ptrA;
    wpB = ptrB;
    std::cout << "shared ptrA use_count = " << ptrA.use_count() << std::endl;
    std::cout << "shared ptrB use_count = " << ptrB.use_count() << std::endl;
  }
  std::cout << "wpA expired = " << wpA.expired() << std::endl;
  std::cout << "wpB expired = " << wpB.expired() << std::endl;
}

typedef std::function<int(int)> Functional;

int TestFunc(int a) { return a; }

//lambda expression
auto lambda = [](int a)->int{ return a; };

//functor 仿函数
class Functor {
public:
  int operator()(int a) {
    return a;
  }
};

//类的成员函数和类静态成员函数
class CTest {
public:
  int Func(int a) {
    return a;
  }
  static int SFunc(int a) {
    return a;
  }
};


void testStdFunction() {
  //封装普通函数
  Functional obj = TestFunc;
  std::cout << "normal function: " << obj(1) << std::endl;
  //封装 lambda 表达式
  obj = lambda;
  std::cout << "lambda function: " << obj(2) << std::endl;
  //封装仿函数
  Functor functorObj;
  obj = functorObj;
  std::cout << "functor function: " << obj(3) << std::endl;
  //封装类成员函数和 static 成员函数
  CTest t;
  obj = std::bind(&CTest::Func, &t, std::placeholders::_1);
  std::cout << "member function: " << obj(4) << std::endl;
  obj = CTest::SFunc;
  std::cout << "static member function: " << obj(5) << std::endl;
}

int main(int argc, char* argv[])
{
  //testObserver();
  
  //testMutex();
  
  //testWeakPtr1();
  
  //testWeakPtr2();

  testStdFunction();

  // string hex_str = "7e6276111924b3128e1676e4d599cc0adb65cd85d4d567166de38e4591b8698f";
  // std::vector<unsigned char> assigned_hash;
  // hex2bytes(hex_str, &assigned_hash);
  //summaryTest();
  //TestOption();

  //testForEach();
  //testStringToDoubleVec();

  // if (Contains32("1111", 16843009)) {
  //   std::cout << "match!!!" << std::endl;
  // } else {
  //   std::cout << "not match!!!" << std::endl;
  // }

  // LRUCache<std::string, std::string> version_cache(10);
  // version_cache.put("one", "first");
  // version_cache.put("two", "second");
  // version_cache.put("three", "third");
  // std::cout << version_cache.get("one") << std::endl;

  // std::string clock = GetClock();
  // std::cout << "clock = " << clock << std::endl;

  // std::string sql_stmt = boost::to_lower_copy(
  //     boost::trim_copy(std::string("select * from query_metrics limit 10")));
  // std::cout << "sql_stmt = " << sql_stmt << std::endl;
  // if (sql_stmt == "select 1"
  //       || boost::contains(sql_stmt, "from query_metrics")) {
  //   std::cout << "-------sql_stmt equal----------\n";
  // }
  
  // A a;
  // auto f1 = std::bind(&A::print, &a, std::placeholders::_2, 22, std::placeholders::_1);
  // f1(44, 55);
  // std::string text = "123.456";
  // // if (is_match(text)) {
  // // int num_text = atol(text.c_str());
  // // std::cout << "num_text = " << num_text << std::endl; }

  // boost::posix_time::ptime tt(boost::gregorian::date(1970,1,1));
  // const boost::gregorian::date& pdate = tt.date();
  // const boost::posix_time::time_duration& ptime = tt.time_of_day();
  // const int32_t date = *reinterpret_cast<const int32_t*>(&pdate);
  // boost::gregorian::date src_date(1970,1,1);
  // const int32_t date2 = *reinterpret_cast<const int32_t*>(&src_date);
  // const int64_t time = *reinterpret_cast<const int64_t*>(&ptime);
  // std::cout << "date = " << date << ", date2 = " << date2 << ", time = " << time << std::endl;

  // //deal_config_ini();
  // std::string host = "10.120.27.197:8170,10.120.22.232:8170";
  // //replace_skv_config2(host);
  // deal_skv_config(host);
  // std::fstream fout;
  // fout.open("aaa.ini", std::ios::app);
  // if (!fout.is_open()) {
  //   std::cout << "aaaa" << std::endl;
  // }
  // fout << "12345";
  // fout.close();
  // fout.open("ccc.ini", std::ios::in|std::ios::ate|std::ios::out);
  // fout << "12345";
  // fout.close();

  // std::map<int, int> map_tt = {
  //   {0, 1},
  //   {1, 2},
  //   {2, 3}};
  // for (int i = 0; i < map_tt.size(); ++i) {
  //   std::map<int, int>::iterator it = map_tt.find(i);
  //   if (it != map_tt.end()) {
  //     std::cout << "--------------" << it->second << std::endl;
  //   }
  // }
  // SubstituteTest();
  
  // std::cout << "+++++++++++++++\n";
  // set_status(100);
  // for (int i = 0; i < 2; ++i) {
  //   std::cout << cohort[i] << " ";
  // }
  // std::cout << std::endl;

  // int64_t bigint1 = 351251297985156000;
  // int64_t bigint_res1 = bigint1/1000;
  // std::cout << "bigint1 = " << bigint1 << ", bigint_res1 = " << bigint_res1 << std::endl;

  // int* point1 = nullptr;
  // int* point2 = nullptr;
  // point_test(11, point1);
  // double_point_test(22, &point2);
  // if(nullptr != point1) std::cout << "point1 addr = " << (void*)point1 << ", num = " << *point1 << std::endl;
  // if(nullptr != point2) std::cout << "point2 addr = " << (void*)point2 << ", num = " << *point2 << std::endl;

  // na1::Foo::show();
	// std::cout << "-----------------------" << std::endl;
	// int* num = new int (10);
	// impala::AnyVal val(num);
	// std::cout << val << std::endl;
	// (*num)++;
	// std::cout << val << std::endl;
  // delete num;
  // std::vector<int> vec{11,22,33};
  // impala::ScopeExitTest sc(5);  
  // sc.Insert(vec);
  // sc.Show();
  // auto clearup = impala::MakeScopeExitTrigger([&sc](){ sc.Release();});
  // const char input_buf[] = "DwABDAAAAAEIAAEAAAAACgACAAAAAAAAAAEIAAMAAAAECAAEAAAAAAAPAAIMAAAAAAA=";
  // //const char input_buf[] = "DwABDAAAAAIIAAEAAAAACgACAAAAAAAAAAEIAAMAAAACCAAEAAAAAAAIAAEAAAABCgACAAAAAAAAAAIIAAMAAAABCAAEAAAAAQAPAAIMAAAAAAA=";
  // int input_len = strlen(input_buf);
  // impala::Base64 base64;
  // size_t encoded_len = base64.EncodedLength(input_len);
  // std::string b64_decoded_str;
  // b64_decoded_str.resize(encoded_len);
  // bool succ = base64.Decode(
  //     input_buf,
  //     input_len,
  //     (char *) (&b64_decoded_str[0]),
  //     b64_decoded_str.size());
  // std::cout << "encoded_len = " << encoded_len << std::endl;

  // for(int i = 0; i < encoded_len; i++)
  // {
  //   std::cout << static_cast<int>(b64_decoded_str[i]) << " ";
  //   if(0 == (i+1) % 12) std::cout << std::endl;
  // }

  // int32_t init_day = 18345;
  // int64_t last_timestamp = CalcDate(18345);
  // std::cout << "last_timestamp = " << last_timestamp << std::endl;
  // int64_t first_timestamp = last_timestamp - 3*3600;
  // int64_t second_timestamp = last_timestamp - 5*3600;
  // std::vector<int64_t> vec_currtime = {last_timestamp-24*3600, first_timestamp, second_timestamp};
  // //std::vector<int64_t> vec_currtime = {0};
  // double total_weight = 0;
  // std::vector<double> weights;
  // for(int i = 0; i < vec_currtime.size(); i++){
  //   double minutes = (last_timestamp - vec_currtime[i]) / 60.0;
  //   double decay_factor = 2.30258509 * 60 / 86400;
  //   double weight = exp(-decay_factor*minutes);
  //   double factor = -decay_factor * minutes;
  //   std::cout << "minutes = " << minutes << ", decay_factor = " << decay_factor << ", factor = " << factor << ", weight = " << weight << std::endl;
  //   if(weight < DBL_EPSILON) weight += DBL_EPSILON;
  //   std::cout << "after weight = " << weight << std::endl;
  //   weights.push_back(weight);
  //   total_weight += weight;
  // }
  // if(total_weight <= 0) std::cout << "get negative total_weight" << std::endl;
  // for(int i = 0; i < weights.size(); i++){
  //   double final_weight = weights[i] / total_weight;
  //   std::cout << "final_weight = " << final_weight << std::endl;
  //   std::string exePath = boost::filesystem::initial_path<boost::filesystem::path>().string();
  //   std::cout << "-----" << exePath << "-----\n";
  // }

  // // std::string finalize_fn_symbol = "_ZN6impala18AggregateFunctions23FunnelInfoByDayFinalizeEPN10impala_udf15FunctionContextERNS1_9StringValE";
  // // finalize_fn_symbol = std::regex_replace(finalize_fn_symbol, std::regex("_ZN6impala18AggregateFunctions\\d+"), string(""));
  // // std::cout << "finalize_fn_symbol_1 = " << finalize_fn_symbol << std::endl;
  // // std::cout << "finalize_fn_symbol_2 = " << finalize_fn_symbol << std::endl;
  
  // char ch_input[][4] = {
  //   {"1ab"}, {"2cd"},{"3ef"},{"4gh"},{"5ij"},{"6kl"},{"7no"},{"8pq"},{"9rs"},
  // };
  // int ch_size = sizeof(ch_input) / sizeof(ch_input[0]);
  // char** ppch = new char* [ch_size];
  // for(int i = 0; i < ch_size; i++){
  //   ppch[i] = ch_input[i];
  // }
  // char** ppch_temp = ppch;
  // std::cout << "pch addr = " << (void*)&ch_input[0][0] << ", ppch = " << ppch_temp << ", ch = " << **ppch_temp << std::endl;
  // ppch_temp += 1;
  // std::cout << "pch addr2 = " << (void*)&ch_input[1][0] << ", ppch2 = " << ppch_temp << ", ch2 = " << **ppch_temp << std::endl;
  // delete [] ppch;

  // std::string str_input1 = "default-pool";
  // std::string str_input2 = "root.default,root.good,root.user,";
  // std::vector<std::string> vec1;
  // std::vector<std::string> vec2;
  // split(vec1, str_input1, is_any_of(","), token_compress_on);
  // split(vec2, str_input2, is_any_of(","), token_compress_on);
  // std::cout << "=============================================" << std::endl;
  // for(auto& str : vec1) {
  //   std::cout << str << " ";
  // }
  // std::cout << std::endl;
  //  for(auto& str : vec2) {
  //    if (str.empty()) continue;
  //    std::cout << str << "-";
  // }
  // std::cout << std::endl;

	return 0;
}
