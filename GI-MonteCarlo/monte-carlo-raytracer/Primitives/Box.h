#ifndef MonteCarlo_Box_h
#define MonteCarlo_Box_h


class Box {
public:
    static std::vector<std::shared_ptr<FinitePlane> > create_box(float height = 1.f,float width = 1.f){
        std::shared_ptr<FinitePlane> plane_left(new FinitePlane());
        plane_left->multMatrix(Matrix::translate(0.f,0.5f,0.f));
        plane_left->multMatrix(Matrix::zrotate(-M_PI/2));
        //plane_left->multMatrix(Matrix::zrotate(M_PI));
        //plane_left->multMatrix(Matrix::scale(height,1.f,width));
        
        std::shared_ptr<FinitePlane> plane_right(new FinitePlane());
        plane_right->multMatrix(Matrix::translate(0.f,0.5f,0.f));
        plane_right->multMatrix(Matrix::zrotate(M_PI/2));
        //plane_right->multMatrix(Matrix::zrotate(M_PI));
        
        std::shared_ptr<FinitePlane> plane_bottom(new FinitePlane());
        plane_bottom->multMatrix(Matrix::translate(0.f,0.5f,0.f));
        plane_bottom->multMatrix(Matrix::zrotate(-M_PI));
        
        std::shared_ptr<FinitePlane> plane_top(new FinitePlane());
        //plane_top->multMatrix(Matrix::zrotate(M_PI));
        plane_top->multMatrix(Matrix::translate(0.f,0.5f,0.f));
        //plane_top->multMatrix(Matrix::zrotate(M_PI));
        
        std::shared_ptr<FinitePlane> plane_front(new FinitePlane());
        plane_front->multMatrix(Matrix::translate(0.f,0.5f,0.f));
        plane_front->multMatrix(Matrix::xrotate(M_PI/2));
        //plane_front->multMatrix(Matrix::xrotate(M_PI));
        
        std::shared_ptr<FinitePlane> plane_back(new FinitePlane());
        plane_back->multMatrix(Matrix::translate(0.f,0.5f,0.f));
        plane_back->multMatrix(Matrix::xrotate(-M_PI/2));
        //plane_back->multMatrix(Matrix::xrotate(M_PI));
        
        std::vector<std::shared_ptr<FinitePlane> > collection;
        collection.push_back(plane_left);
        collection.push_back(plane_right);
        collection.push_back(plane_bottom);
        collection.push_back(plane_top);
        collection.push_back(plane_front);
        collection.push_back(plane_back);
        return collection;
    }
};


#endif
