#ifndef LightSource_H
#define LightSource_H
#include "Util/Vec.h"
#include "Primitives/Primitive.h"
class LightSource : public Primitive{


public:
    CUDA_CALLABLE_MEMBER virtual float surfaceArea() const = 0;
	CUDA_CALLABLE_MEMBER virtual Vec3f sampleUniform(random_kit &random) const = 0;
protected:
	LightSource():Primitive(){}
	CUDA_CALLABLE_MEMBER virtual ~LightSource(){}
};

#endif


