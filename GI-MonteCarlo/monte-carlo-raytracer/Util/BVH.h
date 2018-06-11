#ifndef BVH_h
#define BVH_h

#include "Vec.h"
#include "Photon Map/Photon.h"
#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <queue>

class BVH : public Primitive {
protected:
    std::vector<std::shared_ptr<Triangle> > triangles_;
public:
    BVH(std::vector<std::shared_ptr<Triangle> > triangles):triangles_(triangles){
        float min[3] = {INFINITY};
        float max[3] = {-INFINITY};

        for(std::shared_ptr<Triangle> &t : triangles_){
            Vec3f w0 = t->transform()*t->v0_;
            Vec3f w1 = t->transform()*t->v1_;
            Vec3f w2 = t->transform()*t->v2_;

            min[0] = std::min(w0[0],min[0]);
            min[1] = std::min(w0[1],min[1]);
            min[2] = std::min(w0[2],min[2]);

            min[0] = std::min(w1[0],min[0]);
            min[1] = std::min(w1[1],min[1]);
            min[2] = std::min(w1[2],min[2]);
            
            min[0] = std::min(w2[0],min[0]);
            min[1] = std::min(w2[1],min[1]);
            min[2] = std::min(w2[2],min[2]);
            
            
            max[0] = std::max(w0[0],max[0]);
            max[1] = std::max(w0[1],max[1]);
            max[2] = std::max(w0[2],max[2]);
            
            max[0] = std::max(w1[0],max[0]);
            max[1] = std::max(w1[1],max[1]);
            max[2] = std::max(w1[2],max[2]);
            
            max[0] = std::max(w2[0],max[0]);
            max[1] = std::max(w2[1],max[1]);
            max[2] = std::max(w2[2],max[2]);
        }
    }
public:

    
};

#endif









