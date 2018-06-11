#ifndef Sphere_H
#define Sphere_H
#include "Util/Object.h"
#include "Primitive.h"
#include "Util/Vec.h"
#include <cmath>
#define MIN(a,b) a < b ? a : b

class Sphere : public Primitive{
private:
	CUDA_CALLABLE_MEMBER virtual bool local_hit(const Ray &r, float &t) const{
		//Find hit!
		float a = r.dir().dot(r.dir());
		float b = r.pos().dot(r.dir());
		float c = (r_*r_)-r.pos().dot(r.pos());

		float inner = a*c+b*b;
		if(inner >= 0){
			float t1 = (-b+sqrtf(inner))/a;
			float t2 = (-b-sqrtf(inner))/a;
            
			if(t1 >= 0 || t2 >= 0){
				//There exists at least one solution
				if(t1 < 0){
					t = t2;
				}else if(t2 < 0){
					t = t1;
				}else{
					t = MIN(t1,t2);
				}
				return true;
			}else{
				//The solution is behind the ray: no hit! 
				return false;
			}
            
		}else{
			//No real solution: no hit!
			return false;
		}
	}	
	CUDA_CALLABLE_MEMBER virtual Vec3f local_normal(const Vec3f &local) const{
		Vec3f n(local);
		n.normalize();

		return n;
	}
    
    
       
public:
	CUDA_CALLABLE_MEMBER Sphere(float radius = 1.0f):r_(radius){
        
    }

	CUDA_CALLABLE_MEMBER virtual const char* identifier() const{
		return Sphere::identifier();
	}

	CUDA_CALLABLE_MEMBER static const char* Identifier(){
		return "class_Plane";
	}
    
protected:
	float r_;
};
#endif