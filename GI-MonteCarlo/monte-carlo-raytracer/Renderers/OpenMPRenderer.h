#ifndef OpenMPRenderer_H
#define OpenMPRenderer_H
#include "Renderer.h"
#include "Cameras/Camera.h"
#include "Spectrum/CIE.h"

#include <iomanip>
//#include <ncurses.h>
#include "Util/spectrum.h"

#ifdef __APPLE__
#include <omp.h>
#elif defined _WIN32 || defined _WIN64
#include <C:/MinGW32/lib/gcc/mingw32/4.7.0/include/omp.h>
#endif


template<int W,int H>
class OpenMPRenderer : public Renderer<W,H>{
private:
    int samples_per_pixel_;
public:
    DYN_FNK(percentage_callback, void,int,float,float,float,float);
    DYN_FNK(pass_finished_callback, void,const OpenMPRenderer<W,H> *,const Vec3<float> *image,int);
public:

	OpenMPRenderer(const std::shared_ptr<LightModel> &lightModel, 
        const std::shared_ptr<Camera> &camera
        int samples_per_pixel):Renderer<W,H>(lightModel, camera),samples_per_pixel_(samples_per_pixel){
        //spectrum_ = Spectrum::visible(100);
	}
    
    
	void compute(Vec3<float> *image) const{
		//Rasterizer r(camera);
        omp_set_num_threads(8);
        
		#pragma omp parallel shared(image)
        {
            #pragma omp single
            {
                std::cout << omp_get_num_threads() << std::endl << "progress:" << std::endl;
            }
            
            
        }
	}
};

#endif