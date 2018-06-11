#ifndef LambdaMonteCarlo_H
#define LambdaMonteCarlo_H
#include "LightModels/LightModel.h"

// NOTE: before reading this, go over LambdaRaytracer.h 

class LambdaMonteCarlo : public LightModel{
public:
	CUDA_CALLABLE_MEMBER LambdaMonteCarlo(const std::shared_ptr<Scene> &s):LightModel(s){

	}
	CUDA_CALLABLE_MEMBER virtual Vec3f trace(const Ray &ray) const{
		Vec3<float> intensity(0.1f,0.1f,0.1f);
		//leaving this as is now.
		return intensity;
	}
	//Same as trace method but will have more params as:
	//Vec3f radiance(const Ray &ray, float LAMBDA, RANDOM NR) const{....}
	CUDA_CALLABLE_MEMBER float radiance(const Ray &ray) const{
		Vec3<float> intensity(0.1f,0.1f,0.1f);

		/* PSEUDO:
		 * 1. perform hit-test.
		 * 2. get hitPoint, incident ray
		 * 3. get object->normal
		 * 4. if(object is lightsource) return emittance; 
		 * 5. find bounce_vector (Material::SampleHemisphere) 
		      based on current object->material properties
		      using (2.) and (3.). 
		 * 6. use (5.) in (Material::Reflectance) to find radiance
		 * 7. Recursion => Radiance(hitPoint, bounce_vector)
		 * 8. return (radiance for this lambda);
		 */
		return 1.f; 
	}
};

#endif