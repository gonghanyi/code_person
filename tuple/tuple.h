#ifndef _TUPLE_H_
#define _TUPLE_H_

#include <iostream>
#include <cstring>

namespace na1{
	struct Foo{
		static void show();
	};
};

namespace impala{
class Tuple{
public:
	static Tuple* Create(int size){
		if(0 == size) return NULL;
		Tuple* result = reinterpret_cast<Tuple*>(malloc(size));
		result->Init(size);
		return result;
	}	
	
	static Tuple* const POISON;
	void Init(int size) { memset(this, 0, size); }
};

class AnyVal{
public:
	AnyVal(int* val):val_(val){}
	int getVal() const { return *val_; }
private:
	int* val_;
};

std::ostream& operator<<(std::ostream& os, const AnyVal& val);

};

#endif
