#ifndef AbstractSceneFactory_H
#define AbstractSceneFactory_H
#include "Scene.h"
#include "Cameras/Camera.h"

class AbstractSceneFactory{
public:
	static CUDA_CALLABLE_MEMBER std::shared_ptr< Scene > scene(){
		return NULL;
	}
};
#endif