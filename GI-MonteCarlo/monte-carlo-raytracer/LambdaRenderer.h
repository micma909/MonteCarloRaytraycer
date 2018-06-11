#ifndef LambdaRenderer_H
#define LambdaRenderer_H
#include "Renderers/Renderer.h"
#include "Cameras/Camera.h"

/* 
   ----------------------READ ME----------------------------
   More or less written as a concept to see if this is a good direction
   to be heading in. 
   Im docummenting everything to be done so its clear as most of this
   is totally foreign to me.
   Goal: to implement a diffuse object. 
         knowing how this^ works we continue with MORE advanced classes/methods.
		 ( See http://people.csail.mit.edu/wojciech/BRDFValidation/ExperimentalValidation-talk.pdf
		 slide 19. )
   
   This seems a big step, so we need to discuss this in the morning.
   /Michal
   ---------------------------------------------------------
*/

// Below,these two somehow connected to the color matching curves,
// and since i have no clue how those work im leaving this as it is. 
#define SAMPLING_RESOLUTION 5
#define NR_OF_WAVELENGTHS (1+(400/SAMPLING_RESOLUTION)) 


template<int W,int H>
class LambdaRenderer : public Renderer<W,H>{
public:
	LambdaRenderer(const std::shared_ptr<LightModel> &lightModel, const std::shared_ptr<Camera> &camera):Renderer<W,H>(lightModel, camera){

	}
	void compute(Vec3<float> *image) const{
		//STEP 0: 
		// Generate random number seed for each thread -> Using openMP + "Mersenne Twister 19937 generator"
		//    -- http://en.wikipedia.org/wiki/Mersenne_twister
		//  Each OpenMp seems to thread get assigned a randomly generated value which 
		//  later is passed through the entire BRDF-pipeline. 
		//  Another ex: http://www.pgroup.com/lit/articles/insider/v2n2a4.htm
		// 
		//  This random number generator is described by many sites as one that has a very good
		//  distribution and good to use in MonteCarlo tracing... (as of yet, no clue on my side)..

		#pragma omp parallel for 
		for (int y = 0; y < H; ++y){
			for (int x = 0; x < W; ++x){
				
				int idx = x+W*y;

				// Loop over all wavelengths...
				// Wavelength in nm [380 - 780]
				
				/* STEP 1:
				 *  for(int l = 0; l < nroflambdas; l++){
				 * 	  tot_rad[l] = lightModel_->trace(ray, l, prng);
				 *  }
				 *
				 *  // tot_rad - array of all lambdas found for this pixel!
				 *  // tracemethod should therefore take three params: 
				 *  // - camera ray
				 *  // - wavelength
				 *  // - Mersenne_twister random number ( or some other generator )
				*/

				 // STEP 1 implementing..
				 float radiance[NR_OF_WAVELENGTHS] = {0.0f};

				 for(int l = 0; l < NR_OF_WAVELENGTHS; l++){
				 	float lambda = 380.0f + SAMPLING_RESOLUTION * l;
				 	//raiance[l] += Renderer<W,H>::lightModel_->radiance(ray, l);
				 }

				/* STEP 2: 
				 * 
				 * image[idx] = ConvertSpectrumToRGBVector(radiance_for_this_ray);
				*/

				 // STEP 2 implementing..
				 // 
				Ray ray = Renderer<W,H>::camera_->rasterSpace(x,y,W,H);
				image[idx] = Renderer<W,H>::lightModel_->trace(ray);
			}
		}
	}
};

#endif