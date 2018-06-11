#ifndef PthreadsRenderer_H
#define PthreadsRenderer_H

#include "Renderer.h"
#include "Cameras/Camera.h"
#include "Spectrum/Spectrum.h"
#include "Spectrum/CIE.h"
#include <iomanip>
#include "Util/spectrum.h"
#include "Util/Timer.h"
#include "Util/pthread_barrier_mac.h"

#include <pthread.h>

/* This is the resolution of the color-matching curves, in nanometers per sample. */

/* The number of wavelengths used per pixel sample. */
#define WAVELENGTHS (1 + 400 / RESOLUTION)

#ifdef DEBUG
#   define pthread_err(...) pthread_err(__VA_ARGS__)
void pthread_err(int status){
    static int call = 0;
    call++;
    if (status) {
        const char *err_str = strerror(errno);
        printf("Call no:%d, pthread error no %d:\"%s\"\nterminating.",call,errno,err_str);
        exit(0);
    }
}
#else
#   define pthread_err(...) __VA_ARGS__
#endif

template<int W,int H> class PthreadsRenderer;

template<int W,int H>
struct ThreadParam {
    int thread_id;
    Vec3<float> *image,*norm_image;
    Timer t;
    int pass;
    int seed;
    const PthreadsRenderer<W, H> *renderer;
    pthread_t *save_thread;
    pthread_barrier_t *barrier;

    P_DYN_FNK(percentage_callback, void,int,float,float,float,float);
    P_DYN_FNK(pass_finished_callback, void,const PthreadsRenderer<W,H> *,const Vec3<float> *image,int);
};

template<int W,int H>
class PthreadsRenderer : public Renderer<W,H>{
private:
private:
    int no_threads;
    int samples_per_pixel_;
public:
    DYN_FNK(percentage_callback, void,int,float,float,float,float);
    DYN_FNK(pass_finished_callback, void,const PthreadsRenderer<W,H> *,const Vec3<float> *image,int);
public:
	PthreadsRenderer(const std::shared_ptr<LightModel> &lightModel, const std::shared_ptr<Camera> &camera,int samples_per_pixel,int noThreads):Renderer<W,H>(lightModel, camera),samples_per_pixel_(samples_per_pixel),no_threads(noThreads){
        if(no_threads == 1) std::cout << "Warning: PthreadsRenderer is running a single thread" << std::endl;
        _pass_finished_callback = NULL;
        _percentage_callback = NULL;
	}
    
    static void * pthread_compute_callback(void *p){
        ThreadParam<W,H> *param = (ThreadParam<W,H>*)p;
        const PthreadsRenderer<W,H> *_this = param->renderer;
        Vec3<float> *norm_image = param->norm_image;
        param->_pass_finished_callback(_this,norm_image,0);
        return 0;
    }

    static void * pthread_compute_continue(void *p){
        
        ThreadParam<W,H> *param = (ThreadParam<W,H>*)p;
        param->t.start();
        Vec3<float> *image = param->image;
        Vec3<float> *norm_image = param->norm_image;
        const PthreadsRenderer<W,H> *_this = param->renderer;
        const int samples_per_pixel = _this->samples_per_pixel_;

        //Initialize
        int tid = param->thread_id;
        int threads = _this->no_threads;
        
        //random number generator
        random_kit rnd = random_kit::create_kit();
        rnd.generator.seed(param->seed);
        rnd.generator.discard(10000);
        std::uniform_real_distribution<float> jitter(-0.5,0.5);

        //Thread find its work area
        int rows_per_thread = H/threads;
        int row_start = tid;
        //int row_end = (tid+1) == threads ? H : (tid+1)*rows_per_thread;
        for(int pass = param->pass; pass < samples_per_pixel+param->pass; pass++){
            int y = row_start;
            while (y < H){
                for (int x = 0; x < W; ++x){
                    float radiance[WAVELENGTHS] = {0.0f};
                    for (int w = 0; w < WAVELENGTHS; ++w){
                        /* Get a radiance sample for this wavelength. */
                        float wavelength = 380.0f + RESOLUTION * w;
                        float xf = x+jitter(rnd.generator);
                        float yf = y+jitter(rnd.generator);
                        Ray ray = _this->camera_->rasterSpace(xf,yf,W,H);
                        radiance[w] = _this->lightModel_->radiance(ray,wavelength,rnd);
                        radiance[w] *= 1.f/float(samples_per_pixel);
                    }
                    
                    image[x+W*y] += SpectrumToRGB(radiance, ColorSystems[ID_CIE]); //*(1.f/float(W*H*WAVELENGTHS));
                    norm_image[x+W*y] = image[x+W*y]*1.f/float(pass+1);
                    //image[x+W*y] = GammaCorrect(image[x+W*y], ColorSystems[5]);
                    if (tid == 0 && param->_percentage_callback != NULL) {
                        float curr_pass_percentage = float(y/threads+x/float(W))/float(rows_per_thread);
                        float percentage = float(pass)/float(samples_per_pixel) + curr_pass_percentage/float(samples_per_pixel);
                        param->_percentage_callback(pass,100.f*percentage,100.f*curr_pass_percentage,param->t.read(),param->t.read()*100.f/percentage-param->t.read());
                    }
                }
                y += threads;
            }
            pthread_barrier_wait(param->barrier);
            //Spawn an additional thread displaying the image!
            if(tid == 0 && (param->_pass_finished_callback != NULL)){
                param->_pass_finished_callback(param->renderer,param->norm_image,pass);
            }
        }

        param->t.stop();
        return 0;
    }

    static void * pthread_compute(void *p){
        
        ThreadParam<W,H> *param = (ThreadParam<W,H>*)p;
        param->t.start();
        Vec3<float> *image = param->image;
        Vec3<float> *norm_image = param->norm_image;
        const PthreadsRenderer<W,H> *_this = param->renderer;
        const int samples_per_pixel = _this->samples_per_pixel_;

        //Initialize
        int tid = param->thread_id;
        int threads = _this->no_threads;
        
        //random number generator
        random_kit rnd = random_kit::create_kit();
        rnd.generator.seed(10000*param->thread_id);
        rnd.generator.discard(10000);
        std::uniform_real_distribution<float> jitter(-0.5,0.5);

        //Thread find its work area
        int rows_per_thread = H/threads;
        int row_start = tid;
        //int row_end = (tid+1) == threads ? H : (tid+1)*rows_per_thread;
        for(int pass = 0; pass < samples_per_pixel; pass++){
            int y = row_start;
            while (y < H){
                for (int x = 0; x < W; ++x){
                    float radiance[WAVELENGTHS] = {0.0f};
                    for (int w = 0; w < WAVELENGTHS; ++w){
                        /* Get a radiance sample for this wavelength. */
                        float wavelength = 380.0f + RESOLUTION * w;
                        float xf = x+jitter(rnd.generator);
                        float yf = y+jitter(rnd.generator);
                        Ray ray = _this->camera_->rasterSpace(xf,yf,W,H);
                        radiance[w] = _this->lightModel_->radiance(ray,wavelength,rnd);
                        radiance[w] *= 1.f/float(samples_per_pixel);
                    }
                    
                    image[x+W*y] += SpectrumToRGB(radiance, ColorSystems[ID_CIE]); //*(1.f/float(W*H*WAVELENGTHS));
                    norm_image[x+W*y] = image[x+W*y]*1.f/float(pass+1);
                    //image[x+W*y] = GammaCorrect(image[x+W*y], ColorSystems[5]);
                    if (tid == 0 && param->_percentage_callback != NULL) {
                        float curr_pass_percentage = float(y/threads+x/float(W))/float(rows_per_thread);
                        float percentage = float(pass)/float(samples_per_pixel) + curr_pass_percentage/float(samples_per_pixel);
                        param->_percentage_callback(pass,100.f*percentage,100.f*curr_pass_percentage,param->t.read(),param->t.read()*100.f/percentage-param->t.read());
                    }
                }
                y += threads;
            }
            pthread_barrier_wait(param->barrier);
            //Spawn an additional thread displaying the image!
            if(tid == 0 && (param->_pass_finished_callback != NULL)){
                param->_pass_finished_callback(param->renderer,param->norm_image,pass);
            }
        }

        param->t.stop();
        return 0;
    }
    void resume_compute(Vec3<float> *image, int pass) const{
        Timer t;
        t.start();
        for(int i = 0 ; i < W*H; i++){
            image[i] *= pass;
        }


        Vec3f *norm_image = new Vec3f[W*H];

        //Initialize threads
        pthread_t threads[no_threads];
        pthread_attr_t pthread_custom_attr;
        pthread_err(pthread_attr_init(&pthread_custom_attr));       //Thread attributes
        pthread_barrier_t barrier;
        //pthread_barrierattr_t attribute;
        pthread_barrier_init(&barrier,NULL,no_threads);
        /* Start up threads */
        ThreadParam<W,H> *params = new ThreadParam<W,H>[no_threads];
        for (int i=0; i< no_threads; i++){
            ThreadParam<W,H> *p = params+i;
            p->thread_id = i;
            p->image = image;
            p->pass = pass;
            p->seed = (i+1)*10000*(int)Timer::getCurrentTime();
            p->norm_image = norm_image;
            p->renderer = this;
            p->barrier = &barrier;
            p->set_percentage_callback(this->_percentage_callback);
            p->set_pass_finished_callback(this->_pass_finished_callback);

                pthread_err(pthread_create(&threads[i], &pthread_custom_attr, PthreadsRenderer<W,H>::pthread_compute, (void *)(p)));

        }
        
        /* Synchronize the completion of each thread. */
        for (int i=0; i < no_threads; i++){
            pthread_err(pthread_join(threads[i],NULL));
        }
        t.stop();
        std::cout << "Total time: " << t << std::endl;
        delete [] params;
        delete [] norm_image;
        pthread_barrier_destroy(&barrier);
    }
	void compute(Vec3<float> *image) const{
       //Reset image
        for(int i = 0 ; i < W*H; i++){
            image[i] = Vec3f(0.f,0.f,0.f);
        }

        resume_compute(image, 0);
	}
    
};

#endif