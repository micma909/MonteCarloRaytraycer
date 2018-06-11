#ifndef Mat_H
#define Mat_H
#include <stdio.h>
#include <math.h>
#include "Vec.h"

#ifndef CUDA_CALLABLE_MEMBER
#define CUDA_CALLABLE_MEMBER
#endif

#define MAT Mat<T,N,M>
#define FOR_EACH_COL_PARAM(p) for(int p = 0; p < M; p++)
#define FOR_EACH_ROW_PARAM(p) for(int p = 0; p < N; p++)
#define FOR_EACH_COL FOR_EACH_COL_PARAM(i)
#define FOR_EACH_ROW FOR_EACH_COL_PARAM(j)
#define FOR_EACH_ELEMENT FOR_EACH_ROW FOR_EACH_COL
#define VALUE_AT_ARR(i,j,v) v[i+j*M]
#define VALUE_AT(i,j) VALUE_AT_ARR(i,j,v_)

template<class T = float,int M = 3, int N = 3>
class Mat{
public:
    CUDA_CALLABLE_MEMBER Mat() {
        
	}
	CUDA_CALLABLE_MEMBER Mat(const T &element) {
		FOR_EACH_ELEMENT{
            VALUE_AT_ARR(i,j,v_) = element;
        }
	}
	template<class T2>
	CUDA_CALLABLE_MEMBER Mat(const Mat<T2,N,M> &m) {
        FOR_EACH_ELEMENT{
        	(*this)(i,j) = m(i,j);
        }
	}

	CUDA_CALLABLE_MEMBER void scalarFill(T val) {
        FOR_EACH_ELEMENT{
            VALUE_AT_ARR(i,j,v_) = val;
        }
	}

	CUDA_CALLABLE_MEMBER void arrayFill(const T *array) {
        FOR_EACH_ELEMENT{
            VALUE_AT_ARR(i,j,v_) = VALUE_AT_ARR(i,j,array);
        }
	}

	CUDA_CALLABLE_MEMBER MAT inverse() const{
		//MAT res(T(0));
		return identity();
	}

	CUDA_CALLABLE_MEMBER MAT operator*(const MAT &m) const{
		MAT res(T(0));
		FOR_EACH_ROW_PARAM(j){
			FOR_EACH_COL_PARAM(i){
				FOR_EACH_COL_PARAM(k){
					res(i,j) += (*this)(k,j)*m(i,k);
				}
			}
		}
		return res;
	}
    CUDA_CALLABLE_MEMBER MAT operator*(T scalar) const{
        MAT res;
        FOR_EACH_ELEMENT{
                VALUE_AT_ARR(i,j,res.v_) = VALUE_AT_ARR(i,j,v_)*scalar;
        }
        return res;
	}
    CUDA_CALLABLE_MEMBER Vec<T,N> operator*(const Vec<T,N> &vec) const{
		Vec<T,N> res(0);
		FOR_EACH_ROW{
			FOR_EACH_COL{
				res[j] += VALUE_AT(i,j)*vec[i];
			}
		}
		return res;
	}
    CUDA_CALLABLE_MEMBER void operator*=(T scalar){
		FOR_EACH_ROW{
			FOR_EACH_COL{
				VALUE_AT_ARR(i, j,v_) *= scalar;
			}
		}
	}
    CUDA_CALLABLE_MEMBER MAT operator+(const MAT &mat) const{
		MAT res;
		FOR_EACH_ROW{
			FOR_EACH_COL{
				VALUE_AT_ARR(i, j, res.v_) = (*this)(i,j)+mat(i,j);
			}
		}
		return res;
	}
    CUDA_CALLABLE_MEMBER void operator+=(const MAT &mat){
		FOR_EACH_ROW{
			FOR_EACH_COL{
				VALUE_AT(i, j) += mat(i,j);
			}
		}
	}
    CUDA_CALLABLE_MEMBER MAT operator-(const MAT &mat) const{
		MAT res;
		FOR_EACH_ROW{
			FOR_EACH_COL{
				VALUE_AT_ARR(i, j, res.v_) = (*this)(i,j)-mat(i,j);
			}
		}
		return res;
	}
    CUDA_CALLABLE_MEMBER void operator-=(const MAT &mat){
		FOR_EACH_ROW{
			FOR_EACH_COL{
				VALUE_AT(i, j) -= mat(i,j);
			}
		}
	}
	CUDA_CALLABLE_MEMBER void print() const{
		FOR_EACH_ROW{
			FOR_EACH_COL{
				printf("%f ",VALUE_AT(i,j));
			}
			printf("\n");
		}
	}
    //Getters
	inline CUDA_CALLABLE_MEMBER T operator()(int i,int j) const{
		return VALUE_AT(i,j);
	}
	inline CUDA_CALLABLE_MEMBER T& operator()(int i,int j){
		return VALUE_AT(i,j);
	}

	inline  CUDA_CALLABLE_MEMBER T operator[](int i) const{
		return v_[i];
	}
	inline  CUDA_CALLABLE_MEMBER T& operator[](int i){
		return v_[i];
	}
    
    //Factory
    CUDA_CALLABLE_MEMBER static MAT identity(){
        MAT m;
        FOR_EACH_ROW_PARAM(i){
            FOR_EACH_COL_PARAM(j){
                m(i,j) = (i != j) ? 0 : 1;
            }
        }
        return m;
    }
    
protected:
	T v_[M*N];
};

typedef Mat<float,4,4> Mat4x4f;
typedef Mat<float,3,3> Mat3x3f;
typedef Mat<double,4,4> Mat4x4d;
typedef Mat<double,3,3> Mat3x3d;

#undef FOR_EACH_COL
#undef FOR_EACH_ROW
#undef FOR_EACH_ELEMENT
#undef VALUE_AT


namespace Matrix{

	template<class T>
	CUDA_CALLABLE_MEMBER Mat<T,2,2> mat(T a,T b,T c, T d){
		Mat<T,2,2> m;
		m[0] = a; m[1] = b;
		m[2] = c; m[3] = d;
		return m;
	}

	template<class T>
	CUDA_CALLABLE_MEMBER Mat<T,3,3> mat(T a,T b,T c, T d,T e,T f,T g, T h, T i){
		Mat<T,3,3> m;
		m[0] = a; m[1] = b; m[2] = c;
		m[3] = d; m[4] = e; m[5] = f; 
		m[6] = g; m[7] = h; m[8] = i;
		return m;
	}

	template<class T>
	CUDA_CALLABLE_MEMBER Mat<T,4,4> mat(T a,T b,T c,T d,T e,T f,T g,T h,T i,T j,T k,T l,T m,T n,T o,T p){
		Mat<T,4,4> m_;
		m_[0]  = a;  m_[1] = b;  m_[2] = c;  m_[3] = d;
		m_[4]  = e;  m_[5] = f;  m_[6] = g;  m_[7] = h;
		m_[8]  = i;  m_[9] = j; m_[10] = k; m_[11] = l;
		m_[12] = m; m_[13] = n; m_[14] = o; m_[15] = p;
		return m_;
	}

	template<class T>
	CUDA_CALLABLE_MEMBER bool inverse(const Mat<T,3,3> &m, Mat<T,3,3> &im){
		T tolerance = T(1e-6);

		T det =  +m(0,0)*(m(1,1)*m(2,2)-m(2,1)*m(1,2))
                 -m(0,1)*(m(1,0)*m(2,2)-m(1,2)*m(2,0))
                 +m(0,2)*(m(1,0)*m(2,1)-m(1,1)*m(2,0));
        if(std::abs(det) <= tolerance) return false;
		T idet = T(1)/det;
		im(0,0) =  (m(1,1)*m(2,2)-m(2,1)*m(1,2))*idet;
		im(1,0) = -(m(0,1)*m(2,2)-m(0,2)*m(2,1))*idet;
		im(2,0) =  (m(0,1)*m(1,2)-m(0,2)*m(1,1))*idet;
		im(0,1) = -(m(1,0)*m(2,2)-m(1,2)*m(2,0))*idet;
		im(1,1) =  (m(0,0)*m(2,2)-m(0,2)*m(2,0))*idet;
		im(2,1) = -(m(0,0)*m(1,2)-m(1,0)*m(0,2))*idet;
		im(0,2) =  (m(1,0)*m(2,1)-m(2,0)*m(1,1))*idet;
		im(1,2) = -(m(0,0)*m(2,1)-m(2,0)*m(0,1))*idet;
		im(2,2) =  (m(0,0)*m(1,1)-m(1,0)*m(0,1))*idet;

		
		return true;
	}

	template<class T>
	CUDA_CALLABLE_MEMBER bool inverse(const Mat<T,4,4> &m, Mat<T,4,4> &im){
		T tolerance = T(1e-6);

		 // Determinants of 2x2 submatrices
        T S0 = m[0] * m[5] - m[1] * m[4];
        T S1 = m[0] * m[6] - m[2] * m[4];
        T S2 = m[0] * m[7] - m[3] * m[4];
        T S3 = m[1] * m[6] - m[2] * m[5];
        T S4 = m[1] * m[7] - m[3] * m[5];
        T S5 = m[2] * m[7] - m[3] * m[6];
 
        T C5 = m[10] * m[15] - m[11] * m[14];
        T C4 = m[9] * m[15] - m[11] * m[13];
        T C3 = m[9] * m[14] - m[10] * m[13];
        T C2 = m[8] * m[15] - m[11] * m[12];
        T C1 = m[8] * m[14] - m[10] * m[12];
        T C0 = m[8] * m[13] - m[9] * m[12];
 
        // If determinant equals 0, there is no inverse
        T det = S0 * C5 - S1 * C4 + S2 * C3 + S3 * C2 - S4 * C1 + S5 * C0;
        if(std::abs(det) <= tolerance) return false;
 
 		T idet = T(1)/det;
        // Compute adjugate matrix

        im[0] =  (m[5] * C5  - m[6] * C4  + m[7] * C3)*idet;
        im[1] = (-m[1] * C5 + m[2] * C4  - m[3] * C3)*idet;
        im[2] =  (m[13] * S5 - m[14] * S4 + m[15] * S3)*idet;
        im[3] = (-m[9] * S5 + m[10] * S4 - m[11] * S3)*idet;
 
        im[4] = (-m[4] * C5  + m[6] * C2  - m[7] * C1)*idet;
        im[5] =  (m[0] * C5 - m[2] * C2  + m[3] * C1)*idet;
        im[6] = (-m[12] * S5 + m[14] * S2 - m[15] * S1)*idet;
        im[7] =  (m[8] * S5 - m[10] * S2 + m[11] * S1)*idet;
 
        im[8] =  (m[4] * C4  - m[5] * C2  + m[7] * C0)*idet;
        im[9] = (-m[0] * C4 + m[1] * C2  - m[3] * C0)*idet;
        im[10] = (m[12] * S4 - m[13] * S2 + m[15] * S0)*idet;
        im[11] =(-m[8] * S4 + m[9] * S2  - m[11] * S0)*idet;

        im[12] =(-m[4] * C3  + m[5] * C1  - m[6] * C0)*idet;
        im[13] = (m[0] * C3 - m[1] * C1  + m[2] * C0)*idet;
        im[14] =(-m[12] * S3 + m[13] * S1 - m[14] * S0)*idet;
        im[15] = (m[8] * S3 - m[9] * S1  + m[10] * S0 ) *idet;

		
		return true;
	}

	
	CUDA_CALLABLE_MEMBER Mat<float,4,4> xrotate(float theta){
		return Matrix::mat(1.f,0.f,0.f,0.f,
				   		   0.f,cosf(theta),-sinf(theta),0.f,
				   		   0.f,sinf(theta),cosf(theta),0.f,
				   		   0.f,0.f,0.f,1.f);
	}
    
    CUDA_CALLABLE_MEMBER Mat<double,4,4> xrotate(double theta){
		return Matrix::mat(1.0,0.0,0.0,0.0,
				   		   0.0,cos(theta),-sin(theta),0.0,
				   		   0.0,sin(theta),cos(theta),0.0,
				   		   0.0,0.0,0.0,1.0);
	}

	CUDA_CALLABLE_MEMBER Mat<float,4,4> yrotate(float theta){
		return Matrix::mat(cosf(theta),0.f,sinf(theta),0.f,
		   		   		   0.f,1.f,0.f,0.f,
		   		   		   -sinf(theta),0.f,cosf(theta),0.f,
		   		   		   0.f,0.f,0.f,1.f);
	}
    
    CUDA_CALLABLE_MEMBER Mat<double,4,4> yrotate(double theta){
		return Matrix::mat(std::cos(theta),0.,std::sin(theta),0.,
		   		   		   0.,1.,0.,0.,
		   		   		   -std::sin(theta),0.,std::cos(theta),0.,
		   		   		   0.,0.,0.,1.);
	}

	CUDA_CALLABLE_MEMBER Mat<float,4,4> zrotate(float theta){
		return Matrix::mat(cosf(theta),-sinf(theta),0.f,0.f,
		   		   		   sinf(theta),cosf(theta),0.f,0.f,
		   		   		   0.f,0.f,1.f,0.f,
		   		   		   0.f,0.f,0.f,1.f);
	}
    
	CUDA_CALLABLE_MEMBER Mat<double,4,4> zrotate(double theta){
		return Matrix::mat(cos(theta),-sin(theta),0.,0.,
		   		   		   sin(theta),cos(theta),0.,0.,
		   		   		   0.,0.,1.,0.,
		   		   		   0.,0.,0.,1.);
	}

	template<class T>
	CUDA_CALLABLE_MEMBER Mat<T,4,4> translate(T x,T y,T z){
		return Matrix::mat(T(1),T(0),T(0),T(x),
				   		   T(0),T(1),T(0),T(y),
				   		   T(0),T(0),T(1),T(z),
				   		   T(0),T(0),T(0),T(1));
	}

	template<class T>
	CUDA_CALLABLE_MEMBER Mat<T,4,4> scale(T xs,T ys,T zs){
		return Matrix::mat(T(xs),T(0),T(0),T(0),
				   		   T(0),T(ys),T(0),T(0),
				   		   T(0),T(0),T(zs),T(0),
				   		   T(0),T(0),T(0),T(1));
	}

	template<class T>
	CUDA_CALLABLE_MEMBER Mat<T,4,4> scale(T s){
		return Matrix::scale(s,s,s);
	}


};

#endif