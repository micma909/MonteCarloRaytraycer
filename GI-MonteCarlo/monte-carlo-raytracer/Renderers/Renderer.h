#ifndef Renderer_H
#define Renderer_H
#include "LightModels/LightModel.h"
#include "Cameras/Camera.h"
#include <memory>
//#include <cstddef> //NULL är def. här
template<int W,int H>
class Renderer{
public:
	Renderer(const std::shared_ptr<LightModel> &lightModel, const std::shared_ptr<Camera> &camera):lightModel_(lightModel),camera_(camera){

	}
	~Renderer(){
		//if(lightModel_ != NULL) delete lightModel_;
		//if(camera_ != NULL) delete camera_;
	}
    
	CUDA_CALLABLE_MEMBER virtual void compute(Vec3<float> *image) const = 0;

protected:
	std::shared_ptr<LightModel> lightModel_;
	std::shared_ptr<Camera> camera_;
};


#endif