#ifndef Vec4_H
#define Vec4_H

#ifndef CUDA_CALLABLE_MEMBER
#define CUDA_CALLABLE_MEMBER
#endif

#include "Vec.h"

#define FOR_EACH_ELEMENT for(int i = 0; i < 4; i++)
#define FOR_EACH_ELEMENT_N(N) for(int i = 0; i < N; i++)
#define VALUE_AT(i) v_[i]
#define MIN(A,B) (B < A ? B : A)

template<class T>
class Vec4 : public Vec<T,4>{
public:
	CUDA_CALLABLE_MEMBER Vec4():Vec<T,4>(){
		
	}

	CUDA_CALLABLE_MEMBER Vec4(const Vec4 &v):Vec<T,4>(){
		FOR_EACH_ELEMENT_N(4){
			(*this)[i] = v[i];
		}
	}

	//Vec3 v3(1,2,3);
	//Vec4 v4(v3);
	//v4 = ??? // v4 = 1,2,3,0
	template<class T2,int D>
	CUDA_CALLABLE_MEMBER Vec4(const Vec<T2,D> &v):Vec<T,4>(){
		int N = MIN(4,D);
		FOR_EACH_ELEMENT_N(N){
			(*this)[i] = v[i];
		}

		for(int i = N; i < 4; i++){
			(*this)[i] = 0;
		}

	}
	
	//Vec3 v2(1,2);
	//Vec4 v4(v2,3,10);
	//v4 = ??? // v4 = 1,2,3,10
	CUDA_CALLABLE_MEMBER Vec4(const Vec2<T> &v,T z,T w):Vec<T,4>(){
		FOR_EACH_ELEMENT_N(2){
			(*this)[i] = v[i];
		}
		(*this)[2] = z;
		(*this)[3] = w;
	}

	//Vec3 v3(1,2,5);
	//Vec4 v4(v2,10);
	//v4 = ??? // v4 = 1,2,5,10
	CUDA_CALLABLE_MEMBER Vec4(const Vec3<T> &v,T w):Vec<T,4>(){
		FOR_EACH_ELEMENT_N(3){
			(*this)[i] = v[i];
		}
		(*this)[3] = w;
	}

	CUDA_CALLABLE_MEMBER Vec4(T x,T y,T z,T w):Vec<T,4>(){
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
		(*this)[3] = w;
	}


};
#undef MIN
#undef FOR_EACH_ELEMENT 
#undef VALUE_AT
#endif