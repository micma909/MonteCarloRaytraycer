#ifndef MonteCarlo_Definitions_h
#define MonteCarlo_Definitions_h

#define NDEBUG

//Output image
#define WIDTH (1280/2)
#define HEIGHT (1280/2)


#define EPSILON 0.0001f
#define MAX_INNER_REFRACTION (30)
//Environment
//#define XCODE
/* This is the resolution of the color-matching curves, in nanometers per sample. */
#define RESOLUTION 40


//CUDA
#define CUDA_HELPER_H
#ifdef __CUDACC__
#ifndef CUDA_CALLABLE_MEMBER
#define CUDA_CALLABLE_MEMBER __host__ __device__
#endif
#else
#define CUDA_CALLABLE_MEMBER
#endif


//Tests
/*
#define angle_90_deg(x,y,precision)                                 \
{                                                                   \
    float angle = x.dot(y);                                         \
    bool assertion = angle > -precision && angle < (1.f+precision); \
    if(!assertion){                                                 \
        std::cout << "!!angle = " << angle << std::endl;            \
        x.print();                                                  \
        std::cout << std::endl;                                     \
        y.print();                                                  \
        assert(assertion);                                          \
    }                                                               \
}                                                                   \
*/
#define angle_90_deg(x,y,precision)
//#define assert(x)

#define DYN_FNK(name,out,...)                           \
public:                                                 \
void set_##name(out (*n)(__VA_ARGS__)){                 \
_##name = n;                                        \
}                                                       \
private:                                                \
out (*_##name)(__VA_ARGS__){NULL};                            \

#define P_DYN_FNK(name,out,...)                           \
public:                                                 \
void set_##name(out (*n)(__VA_ARGS__)){                 \
_##name = n;                                        \
}                                                       \
out (*_##name)(__VA_ARGS__){NULL};                            \





#endif
