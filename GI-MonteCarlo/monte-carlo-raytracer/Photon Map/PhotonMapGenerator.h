#ifndef MonteCarlo_PhotonMapGenerator_h
#define MonteCarlo_PhotonMapGenerator_h
#include "Scenes/Scene.h"
#include "Util/KDTree.h"
#include "Materials/Diffuse.h"
#include <vector>
#include "Definitions.h"

class PhotonMapGenerator {
public:
    
    bool russianRoulette(){
        float r = (double) std::rand() / (RAND_MAX);
        return r < 0.5;
    }
    

    Vec3f generateRandVecFromHemisphere(const Vec3f &n,random_kit_t &rnd) const{
        float r1 = rnd.zero_to_one(rnd.generator);
        float r2 = rnd.zero_to_one(rnd.generator);
        
        float phi = (r1)*M_PI*2.f;
        float theta = (r2)*M_PI_2;
        Vec3f exitant(std::cos(phi)*std::sin(theta),std::cos(theta),std::sin(phi)*std::sin(theta));
		exitant = exitant.rotate(n);
		exitant.normalize();
 		return exitant;
	}
    
    virtual KDTree<float, 3,Photon>* generateGlobalPhotonMap(const std::shared_ptr<Scene> scene){
        
        random_kit_t random = random_kit_t::create_kit();
        std::vector<Photon> photons;
        std::shared_ptr<LightSource> lightSource = scene->getLights()[0];
        Vec3f dummy;
        float emittance = lightSource->getMaterial()->emittance(dummy, dummy, 0);
        float energy = emittance*lightSource->surfaceArea();
        
        //int nr_bounces =  1;
        int nr_initial_photons_per_wavelength = 1000000/WAVELENGTHS;
        Vec3f normal(0,-1,0);
        float reflectance = 0.8;
        
        //float radiance[WAVELENGTHS] = {0.0f};
        for (int w = 0; w < WAVELENGTHS; ++w){
            // Get a radiance sample for this wavelength.
            float wavelength = 380.0f + RESOLUTION * w;                
            for (int i = 0; i < nr_initial_photons_per_wavelength; i++) {
                    Photon p ;//= source->generateRanomPhoton(random);
                    p.wavelength() = wavelength;
                    p.pos() = Vec3f(0.f,4.9f,0.f);
                    p.dir() = generateRandVecFromHemisphere(normal,random);
                    p.flux() = 1;
                    bool isFirst = true;
                    do{
                        float t;
                        Ray ray_out;
                        Ray ray_in(p.pos(), p.dir());
                        Primitive *primitive = scene->refractive_hit(ray_in, wavelength,ray_out,t,random);
                        
                        //if (primitive != NULL && !(ray_in.pos() == ray_out.pos()) ) { //Only store caustics
                        if (primitive != NULL ) {
                            
                            //Set photon position & direction!
                            p.pos() = ray_out.pos()+ray_out.dir()*t;
                            //Find normal
                            Vec3f normal = primitive->normal(p.pos());
                            //Move out of object
                            p.pos() += (10.f*EPSILON)*normal;
                            p.dir() = ray_out.dir();
                            //Store photon!
                            if(!isFirst) photons.push_back(p);
                            isFirst = false;
                            //rouski rouletti!
                            
                            //move along the normal
                            //p = Photon();
                            //p.pos() += normal*EPSILON;
                            //Generate new direction
                            /*Vec3f V = p.dir();
                            Vec3f R = V-2*(V.dot(normal))*normal;
                            R.normalize();
                            */
                            
                            p.dir() = generateRandVecFromHemisphere(normal, random);
                        }else{
                            break; //Cancel loop - no more objects will be hit
                        }
                    }while (random.zero_to_one(random.generator) < reflectance);
                }
            
        }
        float flux = energy/float(photons.size());
        for(Photon &p : photons){
            p.flux() = flux;
        }
        
        //Create kd-trees
        KDTree<float, 3,Photon> *tree = new KDTree<float, 3,Photon>(photons,[](const Photon &p)->Vec<float, 3>{
            return p.pos();
        });
        
        return tree;
    }
    
    virtual KDTree<float, 3,Photon>** generateCausticsPhotonMap(const std::shared_ptr<Scene> scene){
        std::vector<Photon> photons[WAVELENGTHS];

        
        //TEST SCENE
        bool has_at_least_one_refractive = false;
        auto primitives = scene->getObjects();
        for(auto &primitive : primitives){
            if(primitive->isRefractive()){
                has_at_least_one_refractive = true;
                break;
            }
        }

        if(!has_at_least_one_refractive){
            std::cout << "Warning: No refractive objects scene" << std::endl;
        }else{
            random_kit_t random = random_kit_t::create_kit();
            std::shared_ptr<LightSource> lightSource = scene->getLights()[0];
            Vec3f dummy;
            float emittance = lightSource->getMaterial()->emittance(dummy, dummy, 0);
            float energy = emittance*lightSource->surfaceArea();
            
            //int nr_bounces =  1;
            int total_number_of_photons = 80000;
            float flux = energy/float(total_number_of_photons);
            const int max_nr_per_wavelength = total_number_of_photons/WAVELENGTHS;
            Vec3f normal(0,-1,0);
            
            //Skickat ut fotoner i scenen
            for (int w = 0; w < WAVELENGTHS; ++w){
                // Get a radiance sample for this wavelength.
                float wavelength = 380.0f + RESOLUTION * w;
                int nr_photons = 0;
                
                while (nr_photons < max_nr_per_wavelength) {
                    //Create photon
                    Photon p;
                    p.wavelength() = wavelength;
                    p.pos() = lightSource->sampleUniform(random)+EPSILON*normal;
                    p.dir() =  generateRandVecFromHemisphere(normal,random);
                    p.flux() = flux;
                    
                    //Send out in scene
                    float t;
                    Ray ray_out;
                    Ray ray_in(p.pos(), p.dir());
                    Primitive *primitive = scene->refractive_hit(ray_in, wavelength,ray_out,t,random);
                    //Only store caustics on diffuse surfaces
                    if (primitive != NULL
                        && !(ray_in.pos() == ray_out.pos())
                        && dynamic_cast<Diffuse*>(primitive->getMaterial().get()) != NULL) {
                        //Set photon position & direction!
                        p.pos() = ray_out.pos()+ray_out.dir()*t;
                        p.dir() = ray_out.dir();
                        //Find normal
                        Vec3f normal = primitive->normal(p.pos());
                        //Move out of object
                        p.pos() += (10.f*EPSILON)*normal;
                        //Store photon!
                        photons[w].push_back(p);
                        nr_photons++;
                    }
                }
            }
        }
        
        std::cout << "Photons in scene, building KDtree..." << std::endl;
        
        KDTree<float, 3,Photon> **tree = new KDTree<float, 3,Photon>*[WAVELENGTHS];
        for(int i = 0 ; i < WAVELENGTHS; i++){
            tree[i] = new KDTree<float, 3,Photon>(photons[i],[](const Photon &p)->Vec<float, 3>{
                return p.pos();
            });
        }
        
        return tree;
    }
};

#endif
