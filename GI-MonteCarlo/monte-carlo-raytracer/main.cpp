#include <stdio.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <memory>
#include <random>
#include <iomanip>

#include "Definitions.h"
#include "Scenes/CornellBox.h"
#include "Scenes/CornellBox_720p.h"
#include "Scenes/CornellBox_cubes.h"
#include "Scenes/SphereScene.h"
#include "Scenes/BoxScene.h"
#include "Util/Mat.h"
#include "Cameras/Camera.h"
#include "Renderers/Renderer.h"
#include "LightModels/MonteCarlo.h"
#include "LightModels/MonteCarloCaustics.h"
#include "LightModels/LightModel.h"
#include "LightModels/SceneSignature.h"
#include "LightModels/HitTest.h"
#include "LightModels/PhongShading.h"
#include "Renderers/StandardRenderer.h"
#include "Util/savebmp.h"
#include "Primitives/Sphere.h"
#include "Scenes/StandardScene.h"
#include "Photon Map/PhotonMapGenerator.h"
#include "Materials/Material.h"
#include "Scenes/CornellBox_720p_prism.h"

//#include "Renderers/OpenMPIRenderer.h"

#include "Util/Timer.h"
#include "Renderers/PthreadsRenderer.h"

#ifdef __APPLE__
// #include <omp.h>
#define ENABLE_OPENEXR 1
#define ENABLE_BMP 0
#define PHOTONMAP 0
#define CAUSTICS 1
#include <half.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#elif defined _WIN32 || defined _WIN64
#define ENABLE_OPENEXR 0
#define ENABLE_BMP 1
#define PHOTONMAP 0
#define CAUSTICS 0
#include "Renderers/OpenMPRenderer.h"
#include <C:/MinGW32/lib/gcc/mingw32/4.7.0/include/omp.h>
#else
#include <half.h>
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#define ENABLE_OPENEXR 1
#define ENABLE_BMP 0
#define PHOTONMAP 0
#define CAUSTICS 0
#endif


#if PHOTONMAP == 1
#include "Photon Map/PhotonMapLightModel.h"
#include "Photon Map/DirectPhotonMapLightModel.h"
#include "Photon Map/PhotonMapGenerator.h"
#endif

//#define PHOTONMAP
#if ENABLE_OPENEXR == 1
void
writeRgba (const char fileName[],
            const Imf::Rgba *pixels,
            int width,
            int height)
{
    //
    // Write an RGBA image using class RgbaOutputFile.
    //
    //	- open the file
    //	- describe the memory layout of the pixels
    //	- store the pixels in the file
    //
    
    
    Imf::RgbaOutputFile file (fileName, width, height, Imf::WRITE_RGBA);
    file.setFrameBuffer (pixels, 1, width);
    file.writePixels (height);
}



void normalize(Vec3f *image){
    //Find max
    float max = image[0].length();
    for (int i = 1; i < WIDTH*HEIGHT; i++) {
        float current = image[i].length();
        if(current > max){
            max = current;
        }
    }
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        image[i] *= 1.f/max;
    }
    
}

void imageToRGBA(Imf::Rgba *pixels,const Vec3f *image,int width,int height){
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            pixels[x+(height-y-1)*width] = Imf::Rgba(image[x+y*width][0]/255.f,
                                                     image[x+y*width][1]/255.f,
                                                     image[x+y*width][2]/255.f);
        }
    }
}
void readRgbaToArray(const char fileName[], Vec3f *image_from_sequence){
  Imf::RgbaInputFile file ( fileName );
  Imath::Box2i dw = file.dataWindow();
  int width = dw.max.x - dw.min.x + 1;
  int height = dw.max.y - dw.min.y + 1;
  std::cout << width << "x" << height << std::endl;
  Imf::Array2D<Imf::Rgba> pixels;
  pixels.resizeErase (height, width);
  file.setFrameBuffer( &pixels[0][0] - dw.min.x - dw.min.y * width, 1, width );
  file.readPixels (dw.min.y, dw.max.y);

  //Vec3f *loaded_img = new Vec3f [width*height];

  for(int x = 0; x < width; ++x){
    for(int y = 0; y < height; ++y){

        image_from_sequence[x+(height-y-1)*width][0] = (pixels[y][x].r)*255.f;
        image_from_sequence[x+(height-y-1)*width][1] = (pixels[y][x].g)*255.f;
        image_from_sequence[x+(height-y-1)*width][2] = (pixels[y][x].b)*255.f;
    }
  }
}


#endif

void percentage(int pass,float total_p,float pass_p,float time_elapsed,float time_left){
    static int c_pass = 0;
    if(c_pass != pass){
        std::cout << pass << ": " <<  std::setprecision(4) << total_p << "% (" << std::setprecision(4) << pass_p << "%) Elapsed time:" << time_elapsed << " time left: "<< (int)time_left << "(s)" << std::endl;
        c_pass = pass;
    }
}

void finished(const PthreadsRenderer<WIDTH,HEIGHT> *renderer,
              const Vec3<float> *image ,
              int pass){
    #if ENABLE_BMP == 1
    std::cout << "Current pass: " << pass << std::endl;
    std::ostringstream ss_bmp;
    ss_bmp << "tmp/bmp_result_" << pass << ".bmp";
    savebmp(ss_bmp.str().c_str(),WIDTH,HEIGHT, 72, image);
    #endif
    #if ENABLE_OPENEXR == 1
    std::ostringstream ss_rgba;
    ss_rgba << "tmp/exr_result_" << pass << ".exr";
    Imf::Rgba *rgba = new Imf::Rgba[WIDTH*HEIGHT];
    imageToRGBA(rgba,image,WIDTH,HEIGHT);
    writeRgba(ss_rgba.str().c_str(),rgba,WIDTH,HEIGHT);
    delete [] rgba;
    #endif
}

int main(int narg,char *args[]){
    //Parameters
    int samples_per_pixel = 1;
    int indidect = 1;
    int direct = 1;
	std::shared_ptr<Scene> scene = SphereScene::scene();
    std::shared_ptr<Camera> camera = scene->getCamera();

    
#if PHOTONMAP == 1
    
     std::shared_ptr<LightSource> lightSource = scene->getLights()[0];
     std::shared_ptr<PhotonMapGenerator> generator(new PhotonMapGenerator());
     KDTree<float, 3,Photon> *global = generator->generateGlobalPhotonMap(scene);
     std::shared_ptr<LightModel> lightModel (new PhotonMapLightModel(scene,global,global));
    
    //std::shared_ptr<LightModel> lightModel_preview (new DirectPhotonMapLightModel(scene,trees,trees));
    //PthreadsRenderer<WIDTH,HEIGHT> raytracer_preview(lightModel_preview,camera,1,8);

#else
    std::shared_ptr<LightModel> lightModel_preview (new SceneSignature(scene));
    PthreadsRenderer<WIDTH,HEIGHT> raytracer_preview(lightModel_preview,camera,1,8);

#if CAUSTICS == 1
    std::shared_ptr<PhotonMapGenerator> generator(new PhotonMapGenerator());
    KDTree<float, 3,Photon> **caustics = generator->generateCausticsPhotonMap(scene);
    std::cout << "Casutic map done!" << std::endl;
    std::shared_ptr<LightModel> lightModel (new MonteCarloCaustics(scene,caustics,direct,indidect));
#else
    std::shared_ptr<LightModel> lightModel (new MonteCarlo(scene,direct,indidect));

#endif
#endif
    

    //
	PthreadsRenderer<WIDTH,HEIGHT> raytracer(lightModel,camera,samples_per_pixel,8);
    
	Vec3f *image = new Vec3f [WIDTH*HEIGHT];

    //KÖRS EJ
    //readRgbaToArray("tmp/exr_result_34.exr",image);
    //Preview
    //raytracer_preview.set_percentage_callback(percentage);
    raytracer_preview.set_pass_finished_callback(finished);
    raytracer_preview.compute(image);

    //Callbacks
    raytracer.set_percentage_callback(percentage);
    raytracer.set_pass_finished_callback(finished);
    raytracer.compute(image);
	//raytracer.resume_compute(image,0);

	delete [] image;
#if PHOTONMAP == 1
    delete global;
#endif
#if CAUSTICS == 1
    delete caustics;
#endif
	return 0;
}




