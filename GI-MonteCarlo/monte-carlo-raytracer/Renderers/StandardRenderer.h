#ifndef StandardRenderer_H
#define StandardRenderer_H

#include "Renderer.h"
#include "Cameras/Camera.h"
#include "Util/spectrum.h"
#include "Util/BVH.h"
#include "Spectrum/CIE.h"

#include <iomanip>

template<int W,int H>
class StandardRenderer : public Renderer<W,H>{
    
    float * spectrum(int &N) const{
        float res = 10;
        N = (780-380)/res;
        float *w = new float[N];
        for (int i = 0; i < N; i++) {
            w[i] = res*i+380;
        }
        return w;
    }
public:
	StandardRenderer(const std::shared_ptr<LightModel> &lightModel, const std::shared_ptr<Camera> &camera):Renderer<W,H>(lightModel, camera){

	}
	void compute(Vec3<float> *image) const{
		//Rasterizer r(camera);
        std::cout << "progress:" << std::endl;
		for (int y = 0; y < H; ++y){
			for (int x = 0; x < W; ++x){
                Ray ray = Renderer<W,H>::camera_->rasterSpace(x,y,W,H);
                image[x+W*y] = Renderer<W,H>::lightModel_->radiance(ray,0);

                /*
                Ray ray = Renderer<W,H>::camera_->rasterSpace(x,y,W,H);
                
                int N;
                float *w = nullptr;
                w = spectrum(N);
                float *i = new float[N];
                for (int j = 0; j < N; j++) {
                    i[j] = Renderer<W,H>::lightModel_->radiance(ray,w[j]);
                }
                
                struct colourSystem *cs = &CIEsystem;
                float c_x,c_y,c_z;
                float r,g,b;
                spectrum_to_xyz(i,w,N, &c_x, &c_y, &c_z);
                xyz_to_rgb(cs, c_x, c_y, c_z, &r, &g, &b);
                constrain_rgb(&r, &g, &b);
                norm_rgb(&r, &g, &b);*/

                float radiance[WAVELENGTHS] = {0.0f};
                for (int w = 0; w < WAVELENGTHS; ++w)
                {
                    /* Get a radiance sample for this wavelength. */
                    float wavelength = 380.0f + RESOLUTION * w;
                    radiance[w] += Renderer<W,H>::lightModel_->radiance(ray,wavelength);
                }


            //   Vec3f color = SpectrumToRGB(radiance, colorSystem) / (H*W * WAVELENGTHS);
                //color.print();
               // SpectrumToRGB(radiance, ColorSystems[5]).print();

               image[x+W*y] = SpectrumToRGB(radiance, ColorSystems[ID_CIE]);// * (1.f/ (H*W * WAVELENGTHS));
                
			}
            std::cout << std::setprecision(4) << float(y*WIDTH)/float(W*H)*100 << "% \r";// << std::endl;
            std::cout.flush();
		}
	}
};

#endif