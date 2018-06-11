#ifndef Scene_H
#define Scene_H
#include "Primitives/Primitive.h"
#include "Primitives/LightSource.h"
#include "Cameras/Camera.h"
#include <vector>
#include <memory>

class Scene{
public:
	CUDA_CALLABLE_MEMBER virtual Primitive *hit(const Ray &r,float &t) const = 0;
    CUDA_CALLABLE_MEMBER virtual Primitive *refractive_hit(const Ray &ray_in,float wavelength,Ray &ray_out,float &t_out, random_kit_t& rnd) const = 0;
	CUDA_CALLABLE_MEMBER virtual std::vector<std::shared_ptr< LightSource >> getLights() const = 0;
    CUDA_CALLABLE_MEMBER virtual std::vector<std::shared_ptr< Primitive >> getObjects() const = 0;
    CUDA_CALLABLE_MEMBER virtual std::shared_ptr< Camera > getCamera() const = 0;

	CUDA_CALLABLE_MEMBER virtual ~Scene(){}

	CUDA_CALLABLE_MEMBER virtual void addPrimitive(const std::shared_ptr<Primitive> &p)= 0;
	CUDA_CALLABLE_MEMBER virtual void addLightSource(const std::shared_ptr<LightSource> &l)= 0;
    CUDA_CALLABLE_MEMBER virtual void setCamera(const std::shared_ptr< Camera > &c)= 0;

protected:
	CUDA_CALLABLE_MEMBER Scene(){}
};
#endif