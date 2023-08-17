#include "scope_exit_trigger.h"
#include <iostream>
#include <vector>
#include <cstring>

using std::string;

namespace impala{

  class ScopeExitTest{
    public: 
      ScopeExitTest(int len):len_(len){
        buf_ = new int [len_];
        memset(buf_, 0, sizeof(int)*len_);
      }
      
      void Insert(std::vector<int>& vec){
        if(nullptr == buf_ || len_ <= 0) return;
        int size = vec.size();
        for(int i = 0; i < len_ && i < size; i++){
          *(buf_+i) = vec[i];
        } 
      }

      void Show(){
        for(int i = 0; i < len_; i++){
          std::cout << *(buf_+i) << " ";
        }
        std::cout << std::endl;
      }

      void Release(){
        if(nullptr != buf_)
        {
          std::cout << "ScopeExitTest::Release" << std::endl;
          delete [] buf_;
        }
      }

    private:
      int* buf_;
      int len_;
  };

};
