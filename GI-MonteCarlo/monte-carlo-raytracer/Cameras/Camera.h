#ifndef Camera_H
#define Camera_H
#include <stdio.h>
#include <cassert>
#include "Util/Ray.h"
#include "Util/Vec.h"
#include "Util/Object.h"

class Camera : public Object{
public:
    CUDA_CALLABLE_MEMBER Camera(float aspect_ratio,float fov_degrees):aspect_ratio_(aspect_ratio),fov_degrees_(fov_degrees),Object(){

    }
    //x : -1 .. 1
    //y : -1 .. 1
    CUDA_CALLABLE_MEMBER Ray screenSpace(float x,float y) const{

    	//assert(x >= -1 && x <= 1);
    	//assert(y >= -1 && y <= 1);

    	//x : -aspect_ratio_ .. aspect_ratio_
    	//y : -1 .. 1
    	x = x*aspect_ratio_;

    	Vec3f localPos(x,y,0.f);

    	float fov_radians = (fov_degrees_*M_PI)/180.f;
    	float eye_z =  aspect_ratio_*atan(fov_radians/2.f);
    
    	Vec3f eye_pos(0.f,0.f,eye_z);

    	Vec3f rayDir(localPos-eye_pos);
 
        rayDir = Object::transform()*(rayDir.xyz0());
       	rayDir.normalize();
    	Vec3f rayPos = Object::transform()*eye_pos.xyz1();
    
    	return Ray(rayPos,rayDir);
    }

    CUDA_CALLABLE_MEMBER Ray rasterSpace(int ix,int iy,int W,int H) const{
    	float x = (((float)ix+0.5f)/(float)W)*2.f-1.f;
    	float y = (((float)iy+0.5f)/(float)H)*2.f-1.f;
        if(ix == 0 && iy == 2){
            //printf("x:%f y:%f",x,y);
        }
    	return screenSpace(x,y);
    }
    
    CUDA_CALLABLE_MEMBER Ray rasterSpace(float ix,float iy,int W,int H) const{
    	float x = (((float)ix+0.5f)/(float)W)*2.f-1.f;
    	float y = (((float)iy+0.5f)/(float)H)*2.f-1.f;
        if(ix == 0 && iy == 2){
            //printf("x:%f y:%f",x,y);
        }
    	return screenSpace(x,y);
    }

    CUDA_CALLABLE_MEMBER void set_aspect_ratio(float ar){
        aspect_ratio_ = ar;
    }

    CUDA_CALLABLE_MEMBER void set_fov_degrees(float fov){
        fov_degrees_ = fov;
    }

    CUDA_CALLABLE_MEMBER float aspect_ratio() const{
        return aspect_ratio_;
    }

    CUDA_CALLABLE_MEMBER float fov_degrees() const{
        return fov_degrees_;
    }
protected:
	float aspect_ratio_;
	float fov_degrees_;
};

#endif