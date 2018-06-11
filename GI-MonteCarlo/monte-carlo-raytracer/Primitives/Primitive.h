#ifndef Primitive_H
#define Primitive_H
#include "Util/Ray.h"
#include "Util/Vec.h"
#include "Util/Object.h"
#include "Materials/Material.h"
#include "Materials/Diffuse.h"
#include "Primitives/Refraction.h"
#include <memory>
class Primitive : public Object{
protected:
	CUDA_CALLABLE_MEMBER virtual bool local_hit(const Ray &r,float &t) const = 0;
public: 
	CUDA_CALLABLE_MEMBER bool hit(const Ray &r,float &t) const{

		//Convert ray to object space
		Vec3f rayPos(itransform()*r.pos().xyz1());
		Vec3f rayDir(itransform()*r.dir().xyz0());
		Ray loc_ray(rayPos,rayDir);

		//Let concrete classes deal with this
		bool hit = local_hit(loc_ray,t);

		if(hit){
			//A hit! Hit location in local coordinates:
			/*Vec3f hitLoc = rayDir*t+rayPos;

			//Convert local to world coordinates:
			Vec3f worldHitLoc = transform()*hitLoc.xyz1();

			//t i world coordinates:
			t = (worldHitLoc-r.pos()).length();

			//Behind ray
			if(t < 0){
				return false;
			}*/
		}
		//Find hit!
		return hit;
	}
	CUDA_CALLABLE_MEMBER virtual Vec3f  normal(const Vec3f &world) const{
		Vec3f local(itransform()*world.xyz1());
		Vec3f N = local_normal(local);
        if(_modify_normal != NULL) _modify_normal(N,this,world);
		N = transform()*N.xyz0();
		N.normalize();
		return N;
	}
	//CUDA_CALLABLE_MEMBER virtual Vec3f  color() const = 0;
	CUDA_CALLABLE_MEMBER virtual const char* identifier() const{
		return Primitive::identifier();
	}

	CUDA_CALLABLE_MEMBER static const char* Identifier(){
		return "virtual_Primitive";
	}

	virtual void setMaterial(const std::shared_ptr<Material> &m){
		m_ = m;
        m_->primitive_ = this;
	}
	virtual std::shared_ptr<Material> getMaterial() const{
		return m_;
	}
    virtual std::shared_ptr<const Refraction> getRefraction() const {
        return refraction_;
    }
    virtual void setRefraction(std::shared_ptr<Refraction> r){
        refraction_ = r;
    }
    virtual bool isRefractive() const{
        return refraction_ != NULL;
    }
public:
    P_DYN_FNK(modify_normal, void, Vec3f &N,const Primitive *p,const Vec3f& x);
protected:
    std::shared_ptr<Refraction> refraction_{NULL};
    CUDA_CALLABLE_MEMBER virtual Vec3f  local_normal(const Vec3f &v) const = 0;

	CUDA_CALLABLE_MEMBER Primitive():Object(){
        m_ = std::shared_ptr<Material>(new Diffuse());
    }
	std::shared_ptr<Material> m_;
};
#endif