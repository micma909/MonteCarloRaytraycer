#ifndef ObjLoad_H
#define ObjLoad_H

#include "Triangle.h"
#include "Util/Vec3.h"
#include "Util/Mat.h"
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>

class ObjLoad{
public:
	static std::vector<std::shared_ptr<Triangle> > loadMesh(std::string filename){
        std::ifstream file;
        std::string line;
        file.open(filename.c_str());
        std::string mtl = "mtl";
        double x, y, z;
        //double u, v;
        int v1,v2,v3, vt1,vt2,vt3, n1,n2,n3;
        std::vector<Vec3f> verts;
        std::vector<Vec3f> normals;
        std::vector<Vec3<int> >  face_ind;
        std::vector<Vec3<int> >  norm_ind;
        
        if(file.is_open() == true){
            while(getline(file,line) ){
                if(line.substr(0,1) == "#" || std::string::npos != line.find(mtl)){
                    continue;
                }else if(line.substr(0,2) == "v "){
                    std::istringstream number(line.substr(2));
                    number >> x >> y >> z;
                    verts.push_back(Vec3f(x,y,z));
                }else if(line.substr(0,2) == "vn"){
                    std::istringstream number(line.substr(2));
                    number >> x >> y >> z;
                    Vec3f normal(x,y,z);
                    normals.push_back(Vec3f(x,y,z));
                }else if(line.substr(0,2) == "f "){
                    line = line.substr(2, line.length());
                    replace(line.begin(), line.end(), '/', ' ');
                    std::istringstream number(line);
                    number >> v1 >> vt1 >> n1 >> v2 >> vt2 >> n2 >> v3 >> vt3 >> n3;
                    face_ind.push_back(Vec3<int>(v1-1,v2-1,v3-1));
                    norm_ind.push_back(Vec3<int>(n1-1,n2-1,n3-1));
                }else if(line.substr(0,2) == "vt"){
                    // TEXTURES!
                }
            }
        }
        std::vector<std::shared_ptr<Triangle> > triangles;
        for(int i = 0; i < face_ind.size(); i++){
            //get index to triangle vertecies
            int v1_i = (int)face_ind[i][0];
            int v2_i = (int)face_ind[i][1];
            int v3_i = (int)face_ind[i][2];
            //get one vertex-normal (for now I assume vn1=vn2=vn3 ...)
            int no_i = (int)norm_ind[i][0];
            std::shared_ptr<Triangle> triangle(new Triangle(verts[v1_i], verts[v2_i], verts[v3_i], normals[no_i]));
            //triangles.push_back(new Triangle(, *normals[no_i], mat));
            triangles.push_back(triangle);
        }
        
        return triangles;
	}
};

#endif
