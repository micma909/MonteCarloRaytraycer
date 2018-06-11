#include <iostream>
#include "../Mat.h"

int main(int narg,char *args[]){
	/*Mat4x4f matrix = Matrix::mat(1.f,5.f,6.f, 8.f, 5.f,3.f, 1.f,3.f,7.f,8.f,5.f,6.f,1.f,2.f,3.f,4.f);
	matrix.print();
	Mat4x4f imatrix;
	Matrix::inverse(matrix, imatrix);
	imatrix.print();

	(matrix*imatrix).print();
	*/


	Mat3x3f matrix = Matrix::mat(0.0f,2.0f,0.0f,
								  0.0f,0.0f,2.0f,
								  2.0f,0.0f,0.0f);
	matrix.print();
	Mat3x3f inverse;
	Matrix::inverse(matrix,inverse);
	inverse.print();

	/*
	Mat4x4f imatrix;
	Matrix::inverse(matrix, imatrix);
	imatrix.print();

	(matrix*imatrix).print();
	*/
	return 0;
}




