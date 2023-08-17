#ifndef BASE_MEMORY_REF_COUNTED_H_
#define BASE_MEMORY_REF_COUNTED_H_ 

#include <cassert>
#include "util.h"

class RefCountedBase {
  public:
    bool HasOneRef() const { return ref_count == 1; }
  
  protected:
    RefCountedBase();
    ~RefCountedBase();

    void AddRef() const;
    bool Release() const;

  private:
    mutable int ref_count_;
};

class RefCountedThreadSafeBase {
  public:
    bool HasOneRef() const;

  protected:
    RefCountedThreadSafeBase();
    ~RefCountedThreadSafeBase();

    void AddRef() const;
    bool Release() const;

  private:
    mutable int ref_count_;
    DISALLOW_COPY_AND_ASSIGN(RefCountedThreadSafeBase);
};

template<class T>
class RefCounted : public RefCountedBase {
  public:
    RefCounted() {}

    void AddRef() const {
      RefCountedBase::AddRef();
    }

    void Release() const {
      if (RefCountedBase::Release()) {
        delete static_cast<const T*>(this);
      }
    }

  protected:
    ~RefCounted() {}

  private:
    DISALLOW_COPY_AND_ASSIGN(RefCounted);
    
};

// Forward declaration
template<class T, typename Traits> class RefCountedThreadSafe;

template<typename T>
struct DefaultRefCountedThreadSafeTraits {
  static void Destruct(const T* x) {
    RefCountedThreadSafe<T, 
        DefaultRefCountedThreadSafeTraits>::DeleteInternal(x);
  }
};

template <class T, typename Traits = DefaultRefCountedThreadSafeTraits<T>>
class RefCountedThreadSafe : public RefCountedThreadSafeBase {
public:
  RefCountedThreadSafe() {}

  void AddRef() const {
    RefCountedThreadSafeBase::AddRef();
  }

  void Release() const {
    if (RefCountedThreadSafeBase::Release()) {
      Traits::Destruct(static_cast<const T*>(this));
    }
  }

protected: 
  ~RefCountedThreadSafe() {}

private: 
  friend struct DefaultRefCountedThreadSafeTraits<T>;
  static void DeleteInternal(const T* x) { delete x; }

  DISALLOW_COPY_AND_ASSIGN(RefCountThreadSafe);
};

template<typename T>
class RefCountedData : RefCountedThreadSafe<RefCountedData<T>> {
  public:
    RefCountedData() : data() {}
    RefCountedData(const T& in_value) : data(in_value) {}

    T data;
  private:
    friend class RefCountedThreadSafe<RefCountedData<T>>;
    ~RefCountedData();
};

template<class T>
class scoped_refptr {
  public:
    typedef T element_type;

    scoped_refptr() : ptr_(NULL) {
    }

    scoped_refptr(T* p) : ptr_(p) {
      if (ptr_) {
        ptr_->AddRef();
      }
    }

    scoped_refptr(const scoped_refptr<T>& r) : ptr_(r.ptr_) {
      if (ptr_) {
        ptr_->AddRef();
      }
    }

    template <typename U>
    scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get()) {
      if (ptr_) {
        ptr_->AddRef();
      }
    }

    template <typename U>
    scoped_refptr(scoped_refptr<U>&& r) : ptr_(r.get()) {
      r.ptr_ = nullptr;
    }
  
  protected:
    T* ptr_;

  private:
    template <typename U> friend class scoped_refptr;  
};

#endif
