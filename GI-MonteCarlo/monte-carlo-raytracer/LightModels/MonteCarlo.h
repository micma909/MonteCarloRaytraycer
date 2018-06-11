#ifndef MonteCarlo_H
#define MonteCarlo_H
#include "LightModel.h"
#include <random>
#include <chrono>
#include <random>

class MonteCarlo : public LightModel{
protected:
	int direct_samples;
	int indirect_samples;
private:
    //Distribution
public:

	MonteCarlo(const std::shared_ptr<Scene> &scene, int direct = 1, int indirect = 1):LightModel(scene), direct_samples(direct), indirect_samples(indirect){

    }
	CUDA_CALLABLE_MEMBER virtual float radiance(const Ray &ray,float wavelength,random_kit_t &random) const{
		int depth = 0;
		return radiance(ray, wavelength,random, depth);
	}

    CUDA_CALLABLE_MEMBER float radianceTransfer(const Vec3f &Nx, const Vec3f &x, const Vec3f &Ny, const Vec3f &y, const Primitive* lightSource,float wavelength, random_kit_t& rnd) const{
        Vec3f dir(y-x);
        float r = dir.length();
        dir *= 1.f/r;
        const Ray ray(x,dir);
        //V
        Ray dummy;
        float dummy_t;
        float V = (LightModel::scene_->refractive_hit(ray, wavelength, dummy, dummy_t, rnd) == lightSource);
        //G
        float a = Nx.dot(dir);
        float b = Ny.dot((-1.f)*dir);
        if(a < 0.f || b < 0.f) return -1.f;
        float G = (a*b)/(r*r);

        return G*V;
    }
	CUDA_CALLABLE_MEMBER float directRadiance(const Primitive *object, const Ray &ray, Vec3f x, float wavelength,random_kit_t &random) const{
		
        std::vector<std::shared_ptr<LightSource> > lights = LightModel::scene_->getLights();
 		if(lights.size() != 1) return 0.f;
 		std::shared_ptr<LightSource> l = lights[0];
        std::shared_ptr<FinitePlane> lightSource = std::dynamic_pointer_cast<FinitePlane>(l);
        if(lightSource != NULL){
            float radiance = 0;
            //Shadow rays
            for(int i = 0; i < direct_samples; i++){
                //Light sources
                Vec3f y = lightSource->sampleUniform(random);
                const Vec3f Ny = lightSource->normal(y);
                y += Ny*EPSILON;

                Vec3f exitant = (y-x);
                exitant.normalize();

                Vec3f incident = ray.dir();
                incident.normalize();
                Vec3f Nx = object->normal(x);
                x += Nx*EPSILON;
                
                float radTransfer = radianceTransfer(Nx,x,Ny,y,lightSource.get(),wavelength, random);
                if(radTransfer != -1){
                    //Not in shadow!
                    float fr = object->getMaterial()->BRDF(incident, exitant, Nx,x, wavelength);
                    radiance += fr*lightSource->getMaterial()->emittance(incident,Ny,wavelength)*radTransfer*lightSource->surfaceArea();
                }

            }
            return radiance/float(direct_samples);
        }
		return 0.f;
	}
	CUDA_CALLABLE_MEMBER float indirectRadiance(const Primitive *object,
                                                const Ray &ray,
                                                Vec3f x,
                                                float wavelength,
                                                random_kit_t &random,
                                                int rdepth) const{
        //if(dynamic_cast<const LightSource * >(object) != NULL){
            //return 0;
        //}
        if(indirect_samples == 0) return 0;
        std::shared_ptr<Material> mat = object->getMaterial();

        
        //End recursion
        int Ni = indirect_samples;
        //float reflectance = std::pow(0.8f,float(rdepth));
        /*float absorbtion = rdepth == 0 ? 0.f : 0.2f;
         float rand = random.zero_to_one(random.generator);
         if(rand < absorbtion || Ni <= 0) return 0.f;
         */

        
        float intensity(0.f);
        
        Vec3f incident = -1*ray.dir();
        incident.normalize();
        Vec3f N = object->normal(x);
        angle_90_deg(incident,N,0.001f);


        x += N*EPSILON;
        float absorbtion = 0.f;
        /*
        float reflectance = 0.5f*mat->_reflectance((*mat.get()),x,wavelength);
        reflectance = std::pow(reflectance,rdepth+1);
        float absorbtion = 1.f-reflectance;
        float rand = random.zero_to_one(random.generator);
        if(rand < absorbtion || Ni <= 0) return 0.f;
        */
        if(rdepth == 3) return 0.f;


        float sum = 0.f;
        
        for(int i = 0; i < Ni; i++){
            
            Vec3f exitant = mat->sampleHemisphere(incident,N,random);
            angle_90_deg(exitant,N,0.001f);
            Ray exitantRay(x, exitant);
            float L = radiance(exitantRay, wavelength, random,rdepth+1);
            //float fr = mat->BRDF(incident, exitant, N, x, wavelength);
            //float pdf = mat->pdf(incident, exitant, N, x, wavelength);
            //float costheta = N.dot(exitant);
            sum += L*mat->importanceSampledRadiance(incident, exitant, N, x, wavelength);
        }
        
        intensity = sum/float(Ni);

		return intensity/(1.f-absorbtion);
	}
    
    CUDA_CALLABLE_MEMBER float radiance(Ray ray_in, float wavelength,random_kit_t &random,int rdepth) const{
        float t;
        Ray ray;
        Primitive *object = LightModel::scene_->refractive_hit(ray_in,wavelength,ray,t,random);
        if(object == NULL){
			return 0.f;
		}
        std::shared_ptr<Material> mat = object->getMaterial();
        Vec3f x = ray.pos() + t*ray.dir();
        Vec3f N = object->normal(x);
        
        float emittance = mat->emittance(ray.dir(), N, wavelength);
        float indirect = indirectRadiance(object, ray, x, wavelength,random, rdepth);
        float direct = directRadiance(object, ray, x, wavelength,random);
        return
        emittance
        + indirect
        + direct;
	}
    
};

#endif