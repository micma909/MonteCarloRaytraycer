#ifndef FinitePlane_H
#define FinitePlane_H

#include "Definitions.h"
#include "Util/Object.h"
#include "Primitives/Primitive.h"
#include "Primitives/LightSource.h"
#include <math.h>
#include <random>
#ifdef XCODE
#include "Vec2.h"
#else
#include "../Util/Vec2.h"
#endif

class FinitePlane : public LightSource{
protected:
    CUDA_CALLABLE_MEMBER virtual bool local_hit(const Ray &r, float &t) const{
		//Ray info
		const Vec3f O = r.pos();
		const Vec3f D = r.dir();

		// AX + BY + CZ + D = 0. ok.
		float denom = D*N_; //find angle between.
		if(denom >= 0.f) return false;
		t = -(N_*O) / denom;

		Vec3f pos = r.pos()+r.dir()*t; //discard z-dim
		float u = pos[0];
		float v = pos[2];

		return (u <= width_) &&
		 	   (u >= -width_) &&
		 	   (v <= height_) &&
		 	   (v >= -height_);

	}

	CUDA_CALLABLE_MEMBER virtual Vec3f local_normal(const Vec3f &local) const{
        Vec3f n = N_;
		return n;
	}

public: 
	CUDA_CALLABLE_MEMBER FinitePlane():LightSource(),N_(0.f,1.f,0.f),width_(0.5f),height_(0.5f){
        computeArea();
	}

	~FinitePlane(){

	}

	CUDA_CALLABLE_MEMBER virtual Vec3f sampleUniform(random_kit &random) const {
		float u = width_*(random.zero_to_one(random.generator)-0.5f)*2.f;
		float v = height_*(random.zero_to_one(random.generator)-0.5f)*2.f;
		return transform()*Vec4f(u,0,v,1);
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
        Vec3f u(width_*2.f,0,0);
        Vec3f v(0,0,height_*2.f);
        Vec3f u_world = transform()*u.xyz0();
        Vec3f v_world = transform()*v.xyz0();
		Vec3f cross = u_world.cross(v_world);
        surfaceArea_ = cross.length();
    }

protected:
	const Vec3f N_;
    float surfaceArea_;
private:
    const float width_, height_; //Does not need to change!
};
#endif