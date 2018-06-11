#ifndef BlinnPhongShading_H
#define BlinnPhongShading_H

#include "LightModel.h"
#include "Vec.h"
#include "SceneObject.h"
#include "PointLight.h"


#define EPSILON 0.0001

class BlinnPhongShading : public LightModel{
public:
	CUDA_CALLABLE_MEMBER BlinnPhongShading(Scene *s):LightModel(s){

	}
	CUDA_CALLABLE_MEMBER void setParams(float ks,float kd,float ka,float a){
		Ks = ks;
		Kd = kd;
		Ka = ka;
		alfa = a;
	}
	CUDA_CALLABLE_MEMBER ~BlinnPhongShading(){}

	CUDA_CALLABLE_MEMBER virtual Vec3f radiance(const Ray &ray) const{
		float t;
		SceneObject *object = LightModel::scene_->hit(ray,t);
		
		if (object == NULL){ // No hit at all
			return Vec3f(0.f,0.f,0.f);
		}else{
			//Total intesity
			Vec3f I(0.f);
			//Hit position in world
			const Vec3f hitPos(ray.pos()+ray.dir()*t); 
			//Surface normal
			const Vec3f N = object->normal(hitPos); 

			int nLights;
			LightSource** lights;
			lights = LightModel::scene_->getLights(nLights);

			//Phong
			for(int k = 0; k < nLights; k++){
				//if point.
				if(dynamic_cast<PointLight*>(lights[k]) != NULL){
					PointLight *plight = dynamic_cast<PointLight*>(lights[k]);
					Vec3f i = plight->intensity();

					//Vector to Light
					Vec3f L = plight->pos() - hitPos;
					L.normalize();

					Vec3f incident = ray.dir();
				
					float cosAngIncidence = N*L;
					I += Ka*i;

					if(cosAngIncidence > 0){
						// avoid numerical errors, move slightly outside of surf.
						Vec3f point = hitPos + N*EPSILON;
						// ray from point to lightsource.
						Ray shadow_ray(point, L);

						// Here could add Ka*i (ambient), but would req. smoothstep function
						// for smooth attenuation.
						SceneObject *shade_object = LightModel::scene_->hit(shadow_ray,t);
						// Illuminate only if not occluded.
						if(!shade_object){
							// Diffuse component
							I +=  Kd*(N*L)*i;
							// Calculate halfway vector
							Vec3f H = L + incident;
							H.normalize();
							float tmp = sqrtf(H*H);
							if(tmp != 0.0f){
								H = H*(1.0f/tmp);
								// blinnterm
								float blinnTerm = (H*N > 0.f) ? H*N : 0.f;
								// Specular component
								I +=  Ks * std::pow(blinnTerm, alfa)*blinnTerm*i;
							}
						}
					}
				}
			}
			clip(I);
			return I;
		}
	}

protected:
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