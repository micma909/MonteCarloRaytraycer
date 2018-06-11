#ifndef Vec2_H
#define Vec2_H

#ifndef CUDA_CALLABLE_MEMBER
#define CUDA_CALLABLE_MEMBER
#endif

#include "Vec.h"

#define FOR_EACH_ELEMENT for(int i = 0; i < 2; i++)
#define FOR_EACH_ELEMENT_N(N) for(int i = 0; i < N; i++)
#define VALUE_AT(i) v_[i]

template<class T>
class Vec2 : public Vec<T,2>{
public:
	CUDA_CALLABLE_MEMBER Vec2():Vec<T,2>(){
		
	}

	CUDA_CALLABLE_MEMBER Vec2(const Vec2<T> &v):Vec<T,2>(){
		FOR_EACH_ELEMENT_N(2){
			(*this)[i] = v[i];
		}
	}

	template<class T2,int D>
	CUDA_CALLABLE_MEMBER Vec2(const Vec<T2,D> &v):Vec<T,2>(){
		FOR_EACH_ELEMENT{
			(*this)[i] = v[i];
		}
	}

	CUDA_CALLABLE_MEMBER Vec2(T x,T y):Vec<T,2>(){
		(*this)[0] = x;
		(*this)[1] = y;
	}
};

#undef FOR_EACH_ELEMENT 
#undef VALUE_AT
#endif