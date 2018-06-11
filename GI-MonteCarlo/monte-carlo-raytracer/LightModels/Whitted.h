#ifndef Whitted_H
#define Whitted_H
#include "LightModel.h"

#include "Lights/PointLight.h"
#include "Primitives/Sphere.h"
#include "Primitives/FinitePlane.h"
#include "Scenes/StandardScene.h"

class Whitted : public LightModel{
public:
    CUDA_CALLABLE_MEMBER Whitted(const std::shared_ptr<Scene> &s,float ks,float kd,float ka,float a):LightModel(s){
        setParams(ks, kd, ka, a);

    }
    CUDA_CALLABLE_MEMBER void setParams(float ks,float kd,float ka,float a){
		Ks = ks;
		Kd = kd;
		Ka = ka;
		alfa = a;
	}
	CUDA_CALLABLE_MEMBER virtual Vec3f radiance(const Ray &ray) const{
        Vec3f result = radiance(ray,0);
        clip(result);
        return result;
    }
    
protected:
#define MDEPTH 10
    CUDA_CALLABLE_MEMBER virtual Vec3f radiance(const Ray &ray, int depth) const{
        
        if (depth >= MDEPTH) {
            return Vec3f(0.f);
        }else{
            Ray reflectedRay(Vec3f(0.f),Vec3f(0.f));
            bool reflected;
            Vec3f I = phong(ray, reflectedRay, reflected);
            if (reflected) {
                return I+radiance(reflectedRay,depth+1);
            }else{
                return I;
            }
        }
	}
    
    
    CUDA_CALLABLE_MEMBER Vec3f phong(const Ray &ray,Ray &reflRay,bool &reflected) const{
        float t;
		SceneObject *object = LightModel::scene_->hit(ray,t);
        reflected = false;

		if (object == NULL){ // No hit at all
			return Vec3f(0.f);
		}else{
			//Total intesity
			Vec3f I(0.f);
			//Hit position in world
			const Vec3f hitPos(ray.pos()+ray.dir()*t);

			//Surface normal
			//const Vec3f N = object->normal(hitPos);
            /*
			int nLights;
			LightSource** lights;
			lights = LightModel::scene_->getLights(nLights);
            
			//Phong
			for(int k = 0; k < nLights; k++){
				//if point.
				if(typeid(*lights[k]) == typeid(PointLight)){
					PointLight *plight = (PointLight*)lights[k];
					Vec3f i = plight->intensity();
					Vec3f Ii(0.f);
                    
					//Vector to Light
					Vec3f L = plight->pos() - hitPos;
					float distToLight = L.length();
					L *= 1.f/distToLight; //Normalize
					Vec3f reflPos = hitPos+0.001f*N;
                    
					reflRay = Ray(reflPos,L);
					SceneObject *obj = LightModel::scene_->hit(reflRay,t);
					bool hidden = (obj != NULL) &&
                    (t < distToLight);
					if(L.dot(N) < 0 || hidden){
						//Phong ambient only
						Ii = Ka*i;
					}else{
                        
						L.normalize();
                        
						//Reflected vector
						Vec3f R = L.reflect(N);
                        reflected = true;
						//View vector
						Vec3f V = -1.f*ray.dir();
                        
						//Phong reflection model
						Ii = Ka*i + Kd*(L.dot(N))*i+Ks*std::pow(R*V,alfa)*i;
                        
					}
					
                    
					I += (Ii+ object->color())*0.01f;
                    
				}
			}
            */
            
			//Clip
			//clip(I);
			//I.print();
			return I;
		}
    }
    
    CUDA_CALLABLE_MEMBER void clip(Vec3f &I) const{
		I[0] = I[0] > 1.0f ? 1.0f : I[0];
		I[1] = I[1] > 1.0f ? 1.0f : I[1];
		I[2] = I[2] > 1.0f ? 1.0f : I[2];
		I[0] = I[0] < 0.0f ? 0.0f : I[0];
		I[1] = I[1] < 0.0f ? 0.0f : I[1];
		I[2] = I[2] < 0.0f ? 0.0f : I[2];
	}
    
    float Ks;		//specular reflection constant, the ratio of reflection of the specular term of incoming light,
    float Kd;		//diffuse reflection constant, the ratio of reflection of the diffuse term of incoming light (Lambertian reflectance),
    float Ka;		//ambient reflection constant, the ratio of reflection of the ambient term present in all points in the scene rendered, and
    float alfa;		//shininess constant for this material, which is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.
};



#endif