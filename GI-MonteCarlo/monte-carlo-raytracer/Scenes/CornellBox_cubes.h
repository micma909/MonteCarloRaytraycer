#ifndef CornellBox_cubes_H
#define CornellBox_cubes_H
#include "AbstractSceneFactory.h"

#include "Primitives/Sphere.h"
#include "Primitives/Triangle.h"
#include "Primitives/ObjLoad.h"
#include "Primitives/FinitePlane.h"
#include "Primitives/Ellipse.h"
#include "Primitives/ObjLoad.h"
#include "StandardScene.h"
#include "Util/Mat.h"
#include "Primitives/Glass.h"
#include "Primitives/Diamond.h"
#include "Primitives/Sellmeier.h"
#include "Primitives/Cauchy.h"
#include "Primitives/Box.h"
#include "Materials/Glossy.h"
#include "Materials/Mirror.h"

#include <memory>
#include <vector>

#define WIDE_SCALE (float(WIDTH)/float(HEIGHT))

#define CornellBoxScale (11.f)
class CornellBox_cubes : public AbstractSceneFactory{
public:
    static void place_objects(std::shared_ptr< StandardScene > scene){
        int nr_of_spheres = 4;
        float radius = 1.4f;
        float start = -(WIDE_SCALE*CornellBoxScale)/2.f;
        float increment = float(WIDE_SCALE*CornellBoxScale)/float(nr_of_spheres+1);
        for(int i = 0; i < nr_of_spheres; i++){
            std::shared_ptr<Sphere> sphere0(new Sphere(radius));
            sphere0->multMatrix(Matrix::translate(start + increment*(i+1), -5.f+radius, 2.0f));
            scene->addPrimitive(sphere0);
            
            try {
                std::vector<std::shared_ptr<Triangle> > triangles = ObjLoad::loadMesh("Meshes/CUBE.obj");
                for(std::shared_ptr<Triangle> triangle : triangles){
                    scene->addPrimitive(triangle);
                    triangle->multMatrix(Matrix::translate(start + increment*(i+1), -5.f, -1.f));
                    triangle->multMatrix(Matrix::scale(3.0f,7.3f,4.0f));
                    
                    if(i == 3){
                        
                    }else if( i == 2){
                        std::shared_ptr<Material> metal(new Glossy(3.f,6.f,0.1f));
                        metal->set_reflectance([](const Material *m,const Vec3f &N, float w)->float{
                            return 0.5f;
                        });
                        triangle->setMaterial(metal);
                        
                    }else if( i == 1){
                        triangle->setMaterial(std::shared_ptr<Mirror>(new Mirror()));
                    }else if( i == 0){
                        triangle->setRefraction(std::shared_ptr<Refraction>(new Cauchy(Cauchy::denseFlintGlassSF10())));
                        
                    }
                    
                }
            } catch (std::exception &ex) {
                std::cout << ex.what();
            }

            if(i == 3){
            
            }else if( i == 2){
                std::shared_ptr<Material> metal(new Glossy(3.f,6.f,0.1f));
                metal->set_reflectance([](const Material *m,const Vec3f &N, float w)->float{
                    return 0.5f;
                });
                sphere0->setMaterial(metal);

            }else if( i == 1){
                sphere0->setMaterial(std::shared_ptr<Mirror>(new Mirror()));
            }else if( i == 0){
                sphere0->setRefraction(std::shared_ptr<Refraction>(new Cauchy(Cauchy::denseFlintGlassSF10())));

            }
        }
    }
    
    static void place_walls(std::shared_ptr< StandardScene > scene){
        std::shared_ptr<FinitePlane> plane_left(new FinitePlane());
		plane_left->multMatrix(Matrix::zrotate(-M_PI/2));
		plane_left->multMatrix(Matrix::translate(0.f,-5.f*WIDE_SCALE,0.f));
		plane_left->multMatrix(Matrix::scale(CornellBoxScale));
        
		std::shared_ptr<FinitePlane> plane_right(new FinitePlane());
		plane_right->multMatrix(Matrix::zrotate(M_PI/2));
		plane_right->multMatrix(Matrix::translate(0.f,-5.f*WIDE_SCALE,0.f));
		plane_right->multMatrix(Matrix::scale(CornellBoxScale));
        
		std::shared_ptr<FinitePlane> plane_bottom(new FinitePlane());
		plane_bottom->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_bottom->multMatrix(Matrix::scale(CornellBoxScale)*WIDE_SCALE);

		std::shared_ptr<FinitePlane> plane_top(new FinitePlane());
		plane_top->multMatrix(Matrix::zrotate(M_PI));
		plane_top->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_top->multMatrix(Matrix::scale(CornellBoxScale)*WIDE_SCALE);
        
		std::shared_ptr<FinitePlane> plane_front(new FinitePlane());
		plane_front->multMatrix(Matrix::xrotate(M_PI/2));
		plane_front->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_front->multMatrix(Matrix::scale(CornellBoxScale)*WIDE_SCALE);
        
		std::shared_ptr<FinitePlane> plane_back(new FinitePlane());
		plane_back->multMatrix(Matrix::xrotate(-M_PI/2));
		plane_back->multMatrix(Matrix::translate(0.f,-4.5f,-0.0f));
		plane_back->multMatrix(Matrix::scale(CornellBoxScale)*WIDE_SCALE);
        
        scene->addPrimitive(plane_left);
		scene->addPrimitive(plane_right);
		scene->addPrimitive(plane_bottom);
		scene->addPrimitive(plane_top);
		scene->addPrimitive(plane_front);
        scene->addPrimitive(plane_back);
        
        std::shared_ptr<Diffuse> r_plane(new Diffuse());
        r_plane->set_reflectance([](const Material &m,const Vec3f& x,float w)->float{
            if(w >= 480-RESOLUTION-EPSILON && w <= 480+RESOLUTION+EPSILON)
                return 1.f;
            else
                return 0.f;
        });
        
		//plane_right->setMaterial(r_plane);
        
        std::shared_ptr<Diffuse> l_plane(new Diffuse());
        l_plane->set_reflectance([](const Material &m, const Vec3f& x,float w)->float{
            if(w >= 590-RESOLUTION-EPSILON && w <= 590+RESOLUTION+EPSILON){
                return 1.f;
            }else
                return 0.f;
        });
		//plane_left->setMaterial(l_plane);
        
        
        std::shared_ptr<Glossy> b_plane(new Glossy(2.f,6.f,0.1f));
        b_plane->set_reflectance([](const Material *m, const Vec3f& x,float w)->float{
            Vec3f local =  m->primitive_->itransform()*x;
            if( (int)(floor(local[0]*10.0f)+floor(local[2]*10.0f)) % 2 == 0){
                return 1.0f;
            }else{
                return 0.3f;
            }
            
            return 1.f;
        });
        /*
        plane_bottom->set_modify_normal([](Vec3f &N,const Primitive *p,const Vec3f& x)->void{
            Vec3f local =  p->itransform()*x;
            N = Matrix::zrotate(std::sin(M_PI*local[0]*10.f))*N.xyz0();
            
        });*/
        //plane_front->setMaterial(b_plane);
        //plane_left->setMaterial(r_plane);

        
        plane_left->setMaterial(l_plane);
        plane_right->setMaterial(r_plane);
        plane_bottom->setMaterial(b_plane);
        /*plane_top->setMaterial(b_plane);
        plane_front->setMaterial(b_plane);
        plane_back->setMaterial(b_plane);
        */
        
        //plane_bottom->setMaterial(std::shared_ptr<Material>(new Glossy(1.0,20.f,0.2f)));
        //plane_bottom->setMaterial(std::shared_ptr<Mirror>(new Mirror()));

    }
public:
	static CUDA_CALLABLE_MEMBER std::shared_ptr< StandardScene > scene(){
        //Light Model & Raytracer
        std::shared_ptr< StandardScene > scene(new StandardScene());
        
        //add walls
        place_walls(scene);
        //Place objects in scene
        place_objects(scene);
        
        //lightsource
        std::shared_ptr<FinitePlane> light(new FinitePlane());
        light->multMatrix(Matrix::xrotate(M_PI));
        light->multMatrix(Matrix::translate(0.f,-4.9f,0.f));
        light->multMatrix(Matrix::scale(WIDE_SCALE*(CornellBoxScale/1.f), CornellBoxScale/3.f,CornellBoxScale/3.f));
        std::shared_ptr<Diffuse> l_mat(new Diffuse(0.f));
        l_mat->set_emitt([](const Material &m,const Vec3f &I, const Vec3f &N, float wavelength)->float{
            //return 2*300.f;// <- med indirect
            return 60*300.f;// <- utan indirect vanliga fall
        });
        light->setMaterial(l_mat);
        scene->addLightSource(light);
        
        
        //Camera
        std::shared_ptr< Camera > camera(new Camera((float)WIDTH/(float)HEIGHT,27.0f) );
        camera->multMatrix(Matrix::scale(0.2f,0.2f,1.f));
        camera->multMatrix(Matrix::translate(0.f,0.f,15.f));
        //camera->multMatrix(Matrix::xrotate(-0.05));
        
        scene->setCamera(camera);
        
        return scene;
	}
    
};
#endif
