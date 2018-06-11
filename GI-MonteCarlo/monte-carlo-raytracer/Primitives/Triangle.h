#ifndef Triangle_H
#define Triangle_H

#include "Primitive.h"
#include "Util/Vec3.h"
#include "Util/Mat.h"

/*
    C
   /\
  /  \
 /____\
B      A

*/

class Triangle : public Primitive {
private:
    //cached values
    Vec3f e0_,e1_;
    Vec3f N_;
public:
    //Original
    const Vec3f v0_,v1_,v2_;

protected:
    CUDA_CALLABLE_MEMBER virtual bool local_hit(const Ray &r, float &t) const{
        
        //Begin calculating determinant - also used to calculate u parameter
        Vec3f P = r.dir().cross(e1_);
        //if determinant is near zero, ray lies in plane of triangle
        float det = e0_.dot(P);
        if(det > -EPSILON && det < EPSILON){
            return false;
        }
        //calculate distance from V1 to ray origin
        float inv_det = 1.f/det;
        Vec3f T = r.pos()-v0_;
        
        //Calculate u parameter and test bound
        float u = T.dot(P) * inv_det;
        //The intersection lies outside of the triangle
        if(u < 0.f || u > 1.f) return false;
        //Prepare to test v parameter
        Vec3f Q = T.cross(e0_);// CROSS(Q, T, e1);
        //Calculate V parameter and test bound
        float v = r.dir().dot(Q) * inv_det;
        //The intersection lies outside of the triangle
        if(v < 0.f || u + v  > 1.f) return false;
        t = e1_.dot(Q) * inv_det;
        if(t > EPSILON) { //ray intersection
            return true;
        }
        
        // No hit, no win
        return false;
	}
    //
	CUDA_CALLABLE_MEMBER virtual Vec3f local_normal(const Vec3f &local) const{
        //Disabled
        assert(false);
        return N_;
	}
    
    CUDA_CALLABLE_MEMBER virtual Vec3f  normal(const Vec3f &world) const{
		return N_;
	}

    void init(){
        //Recompute edges
        e0_ = v1_-v0_;
        e1_ = v2_-v0_;

    }
    
    CUDA_CALLABLE_MEMBER virtual void setTransform(const Mat4x4f &m){
        Primitive::setTransform(m);
        //Recompute normal
        Vec3f w0 = transform()*v0_;
        Vec3f w1 = transform()*v1_;
        Vec3f w2 = transform()*v2_;

        Vec3f we0 = w1-w0;
        Vec3f we1 = w2-w0;
        N_ = (we1).cross(we0);
        N_.normalize();
        N_ = N_*-1;
	}
    
public:
	CUDA_CALLABLE_MEMBER Triangle(const Vec3f &v0,const Vec3f &v1,const Vec3f &v2):v0_(v0),v1_(v1),v2_(v2){
        init();
        //Recompute normal
        N_ = (e1_).cross(e0_);
        N_.normalize();
	}
    CUDA_CALLABLE_MEMBER Triangle(const Vec3f &v0,const Vec3f &v1,const Vec3f &v2, const Vec3f &n):v0_(v0),v1_(v1),v2_(v2),N_(n){
        N_.normalize();
        init();
	}
    
    Vec3f getNormal() const{
        return N_;
    }
    void setNormal(const Vec3f &n){
        N_ = n;
    }
};

#endif
