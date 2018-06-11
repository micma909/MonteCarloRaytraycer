#ifndef StandardScene_H
#define StandardScene_H
#include "Scene.h"
#include <cassert>
#include <set>
#include "Definitions.h"
#include "Util/KDTree.h"
#include "Util/minball.h"
#include "Primitives/Triangle.h"
struct Corner{
    Vec<float, 3> coord;
    Triangle *triangle;
};

class StandardScene : public Scene {
private:
    void range_from_ray(const Ray &t,float x[3],float y[3],float z[3]){

    }
protected:
    CUDA_CALLABLE_MEMBER void snellsLaw(const Vec3f &incident,const Vec3f &N,const Vec3f &x,float n1,float n2,Ray &exitant, random_kit_t& rnd) const{
        
        float n = n1/n2;
        float cosI = -N.dot(incident);
        float sinT2 = 1.f-(n*n)*(1.f-cosI*cosI);
        
        if (sinT2 < 0) {
            //Reflet
            Vec3f v_reflect = ((2.f*cosI)*N)+incident;
            exitant.set_dir(v_reflect);
            exitant.set_pos(x+N*EPSILON);
        }else{
            //refraction 50% chance!
            //Fresnel eqns
            float cosT = std::sqrt(sinT2);
            float r_s = (n1*cosI-n2*cosT)/(n1*cosI+n2*cosT);
            float r_p = (n2*cosI-n1*cosT)/(n2*cosI+n1*cosT);
            float r = ((r_s*r_s + r_p*r_p)/2.f);
            
            //assert(r > 0.f-EPSILON && r < 1.f+EPSILON);
            
            if(rnd.zero_to_one(rnd.generator) < r){
                Vec3f v_reflect = ((2.f*cosI)*N)+incident;
                exitant.set_dir(v_reflect);
                exitant.set_pos(x+N*EPSILON);
            }else{
                //Refract
                Vec3f v_refract;
                v_refract = n*incident+(n*cosI-std::sqrt(sinT2))*N;
                exitant.set_dir(v_refract);
                exitant.set_pos(x-N*EPSILON);
            }
        }
    }
    
    CUDA_CALLABLE_MEMBER virtual Primitive *refractive_hit(const Primitive *orig,float wavelength,Ray ray_in,Ray &ray_out,float &t_out,random_kit_t& rnd,int rdepth ) const{
        if (rdepth > MAX_INNER_REFRACTION) {
            return NULL;
        }
        //Generate new, refracted ray
        Vec3f x = ray_in.pos()+ray_in.dir()*t_out;
        Vec3f N = orig->normal(x);
        
        
        float n1 = 1.00003;
        float n2 = orig->getRefraction()->refracitveIndex(wavelength);
        
        if (N.dot(-1.f*ray_in.dir()) < -EPSILON) {
            N = N*-1;
            std::swap(n1,n2);
        }
        
        snellsLaw(ray_in.dir(),N,x,n1,n2,ray_out, rnd);
        
        //New hit test
        Primitive *new_obj = hit(ray_out, t_out);
        
        if (new_obj == NULL || !new_obj->isRefractive()) {  //Objeketet har lämnats! Returnera nytt objekt
            return new_obj;
        }else{                  //Annars fortsätt refraction
            //std::cout << "ahah" << std::endl;
            return refractive_hit(orig,wavelength, ray_out, ray_out, t_out, rnd, ++rdepth);
        }
    }
public:
    CUDA_CALLABLE_MEMBER virtual Primitive* hit(const Ray &r, float &t) const{
		Primitive *object = NULL;
		t = INFINITY;
        for (int i = 0; i < objects_.size(); i++) {
            
            float tmp_t;
			if(!objects_[i]->hit(r, tmp_t)) continue;
            
			if(tmp_t > 0 && tmp_t < t){
				t = tmp_t;
				object = objects_[i].get();
			}
        }

        
        return object;
	}

    CUDA_CALLABLE_MEMBER virtual Primitive *refractive_hit(const Ray &ray_in,float wavelength,Ray &ray_out,float &t_out, random_kit_t& rnd) const{
        
        Primitive *object = hit(ray_in, t_out);

        ray_out = ray_in; //Assume no refraction, ray in is ray out =)
        //        return object;
        //#warning IMPLEMENTERA snellsLaw !!!1
        if(object != NULL && object->isRefractive()){
            return refractive_hit(object,wavelength, ray_in,ray_out, t_out, rnd, 0);
        }else{
            return object;
        }
    }


	CUDA_CALLABLE_MEMBER virtual std::vector<std::shared_ptr< LightSource > > getLights() const{
		return lights_;
	}
    CUDA_CALLABLE_MEMBER virtual std::vector<std::shared_ptr< Primitive > > getObjects() const{
        return objects_;
    }
    
    CUDA_CALLABLE_MEMBER virtual std::shared_ptr< Camera > getCamera() const{
        return camera_;
    }

	CUDA_CALLABLE_MEMBER StandardScene(){

	}
    CUDA_CALLABLE_MEMBER ~StandardScene(){
        delete tree;
    }

    
    
public:
    
    /*
    CUDA_CALLABLE_MEMBER void optimize(){
        std::vector<Corner> corners;
        max_ =0 ;
        for (std::shared_ptr<Triangle> t : triangles_) {
            
            Vec3f w0 = t->transform()*t->v0_;
            Vec3f w1 = t->transform()*t->v1_;
            Vec3f w2 = t->transform()*t->v2_;

            float min_x = std::min(std::min(w0[0], w1[0]),w2[0]);
            float min_y = std::min(std::min(w0[1], w1[1]),w2[1]);
            float min_z = std::min(std::min(w0[2], w1[2]),w2[2]);
            
            float max_x = std::max(std::max(w0[0], w1[0]),w2[0]);
            float max_y = std::max(std::max(w0[1], w1[1]),w2[1]);
            float max_z = std::max(std::max(w0[2], w1[2]),w2[2]);
            
            max_ = std::max(max_,max_x-min_x);
            max_ = std::max(max_,max_y-min_y);
            max_ = std::max(max_,max_z-min_z);

            Corner c1 ={Vec3<float>(max_x,max_y,max_z),t.get()};
            Corner c2 ={Vec3<float>(max_x,max_y,min_z),t.get()};
            
            Corner c3 ={Vec3<float>(max_x,min_y,max_z),t.get()};
            Corner c4 ={Vec3<float>(max_x,min_y,min_z),t.get()};
            
            Corner c5 ={Vec3<float>(min_x,min_y,min_z),t.get()};
            Corner c6 ={Vec3<float>(min_x,max_y,min_z),t.get()};

            Corner c7 ={Vec3<float>(min_x,min_y,max_z),t.get()};
            Corner c8 ={Vec3<float>(min_x,max_y,max_z),t.get()};
            
            corners.push_back(c1);
            corners.push_back(c2);
            corners.push_back(c3);
            corners.push_back(c4);
            corners.push_back(c5);
            corners.push_back(c6);
            corners.push_back(c7);
            corners.push_back(c8);

        }
        tree = new KDTree<float, 3,Corner>(corners,[](const Corner &c)->Vec<float, 3>{
            return c.coord;
        });
    }*/
    
  
    
	CUDA_CALLABLE_MEMBER virtual void addPrimitive(const std::shared_ptr<Primitive> &p){
        objects_.push_back(p);
	}
	CUDA_CALLABLE_MEMBER virtual void addLightSource(const std::shared_ptr<LightSource> &l){
	    objects_.push_back(l);
    	lights_.push_back(l);
	}
    
    CUDA_CALLABLE_MEMBER virtual void setCamera(const std::shared_ptr<Camera> &c){
	    camera_ = c;
	}
private:
	std::vector<std::shared_ptr< Primitive >> objects_;
	std::vector<std::shared_ptr< LightSource >> lights_;
    std::shared_ptr<Camera> camera_;

    KDTree<float, 3,Corner> *tree;
    float max_,min_;
};
#endif