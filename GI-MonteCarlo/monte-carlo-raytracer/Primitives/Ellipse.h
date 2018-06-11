#ifndef Ellipse_H
#define Ellipse_H
#include "Definitions.h"
#include "Util/Object.h"
#include "Primitives/Primitive.h"
#include "Primitives/LightSource.h"
#include <math.h>
#include <random>

class Ellipse : public LightSource{
private:
protected:
    CUDA_CALLABLE_MEMBER virtual bool local_hit(const Ray &r, float &t) const{
		//Ray info
		return false;
	}
    
	CUDA_CALLABLE_MEMBER virtual Vec3f local_normal(const Vec3f &local) const{
		return Vec3f(0.f,1.f,0.f);
	}
       
public:
	CUDA_CALLABLE_MEMBER Ellipse(float a,float b):LightSource(),a_(1.f),b_(1.f){
        computeArea();
	}
    
	~Ellipse(){
        
	}
    
	CUDA_CALLABLE_MEMBER float rndStandard() const{
        return (double) std::rand() / (RAND_MAX);
    }
    
	CUDA_CALLABLE_MEMBER virtual Vec3f sampleUniform() const {
		//float u = width_*(rndStandard()-0.5f)*2.f;
		return Vec4f(0,0,0,1);
	}
    
    CUDA_CALLABLE_MEMBER virtual float surfaceArea() const{
        return surfaceArea_;
    }
    
    CUDA_CALLABLE_MEMBER void setTransform(const Mat4x4f &m){
        Object::setTransform(m);
        computeArea();
    }
    
    CUDA_CALLABLE_MEMBER void computeArea(){
        //Recompute area!

    }
    
protected:
    float surfaceArea_;
private:
    float a_,b_;
};
#endif