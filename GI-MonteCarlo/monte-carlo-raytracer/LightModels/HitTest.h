#ifndef HitTest_H
#define HitTest_H
#include "LightModel.h"
#include "Util/Vec.h"
#include "Primitives/Sphere.h"
#include "Primitives/Plane.h"
#include "Primitives/Primitive.h"

class HitTest : public LightModel{
public:
	CUDA_CALLABLE_MEMBER HitTest(const std::shared_ptr<Scene> &s):LightModel(s){}
	CUDA_CALLABLE_MEMBER ~HitTest(){}

	CUDA_CALLABLE_MEMBER virtual float radiance(const Ray &ray,float wavelength,random_kit &rnd) const{
		float t;
		Primitive* object = LightModel::scene_->hit(ray,t);
		
		if (object != NULL){ 
			return 1.f;
		}else{
		    // No hit at all
			return 0.f;
		}
	}
protected:
	
};

#endif