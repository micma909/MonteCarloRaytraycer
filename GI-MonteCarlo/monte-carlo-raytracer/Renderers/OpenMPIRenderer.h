#ifndef OpenMPIRenderer_H
#define OpenMPIRenderer_H
#include "Renderer.h"
#include "Cameras/Camera.h"
#include "Util/random_kit.h"
#include <iomanip>
#include <curses.h>

#ifdef __APPLE__
#include "mpi.h"
#elif defined _WIN32 || defined _WIN64
#include <C:/MinGW32/lib/gcc/mingw32/4.7.0/include/omp.h>
#endif
#include "mpi.h"
template<int W,int H>
class OpenMPIRenderer : public Renderer<W,H>{
    int samples_per_pixel;
public:
    OpenMPIRenderer(const std::shared_ptr<LightModel> &lightModel,
     const std::shared_ptr<Camera> &camera,
     int spp):Renderer<W,H>(lightModel, camera),samples_per_pixel(spp){
        //Initialize MPI
              
    } 
    ~OpenMPIRenderer(){
        //Finish MPI
        
    }
    

    void compute(Vec3<float> *image) const{
        
        //Rasterizer r(camera);
        //omp_set_num_threads(8);
        //float thread_progress[128];// Max number of threads;
        for(int i = 0; i < W*H;i++ ){
            image[i] = Vec3f(0.f,0.f,0.f);
        }

        int rank,size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        std::cout << "Hello this is rank " << rank << std::endl;
        //random number generator
        random_kit rnd = random_kit::create_kit();
        rnd.generator.seed(10000*rank);
        rnd.generator.discard(10000);
        std::uniform_real_distribution<float> jitter(-0.5,0.5);
        

        //Thread find its work area
        int rows_per_thread = H/size;
        int row_start = rank*rows_per_thread;
        int row_end = (rank+1) == size ? H : (rank+1)*rows_per_thread;
        int rows = row_end-row_start;

        //Image
        float *local_image = new float[H*W*3];
        
         
        if(rank == 0) std::cout << row_start << " " << row_end << std::endl;
        for(int pass = 0; pass < samples_per_pixel;pass++){
            for (int y = row_start; y < row_end; ++y){
            for (int x = 0; x < W; ++x){
                /*
                Ray ray = Renderer<W,H>::camera_->rasterSpace(x,y,W,H);
                local_image[x+W*y] = Renderer<W,H>::lightModel_->radiance(ray,0,rnd);
                */

                float radiance[WAVELENGTHS];
                for (int w = 0; w < WAVELENGTHS; ++w){
                    /* Get a radiance sample for this wavelength. */
                    float wavelength = 380.0f + RESOLUTION * w;
                    float xf = x+jitter(rnd.generator);
                    float yf = y+jitter(rnd.generator);
                    Ray ray = OpenMPIRenderer::camera_->rasterSpace(xf,yf,W,H);
                    radiance[w] = OpenMPIRenderer::lightModel_->radiance(ray,wavelength,rnd);
                    radiance[w] *= 1.f/float(samples_per_pixel);
                }
                
                image[x+W*y] += SpectrumToRGB(radiance, ColorSystems[ID_CIE]); //*(1.f/float(W*H*WAVELENGTHS));
                local_image[(x+W*y)*3+0] = image[x+W*y][0]*1.f/float(pass+1);
                local_image[(x+W*y)*3+1] = image[x+W*y][1]*1.f/float(pass+1);
                local_image[(x+W*y)*3+2] = image[x+W*y][2]*1.f/float(pass+1);

            }


        }
            if (rank == 0) {
                std::cout << "Thread id0 pass:" << pass << std::endl;
            }
   
        }

        
        if (rank == 0) {
            std::cout << "Thread id 0 is finished!" << std::endl;
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        //Stitch together
        int displs[size];
        int recvcnt[size];
        for (int i = 0; i < size; i++) {
            int rows_per_thread = H/size;
            int row_start = i*rows_per_thread;
            int row_end = (i+1) == size ? H : (i+1)*rows_per_thread;
            recvcnt[i] = (row_end-row_start);
            displs[i] = i*rows_per_thread;
        }
        if(rank == 0){
            for (int i = 0; i < size; i++) {
                std::cout << recvcnt[i] << " ";
            }
            std::cout << std::endl;
            for (int i = 0; i < size; i++) {
                std::cout << displs[i] << " ";
            }
       }
              


        float *result = NULL;
        if(rank == 0){
            result = new float[H*W*3];
        }  
        
        MPI_Gather(local_image+row_start*W*3,   //void *sendbuf
                   rows*W*3,                    //int sendcnt
                   MPI_FLOAT,                   //MPI_Datatype sendtype
                   result,                      //void *recvbuf
                   rows*W*3,                    //int recvcnt
                   MPI_FLOAT,                   //MPI_Datatype recvtype
                   0,                           //int root
                   MPI_COMM_WORLD);             //MPI_Comm comm
        
        //std::cout << "Thread " << rank << " will send from " <<  row_start*W*3 << std::endl;
        //std::cout << "Thread " << rank << " will send " <<  rows*W*3 << std::endl;

        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            for (int i = 0; i < W*H; i++) {
                image[i][0] = result[i*3];
                image[i][1] = result[i*3+1];
                image[i][2] = result[i*3+2];

            }
        }
        
        
        delete [] local_image;
        delete [] result;
    }
};

#endif