#ifndef CornellBox_H
#define CornellBox_H
#include "AbstractSceneFactory.h"

#include "Primitives/Sphere.h"
#include "Primitives/Triangle.h"
#include "Primitives/ObjLoad.h"
#include "Primitives/FinitePlane.h"
#include "Primitives/Plane.h"
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
class CornellBox : public AbstractSceneFactory{
public:
    
    
    static void place_objects(std::shared_ptr< StandardScene > scene){
        float radius = 2.1f;
        float dist = radius/2.f+radius;
        std::shared_ptr<Sphere> sphere0(new Sphere(radius));
  
        sphere0->multMatrix(Matrix::translate(2.f, 0.f, 2.f));
        sphere0->setRefraction(std::shared_ptr<Refraction>(new Sellmeier(Sellmeier::diamond())));
     
        std::shared_ptr<Sphere> sphere1(new Sphere(radius));
        sphere1->multMatrix(Matrix::translate(0.f, -2.f+radius, 0.f));

        sphere1->multMatrix(Matrix::scale(1.0f, 0.55f, 1.0f));
        sphere1->setRefraction(std::shared_ptr<Refraction>(new Cauchy(Cauchy::michoGlass())));

        std::shared_ptr<Sphere> sphere2(new Sphere(radius));
        sphere2->multMatrix(Matrix::translate(-2.8f+2*dist, -5.f+radius, -1.f));
        sphere2->setRefraction(std::shared_ptr<Refraction>(new Cauchy(Cauchy::bariumFlintGlassBaF10())));
        
        std::shared_ptr<Sphere> sphere3(new Sphere(radius));
        sphere3->multMatrix(Matrix::translate(-2.8f+3*dist, -5.f+radius, 3.f));
     
        
        //scene->addPrimitive(sphere0);
        //scene->addPrimitive(sphere1);
        //scene->addPrimitive(sphere2);
        //scene->addPrimitive(sphere3);
        //scene->addPrimitive(triangle);
        //scene->addPrimitive(glow_sphere);
        
        std::shared_ptr<Diffuse> gl_mat(new Diffuse(0.f));
        gl_mat->set_emitt([](const Material &m,const Vec3f &I, const Vec3f &N, float wavelength)->float{
            return 600*300.f;// <- utan indirect vanliga fall
        });
 
        //Cube
        
        try {
            std::vector<std::shared_ptr<Triangle> > triangles = ObjLoad::loadMesh("Meshes/prism.obj");
            for(std::shared_ptr<Triangle> triangle : triangles){
                scene->addPrimitive(triangle);
                triangle->setRefraction(std::shared_ptr<Refraction>(new Cauchy(Cauchy::michoGlass())));

                //Tranformations
                triangle->multMatrix(Matrix::scale(0.3f,0.3f,1.3f));
                //triangle->multMatrix(Matrix::yrotate(M_PI/3.f));//(2.2f, -3.0f, 1.f));
                triangle->multMatrix(Matrix::zrotate(M_PI/2.f));//(2.2f, -3.0f, 1.f));
                
                
                //Translation in world coordinates!!!!!!
                Mat4x4f t = triangle->transform();
                Mat4x4f it = triangle->itransform();
                triangle->multMatrix(it);
                triangle->multMatrix(Matrix::translate(-3.0f, 3.0f, 0.f));
                triangle->multMatrix(t);
                //-------------
                
            }
        } catch (std::exception &ex) {
            std::cout << ex.what();
        }
        
   
        std::shared_ptr<Plane> plane = std::shared_ptr<Plane>(new Plane());
        scene->addPrimitive(plane);
        
        //Tranformations
        //triangle->multMatrix(Matrix::yrotate(M_PI/3.f));//(2.2f, -3.0f, 1.f));
        plane->multMatrix(Matrix::zrotate(M_PI/2.f));//(2.2f, -3.0f, 1.f));
        
        
        //Translation in world coordinates!!!!!!
        Mat4x4f t = plane->transform();
        Mat4x4f it = plane->itransform();
        plane->multMatrix(it);
        plane->multMatrix(Matrix::translate(2.0f, 0.0f, 0.f));
        plane->multMatrix(t);
        //-------------
        
        
        


        /*
        try {
            std::vector<std::shared_ptr<Triangle> > triangles = ObjLoad::loadMesh("Meshes/prism.obj");
            for(std::shared_ptr<Triangle> triangle : triangles){
                scene->addPrimitive(triangle);
                
                triangle->multMatrix(Matrix::zrotate(M_PI/10.f));//(2.2f, -3.0f, 1.f));
                triangle->multMatrix(Matrix::translate(3.f, 2.f, 0.f));
                triangle->multMatrix(Matrix::scale(0.3f,0.3f,1.3f));

                
                triangle->setRefraction(std::shared_ptr<Refraction>(new Cauchy(Cauchy::michoGlass())));
            }
        } catch (std::exception &ex) {
            std::cout << ex.what();
        }*/
        
        //Tratt
        try {
            std::vector<std::shared_ptr<Triangle> > triangles = ObjLoad::loadMesh("Meshes/Tratt.obj");
            for(std::shared_ptr<Triangle> triangle : triangles){
                //scene->addPrimitive(triangle);
                triangle->multMatrix(Matrix::translate(3.f, -5.0f+4.f*EPSILON, 1.7f));
                //triangle->multMatrix(Matrix::yrotate(M_PI-M_PI/4.f));//(2.2f, -3.0f, 1.f));
                triangle->multMatrix(Matrix::scale(0.5f,0.5f,0.5f));
                //triangle->multMatrix(Matrix::scale(3.3f,5.3f,3.3f));
                
                //triangle->setRefraction(std::shared_ptr<Refraction>(new Sellmeier(Sellmeier::sapphire())));
                //triangle->setMaterial(std::shared_ptr<Mirror>(new Mirror()));
                //triangle->setMaterial(std::shared_ptr<Glossy>(new Glossy(3.f,6.f,0.1f)));
                
                //triangle->multMatrix(Matrix::translate(0.f, -0.3f, 0.f));
                triangle->setRefraction(std::shared_ptr<Refraction>(new Sellmeier(Sellmeier::sapphire())));
                //triangle->setRefraction(std::shared_ptr<Refraction>(new Glass()));

                //triangle->setNormal(-1.f*triangle->getNormal());
                
            }
        } catch (std::exception &ex) {
            std::cout << ex.what();
        }

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
		plane_back->multMatrix(Matrix::translate(0.f,-4.5f,-0.0f));
		plane_back->multMatrix(Matrix::scale(CornellBoxScale));
        
        scene->addPrimitive(plane_left);
		scene->addPrimitive(plane_right);
		scene->addPrimitive(plane_bottom);
		scene->addPrimitive(plane_top);
		scene->addPrimitive(plane_front);
        scene->addPrimitive(plane_back);
        
        std::shared_ptr<Diffuse> r_plane(new Diffuse());
        r_plane->set_reflectance([](const Material &m,const Vec3f& x,float w)->float{
            //return 1.f;
            if(w >= 480-RESOLUTION-EPSILON && w <= 480+RESOLUTION+EPSILON)
                return 1.f;
            else
                return 0.f;
        });
        
		plane_right->setMaterial(r_plane);
        
        std::shared_ptr<Diffuse> l_plane(new Diffuse());
        l_plane->set_reflectance([](const Material &m, const Vec3f& x,float w)->float{
            //return 1.f;
            if(w >= 590-RESOLUTION-EPSILON && w <= 590+RESOLUTION+EPSILON){
                return 1.f;
            }else
                return 0.f;
        });
		plane_left->setMaterial(l_plane);
        
        
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
        /*plane_bottom->setMaterial(b_plane);
        plane_top->setMaterial(b_plane);
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
        light->multMatrix(Matrix::scale(CornellBoxScale/50.f,1.f,CornellBoxScale));
        std::shared_ptr<Diffuse> l_mat(new Diffuse(0.f));
        l_mat->set_emitt([](const Material &m,const Vec3f &I, const Vec3f &N, float wavelength)->float{
            //return 2*300.f;// <- med indirect
            return 80*300.f;// <- utan indirect vanliga fall
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
