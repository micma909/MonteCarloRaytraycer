#ifndef Vec_H
#define Vec_H

#ifndef CUDA_CALLABLE_MEMBER
#define CUDA_CALLABLE_MEMBER
#endif

#include <stdarg.h>
#include <cmath>
#include <sstream>

/* return 1 if x == 0, else return 0 */
#define delta(x) ((float)(std::abs(x) <= 1e-3f))
/* return -1 or 1 */
#define sign(x) ((x > 0.0) - (x < 0.0))

#define FOR_EACH_ELEMENT for(int i = 0; i < D; i++)
#define FOR_EACH_ELEMENT_N(N) for(int i = 0; i < N; i++)
#define VALUE_AT(i) v_[i]

template<class T,int D> class Vec;
template<class T> class Vec2;
template<class T> class Vec3;
template<class T> class Vec4;
typedef Vec4<float> Vec4f;
typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

typedef Vec4<double> Vec4d;
typedef Vec3<double> Vec3d;
typedef Vec2<double> Vec2d;


template<class T,int D>
class Vec{
public:
    
    explicit CUDA_CALLABLE_MEMBER Vec(){

	}
    
	explicit CUDA_CALLABLE_MEMBER Vec(T val){
		FOR_EACH_ELEMENT{
			v_[i] = val;
		}
	}
	
    template<class T2,int D2>
    CUDA_CALLABLE_MEMBER Vec(const Vec<T2,D2> &val){
        FOR_EACH_ELEMENT{
            if(i < D2){
                v_[i] = val[i];
            }else{
                break;
            }
		}
	}

	CUDA_CALLABLE_MEMBER Vec<T,D> operator+(const Vec<T,D> &v) const{
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v.v_[i]+v_[i];
		}
		return nv;
	}
    
	CUDA_CALLABLE_MEMBER void operator+=(const Vec<T,D> &v){
		FOR_EACH_ELEMENT{
			v_[i] += v.v_[i];
		}
	}


	CUDA_CALLABLE_MEMBER Vec<T,D> comp_mult(const Vec<T,D> &v){
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v_[i]*v.v_[i];
		}
		return nv;
	}
	CUDA_CALLABLE_MEMBER void operator*=(float scalar){
		FOR_EACH_ELEMENT{
			v_[i] = v_[i]*scalar;
		}
	}
    
	friend CUDA_CALLABLE_MEMBER Vec<T,D> operator*(float scalar,const Vec<T,D> &v){
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v.v_[i]*scalar;
		}
		return nv;
	}
    
	friend CUDA_CALLABLE_MEMBER Vec<T,D> operator*(const Vec<T,D> &v,float scalar){
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v.v_[i]*scalar;
		}
		return nv;
	}
    
    friend CUDA_CALLABLE_MEMBER Vec<T,D> operator/(float scalar,const Vec<T,D> &v){
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v.v_[i]/scalar;
		}
		return nv;
	}
    
    friend CUDA_CALLABLE_MEMBER Vec<T,D> operator/(const Vec<T,D> &v,float scalar){
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v.v_[i]/scalar;
		}
		return nv;
	}
    
	friend CUDA_CALLABLE_MEMBER Vec<T,D> operator+(float scalar,const Vec<T,D> &v){
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v.v_[i]+scalar;
		}
		return nv;
	}
    
    
	CUDA_CALLABLE_MEMBER Vec<T,D> operator-(const Vec<T,D> &v) const{
		Vec<T,D> nv;
		FOR_EACH_ELEMENT{
			nv.v_[i] = v_[i]-v.v_[i];
		}
		return nv;
	}
    
	CUDA_CALLABLE_MEMBER void normalize(){
		T sum = 1.0/std::sqrt(dot(*this));
		FOR_EACH_ELEMENT{
			v_[i] *= sum;
		}
	}

	CUDA_CALLABLE_MEMBER T length() const{
		return std::sqrt(dot(*this));
	}
    
	inline CUDA_CALLABLE_MEMBER T dot(const Vec<T,D> &v) const{
		T sum = 0;
		FOR_EACH_ELEMENT{
			sum += v_[i]*v.v_[i];
		}
		return sum;
	}

	CUDA_CALLABLE_MEMBER T operator*(const Vec<T,D> &v) const{
		return (*this).dot(v);
	}
    
    CUDA_CALLABLE_MEMBER bool operator==(const Vec<T, D> &v) const{
        FOR_EACH_ELEMENT{
			if(v[i] != v_[i]) return false;
		}
        return true;
	}
    
    CUDA_CALLABLE_MEMBER bool operator<(const Vec<T, D> &v) const{
        FOR_EACH_ELEMENT{
			if(v[i] >= v_[i]) return false;
		}
        return true;
	}

	CUDA_CALLABLE_MEMBER T operator[](int i) const{
		return v_[i];
	}
	CUDA_CALLABLE_MEMBER T& operator[](int i){
		return v_[i];
	}
    
    CUDA_CALLABLE_MEMBER T* begin(){
		return v_;
	}
    
	CUDA_CALLABLE_MEMBER void print() const{
		printf("(");
		FOR_EACH_ELEMENT{
			printf("%f, ", VALUE_AT(i));
		}
		printf(")");
	}
    
    friend std::ostream& operator<<(std::ostream &s,const Vec<T, D> &v_){
        std::stringstream ss;
        ss << "(";
        FOR_EACH_ELEMENT{
			ss <<  VALUE_AT(i);
            if (i+1 != D) {
                ss << ",";
            }
		}
        ss << ")";
        s << ss.str();
        return s;
    }
    
protected:
	T v_[D];
};

/*
namespace Vector{
    //2D, 3D and 4D constructors
	template<class T>
	static CUDA_CALLABLE_MEMBER Vec<T,2> vec(T x,T y){
		Vec<T,2> v;
		v[0] = x;
		v[1] = y;
		return v;
	}
    
	template<class T>
	static CUDA_CALLABLE_MEMBER Vec<T,3> vec(T x,T y,T z){
		Vec<T,3> v;
		v[0] = x;
		v[1] = y;
		v[2] = z;
		return v;
	}
    
	template<class T>
	static CUDA_CALLABLE_MEMBER Vec<T,4> vec(T x,T y,T z,T w){
		Vec<T,4> v;
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
		return v;
	}

	template<class T>
	Vec<T,4> xyz1(const Vec<T,3> &v){
		return Vector::vec(v[0],v[1],v[2],T(1)); 
	}
	template<class T>
	Vec<T,4> xyz0(const Vec<T,3> &v){
		return Vector::vec(v[0],v[1],v[2],T(0)); 
	}
	// 3dim Linear interpolation by @param t /
	template<class T>
	Vec<T,3> lerp(const Vec<T,3> &a,const Vec<T,3> &b, float t){
	    return a + (b - a) * t;
	}
	// 3dim Reflection of a vector on a normal //
	template<class T>
	Vec<T,3> reflect(const Vec<T,3> &i, const Vec<T,3> &n){
	    return i - n * (T(2) * (i * n));
	}
	template<class T>
	Vec<T,3> cross(const Vec<T,3> &v_, const Vec<T,3> &v){
		Vec<T,3> ab;
		
		ab[0] = v_[1]*v[2] - v_[2]*v[1]; 
		ab[1] = v_[2]*v[0] - v_[0]*v[2]; 
		ab[2] = v_[0]*v[1] - v_[1]*v[0];
	
		return ab;
	}
	
};*/

#undef FOR_EACH_ELEMENT 
#undef VALUE_AT

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#endif