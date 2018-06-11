#ifndef Vec3_H
#define Vec3_H

#ifndef CUDA_CALLABLE_MEMBER
#define CUDA_CALLABLE_MEMBER
#endif

#include <stdarg.h>
#include <math.h>
#define sign(x) ((x > 0.0) - (x < 0.0))
#define FOR_EACH_ELEMENT for(int i = 0; i < 3; i++)
#define FOR_EACH_ELEMENT_N(N) for(int i = 0; i < N; i++)
#define VALUE_AT(i) v_[i]
#define MIN(A,B) (B < A ? B : A)
template<class T>
class Vec3 : public Vec<T,3>{
public:

	CUDA_CALLABLE_MEMBER Vec3():Vec<T,3>(){
		
	}

	CUDA_CALLABLE_MEMBER Vec3(T v):Vec<T,3>(){
		FOR_EACH_ELEMENT_N(3){
			(*this)[i] = v;
		}
	}

	CUDA_CALLABLE_MEMBER Vec3(const Vec3 &v):Vec<T,3>(){
		FOR_EACH_ELEMENT_N(3){
			(*this)[i] = v[i];
		}
	}

	template<class T2,int D>
	CUDA_CALLABLE_MEMBER Vec3(const Vec<T2,D> &v):Vec<T,3>(){
		int N = MIN(D,3);
		FOR_EACH_ELEMENT_N(N){
			(*this)[i] = v[i];
		}
		for(int i = N; i < 3; i++){
			(*this)[i] = 0;
		}
	}

	CUDA_CALLABLE_MEMBER Vec3(T x,T y,T z):Vec<T,3>(){
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
	}

	CUDA_CALLABLE_MEMBER Vec4<T> xyz1() const{
		return Vec4<T>((*this)[0],(*this)[1],(*this)[2],T(1)); 
	}

	CUDA_CALLABLE_MEMBER Vec4<T> xyz0() const{
		return Vec4<T>((*this)[0],(*this)[1],(*this)[2],T(0)); 
	}

	// 3dim Linear interpolation by @param t /
	CUDA_CALLABLE_MEMBER Vec3<T> lerp(const Vec<T,3> &b, float t){
	    return (*this) + (b - (*this)) * t;
	}

	// 3dim Reflection of a vector on a normal //
	CUDA_CALLABLE_MEMBER Vec3<T> reflect(const Vec<T,3> &n) const{
	    return (*this) - T(2)*(this->dot(n))*n;
	}
    
    Vec3<T> rotate(const Vec3<T> &n) const{
        if (!std::abs(1 - std::abs(n * Vec3<T>(T(0),T(1),T(0) ) )) <= T(1e-3)) {
	        Vec3f bX = n.cross( Vec3<T>(T(0),T(1),T(0)) );
	        bX.normalize();
	        Vec3f bZ = n.cross(bX);
	        bZ.normalize();
            
	        return bX * (*this)[0] + n * (*this)[1] + bZ * (*this)[2];
        } else{
            return ((*this) * sign((*this).dot(n)));
        }
	}


	CUDA_CALLABLE_MEMBER Vec<T,3> cross(const Vec<T,3> &v) const{
		return Vec3( (*this)[1]*v[2] - (*this)[2]*v[1],
                     (*this)[2]*v[0] - (*this)[0]*v[2],
                     (*this)[0]*v[1] - (*this)[1]*v[0]);
	}

};
#undef MIN
#undef FOR_EACH_ELEMENT 
#undef VALUE_AT
#undef sign
#endif