#ifndef Object_H
#define Object_H
#include "Mat.h"

class Object{
public:
	CUDA_CALLABLE_MEMBER void setMatrix(const Mat4x4f &m){
		setTransform(m);
		Matrix::inverse(transform_,itransform_);
	}
	CUDA_CALLABLE_MEMBER void multMatrix(const Mat4x4f &m){
		setTransform(transform_*m);	
		Matrix::inverse(transform_,itransform_);
	}
    
	CUDA_CALLABLE_MEMBER const Mat4x4f& transform() const{
		return transform_;
	}
	CUDA_CALLABLE_MEMBER const Mat4x4f& itransform() const{
		return itransform_;
	}
	/*
	CUDA_CALLABLE_MEMBER void rotateInPlace(const Mat4x4f &rot_mat) const{
		Mat4x4f irot_mat,C;
		Matrix::inverse(rot_mat,irot_mat);	
			
		//Flytta till origo
		C = Mat4x4f::identity();
		//Rotation
		C = C*rot_mat;
		//Tranformera
		C = C*transform_;
		//Invers rotera
		C= C*irot_mat;
	}*/

protected:
	CUDA_CALLABLE_MEMBER virtual void setTransform(const Mat4x4f &m){
		transform_ = m;
	} 
	CUDA_CALLABLE_MEMBER Object(){
        setMatrix(Mat4x4f::identity());
    }

private:
	Mat4x4f transform_;
	Mat4x4f itransform_;
};
#endif