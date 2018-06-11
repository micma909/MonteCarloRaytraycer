#ifndef PhongShading_H
#define PhongShading_H

#include "LightModel.h"
#include "Util/Vec.h"
#include "Primitives/Primitive.h"
#include <typeinfo>
#include <vector>

class PhongShading : public LightModel{
public:
	CUDA_CALLABLE_MEMBER PhongShading(const std::shared_ptr<Scene> &s,float ks =0.2f,float kd =0.2f,float ka =0.3f,float a =5.f):LightModel(s){
        setParams(ks, kd, ka, a);
	}
	CUDA_CALLABLE_MEMBER void setParams(float ks,float kd,float ka,float a){
		Ks = ks;
		Kd = kd;
		Ka = ka;
		alfa = a;
	}
	CUDA_CALLABLE_MEMBER ~PhongShading(){}

	CUDA_CALLABLE_MEMBER virtual float radiance(const Ray &ray,float wavelength,random_kit &rnd) const{
		float t;
		Primitive *object = LightModel::scene_->hit(ray,t);
		
		if (object == NULL){ // No hit at all
			return 0.f;
		}else{
			//Total intesity
			float I(0.f);
			//Hit position in world
			const Vec3f hitPos(ray.pos()+ray.dir()*t); 
			//Surface normal
			const Vec3f N = object->normal(hitPos); 

			std::vector<std::shared_ptr< LightSource >>  lights;
			lights = LightModel::scene_->getLights();

			//Phong
            std::shared_ptr<Camera> camera = LightModel::scene_->getCamera();
            //Get camera location and place lightsource there
            Vec3f lightPos = camera->transform()*Vec4f(0.f,0.f,0.f,1.f);
            
            float i = 1.f;
            float Ii(0.f);
            
            //Vector to Light
            Vec3f L = lightPos - hitPos;
            float distToLight = L.length();
            L *= 1.f/distToLight; //Normalize
            Vec3f reflPos = hitPos+0.0001f*N;
            
            Ray reflRay(reflPos,L);
            Primitive *obj = LightModel::scene_->hit(reflRay,t);
            bool hidden = (obj != NULL) &&
            (t < distToLight);
            if(L.dot(N) < 1e-3 || hidden){
                //Phong ambient only
                Ii = Ka*i;
            }else{
                
                L.normalize();
                
                //Reflected vector
                Vec3f R = L.reflect(N);
                
                //View vector
                Vec3f V = -1.f*ray.dir();
                
                //Phong reflection model
                Ii = Ka*i + Kd*(L.dot(N))*i+Ks*std::pow(R*V,alfa)*i;
                
            }
            
            I  += Ii*1.f;
        
			return I;
		}
	}

protected:
	CUDA_CALLABLE_MEMBER void clip(float &I) const{
		I = I > 1.0f ? 1.0f : I;
        I = I < 0.0f ? 0.0f : I;

	}

	float Ks;		//specular reflection constant, the ratio of reflection of the specular term of incoming light,
	float Kd;		//diffuse reflection constant, the ratio of reflection of the diffuse term of incoming light (Lambertian reflectance),
	float Ka;		//ambient reflection constant, the ratio of reflection of the ambient term present in all points in the scene rendered, and
	float alfa;		//shininess constant for this material, which is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.
};

#endif