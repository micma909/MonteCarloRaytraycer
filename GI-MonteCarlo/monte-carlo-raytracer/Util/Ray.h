#ifndef Ray_H
#define Ray_H
#include "Vec.h"
class Ray{
public:
    CUDA_CALLABLE_MEMBER Ray(){
		//dir_.normalize();
	}
	CUDA_CALLABLE_MEMBER Ray(const Vec3f &pos, const Vec3f &dir):pos_(pos), dir_(dir){
		//dir_.normalize();
	}
	CUDA_CALLABLE_MEMBER Ray(const Ray &ray):pos_(ray.pos_), dir_(ray.dir_){
		//dir_.normalize();
	}

	CUDA_CALLABLE_MEMBER const Vec3f pos() const{
		return pos_;
	} 
	CUDA_CALLABLE_MEMBER const Vec3f dir() const{
		return dir_;
	}
	CUDA_CALLABLE_MEMBER void set_pos(const Vec3f &pos){
        pos_ = pos;
	}
	CUDA_CALLABLE_MEMBER void set_dir(const Vec3f &dir){
        dir_ = dir;
	}
protected:
	Vec3f pos_;
	Vec3f dir_;
};

#endif