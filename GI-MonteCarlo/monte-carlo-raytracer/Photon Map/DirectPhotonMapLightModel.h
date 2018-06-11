#ifndef __MonteCarlo__DirectPhotonMapLightModel__
#define __MonteCarlo__DirectPhotonMapLightModel__

#include <iostream>
#include <memory>
#include <map>
#include "LightModels/LightModel.h"
#include "LightModels/MonteCarlo.h"
#include "Util/KDTree.h"
#include "Photon Map/Photon.h"

class DirectPhotonMapLightModel : public LightModel{
public:
    //KDTree<float, 3,Photon> *photonMap;
    KDTree<float, 3,Photon> **global_,**caustics_;
    CUDA_CALLABLE_MEMBER DirectPhotonMapLightModel(const std::shared_ptr<Scene> &s, KDTree<float, 3,Photon> **global,KDTree<float, 3,Photon> **caustics):LightModel(s),global_(global),caustics_(caustics){
	}
    CUDA_CALLABLE_MEMBER ~DirectPhotonMapLightModel(){}
    inline float gaussianFilter(float d,float r) const{
        return 1.f/(M_PI*r*r);
        float a = 0.918f;
        float b = 1.953f;
        float d2 = d*d;
        
        float denom = 1.f-std::exp( -b*(d2/(2*r*r)) );
        float enumer = 1.f-std::exp( -b);

        return a*(1-denom/enumer);
    }
    CUDA_CALLABLE_MEMBER float indirectRadiance(const Primitive *p,const Vec3f &x,const Vec3f &indicent,float wavelength) const{
        float radiance = 0;
        Vec3f normal;
        std::shared_ptr<Material> m = p->getMaterial();
        std::multimap<float, Photon > closest =  global_[(int)std::floor((wavelength-380.f)/RESOLUTION)]->search_n(x, 30);
        float r = (closest.rbegin())->first;
        for (auto &it : closest) {
            Photon photon = (it.second);
            float d = (photon.pos()-x).length();
            float wp =  gaussianFilter(d,r);
            normal = p->normal(photon.pos());
            float brdf = m->BRDF(indicent, photon.dir(), normal, x,wavelength);
            radiance += brdf*photon.flux()*wp;
            
        }
        return radiance;
    }
    
    CUDA_CALLABLE_MEMBER virtual float radiance(const Ray &ray_in, float wavelength,random_kit &rnd) const{
        float radiance = 0;
        
        
        std::shared_ptr<Scene> s = LightModel::scene_;
        float t;
        Ray ray;
        Primitive *p = s->refractive_hit(ray_in, wavelength,ray,t,rnd);
        if (p != NULL) {
            //Hit pos
            Vec<float,3> x = ray.pos()+t*ray.dir();
            Vec3f normal = p->normal(x);
            x += normal*(100*EPSILON);
            
            //Global Photon map
            Vec3f incident = ray.dir()*-1.f;
            radiance += indirectRadiance(p, x,incident, wavelength);
        }
        //std::cout << radiance << std::endl;
        return radiance;
    }
    
   };
#endif /* defined(__MonteCarlo__PhotonMapRenderer__) */
