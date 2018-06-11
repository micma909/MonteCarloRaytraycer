#ifndef SphereScene_H
#define SphereScene_H
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

#define CornellBoxScale (11.f)
class SphereScene : public AbstractSceneFactory{
public:
    
    
    static void place_objects(std::shared_ptr< StandardScene > scene){
        float radius = 1.5f;
        //float dist = radius/2.f+2*radius;
        
        //First row
        std::shared_ptr<Sphere> sphere0(new Sphere(radius));
        sphere0->multMatrix(Matrix::translate(-2.f, -4.f+radius, 2.4f));
        sphere0->setRefraction(std::shared_ptr<Refraction>(new Sellmeier(Sellmeier::diamond())));

        std::shared_ptr<Sphere> sphere1(new Sphere(radius));
        sphere1->multMatrix(Matrix::translate(-2.f, 0.f+radius*1.2f, -2.4f));
        sphere1->setMaterial(std::shared_ptr<Mirror>(new Mirror()));

        std::shared_ptr<Sphere> sphere2(new Sphere(radius));
        sphere2->multMatrix(Matrix::translate(-5.f+2*3, -5.f+radius, 2.f));
        
        std::shared_ptr<Sphere> sphere3(new Sphere(radius));
        sphere3->multMatrix(Matrix::translate(-5.f+2*4, -5.f+radius+2.f, -2.f));
        std::shared_ptr<Material> glow(new Glossy(3.f,6.f,0.1f));
        glow->set_reflectance([](const Material *m,const Vec3f &N, float w)->float{
            return 1.0f;
        });
		sphere3->setMaterial(glow);
        
        
        scene->addPrimitive(sphere0);
        scene->addPrimitive(sphere1);
        //scene->addPrimitive(sphere2);
        //scene->addPrimitive(sphere3);

    }
    
    static void place_walls(std::shared_ptr< StandardScene > scene){
        
        std::shared_ptr<FinitePlane> plane_left(new FinitePlane());
		plane_left->multMatrix(Matrix::zrotate(-M_PI/2));
		plane_left->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_left->multMatrix(Matrix::scale(CornellBoxScale));
        
		std::shared_ptr<FinitePlane> plane_right(new FinitePlane());
		plane_right->multMatrix(Matrix::zrotate(M_PI/2));
		plane_right->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_right->multMatrix(Matrix::scale(CornellBoxScale));
        
		std::shared_ptr<FinitePlane> plane_bottom(new FinitePlane());
		plane_bottom->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_bottom->multMatrix(Matrix::scale(CornellBoxScale));
        std::shared_ptr<Diffuse> b_plane(new Diffuse());
        b_plane->set_reflectance([](const Material &m, const Vec3f& x,float w)->float{
            
            if( (int)(floor(x[0]/2.0f)+floor(x[2]/2.0f)+1.0f) % 2 == 0){
                return 1.0f;
            }else{
                return 0.5f;
            }
            
            return 1.f;
            
        });
        plane_bottom->setMaterial(b_plane);
        
		std::shared_ptr<FinitePlane> plane_top(new FinitePlane());
		plane_top->multMatrix(Matrix::zrotate(M_PI));
		plane_top->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_top->multMatrix(Matrix::scale(CornellBoxScale));
        
		std::shared_ptr<FinitePlane> plane_front(new FinitePlane());
		plane_front->multMatrix(Matrix::xrotate(M_PI/2));
		plane_front->multMatrix(Matrix::translate(0.f,-5.f,0.f));
		plane_front->multMatrix(Matrix::scale(CornellBoxScale));
        
		std::shared_ptr<FinitePlane> plane_back(new FinitePlane());
		plane_back->multMatrix(Matrix::xrotate(-M_PI/2));
		plane_back->multMatrix(Matrix::translate(0.f,-5.f,-0.0f));
		plane_back->multMatrix(Matrix::scale(CornellBoxScale));
        
        scene->addPrimitive(plane_left);
		scene->addPrimitive(plane_right);
		scene->addPrimitive(plane_bottom);
		scene->addPrimitive(plane_top);
		scene->addPrimitive(plane_front);
        scene->addPrimitive(plane_back);
        
        std::shared_ptr<Diffuse> l_plane(new Diffuse());
        l_plane->set_reflectance([](const Material &m, const Vec3f& x,float w)->float{
            if(w >= 590-RESOLUTION-EPSILON && w <= 590+RESOLUTION+EPSILON){
                return 1.f;
            }else
                return 0.f;
        });
		plane_left->setMaterial(l_plane);
        
        std::shared_ptr<Diffuse> r_plane(new Diffuse());
        r_plane->set_reflectance([](const Material &m,const Vec3f& x,float w)->float{
            
            if(w >= 480-RESOLUTION-EPSILON && w <= 480+RESOLUTION+EPSILON)
                return 1.f;
            else
                return 0.f;
        });
        
		plane_right->setMaterial(r_plane);
        
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
        light->multMatrix(Matrix::scale(CornellBoxScale/3.f));
        std::shared_ptr<Diffuse> l_mat(new Diffuse(0.f));
        l_mat->set_emitt([](const Material &m,const Vec3f &I, const Vec3f &N, float wavelength)->float{
            //return 2*300.f;// <- med indirect
            return 60*300.f;// <- utan indirect vanliga fall
        });
        light->setMaterial(l_mat);
        scene->addLightSource(light);
        
        
        //Camera
        std::shared_ptr< Camera > camera(new Camera((float)WIDTH/(float)HEIGHT,51.2) );
        camera->multMatrix(Matrix::scale(0.2f,0.2f,1.f));
        camera->multMatrix(Matrix::translate(0.f,0.f,15.f));
        //camera->multMatrix(Matrix::xrotate(-0.05));
        
        scene->setCamera(camera);
        
        return scene;
	}
    
};
#endif
