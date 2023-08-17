#include "tuple.h"
#include <iostream>

void na1::Foo::show(){
	std::cout << "namespace1 test" << std::endl;
}

namespace impala{

std::ostream& operator<<(std::ostream& os, const AnyVal& v)
{
	os << v.getVal();
	return os;
}

};
