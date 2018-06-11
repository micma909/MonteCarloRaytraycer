#ifndef Plane_H
#define Plane_H
#include "Util/Object.h"
#include "Primitive.h"
#include <math.h>

class Plane : public Primitive{
protected:
    CUDA_CALLABLE_MEMBER virtual bool local_hit(const Ray &r, float &t) const{
		//Ray info
		const Vec3f D = r.dir();

		// AX + BY + CZ + D = 0. ok.
		float denom = D*N_; //find angle between.
		if(denom >= 0.f) return false;
        const Vec3f O = r.pos();
		t = -(N_*O) / denom;
		return true;
	}

	CUDA_CALLABLE_MEMBER virtual Vec3f local_normal(const Vec3f &local) const{
        Vec3f n = N_;
		return n;
	}
    
public: 
	CUDA_CALLABLE_MEMBER Plane():N_(0.f,1.f,0.f){

	}

	CUDA_CALLABLE_MEMBER virtual const char* identifier() const{
		return Plane::identifier();
	}

	CUDA_CALLABLE_MEMBER static const char* Identifier(){
		return "class_Primitive";
	}
protected:
	const Vec3f N_;
};
#endif