#include <stdio.h>
#include <iostream>
#include <time.h>

#include "Definitions.h"
#include "Scenes/CornellBox.h"
#include "Util/Mat.h"
#include "Cameras/Camera.h"
#include "Renderers/Renderer.h"
#include "LightModels/MonteCarlo.h"
#include "LightModels/LightModel.h"
#include "LightModels/HitTest.h"
#include "LightModels/PhongShading.h"
#include "Renderers/StandardRenderer.h"
#include "Renderers/OpenMPIRenderer.h"
#include "Util/savebmp.h"
#include "Primitives/Sphere.h"
#include "Scenes/StandardScene.h"


#include "Materials/Material.h"
#include <fstream> 

#include <vector>
#include <memory>
#include <sstream>
#include <random>
#ifdef __APPLE__
    #include <omp.h>
    #include "mpi.h"
#elif defined _WIN32 || defined _WIN64
    #include <C:/MinGW32/lib/gcc/mingw32/4.7.0/include/omp.h>
#endif  

/*
struct MT19937 {
private:
    static std::mt19937_64 rng;
public:
    // This is equivalent to srand().
    static void seed(uint64_t new_seed = 0x530FD819d) {
        rng.seed(new_seed);
    }

    // This is equivalent to rand().
    static uint64_t get() {
        return rng();
    }
};

/* // in main do:
    MT19937::seed();

    for (int i = 0; i < 10; ++ i)
        std::cout << MT19937::get() << std::endl;
*/


//std::mt19937_64 MT19937::rng;

void saveAsBinary(Vec3f *image,int W,int H){
    std::stringstream ss;
    ss << "tmp/result_" << W << "_" << H << ".bin";
    std::ofstream myFile (ss.str().c_str(), std::ios::out | std::ios::binary);
    myFile.write ((char*)image, sizeof(Vec3f)*W*H);
}

int main(int narg,char *args[]){

    MPI_Init(&narg, &args);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int direct = 1;
    int indidect = 1;
    int spp = 30;
    std::shared_ptr<Scene> scene = CornellBox::scene();
    std::shared_ptr<Camera> camera = scene->getCamera();

    
    std::shared_ptr<LightModel> lightModel (new MonteCarlo(scene,direct,indidect));
    OpenMPIRenderer<WIDTH,HEIGHT> raytracer(lightModel,camera,spp);
    Vec3f *image = new Vec3f [WIDTH*HEIGHT];

    raytracer.compute(image);

    int dpi = 72;
    int width =  WIDTH;
    int height =  HEIGHT;
    if(rank == 0){
        //save as binary
        saveAsBinary(image,WIDTH,HEIGHT);
    } 
    if(rank == 0) savebmp("result.bmp",WIDTH,HEIGHT, 72, image);

    delete [] image;
    
    MPI_Finalize();

    return 0;
}




