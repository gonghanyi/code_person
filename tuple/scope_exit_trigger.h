#ifndef _SCOPE_EXIT_TRIGGER_H_
#define _SCOPE_EXIT_TRIGGER_H_

//销毁客户端提供的函数的程序类
//例如：A a; auto tmp = MakeScopeExitTrigger([&a](){ a.Close(); });

namespace impala{
  
  template<typename T>
  class ScopeExitTrigger;

  template<typename T>
  ScopeExitTrigger<T> MakeScopeExitTrigger(const T& trigger);

  template<typename T>
  class ScopeExitTrigger{
    public:
      ~ScopeExitTrigger(){ trigger_(); }
      friend ScopeExitTrigger<T> MakeScopeExitTrigger<>(const T& trigger);
    
    private:
      explicit ScopeExitTrigger(const T& t):trigger_(t){} 
    private:
      T trigger_;
  };
 
  template<typename T>
  ScopeExitTrigger<T> MakeScopeExitTrigger(const T& trigger)
  {
    return ScopeExitTrigger<T>(trigger);
  }
};

#endif
