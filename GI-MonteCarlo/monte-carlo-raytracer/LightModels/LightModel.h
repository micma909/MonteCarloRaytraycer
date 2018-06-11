#ifndef LightModel_H
#define LightModel_H
#include "Util/Vec.h"
#include "Util/Ray.h"
#include "Scenes/Scene.h"
#include "Util/random_kit.h"

#include <vector>
#include <memory>

class LightModel{
public:
	CUDA_CALLABLE_MEMBER LightModel(const std::shared_ptr<Scene> &scene):scene_(scene){}
	CUDA_CALLABLE_MEMBER ~LightModel(){
		//if(scene_ != NULL) delete scene_;
	}
	CUDA_CALLABLE_MEMBER virtual float radiance(const Ray &ray, float wavelength,random_kit &rnd) const = 0;
protected:
	std::shared_ptr<Scene> scene_;
};

#endif