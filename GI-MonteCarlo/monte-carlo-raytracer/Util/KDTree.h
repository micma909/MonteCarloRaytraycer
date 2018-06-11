#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#define SQ(x) ((x) * (x))

#ifndef KDTree_h
#define KDTree_h

#include "Vec.h"
#include "Photon Map/Photon.h"
#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <queue>

template <class T,int K,class T2 = Vec<T, K> > class KDTree;

template<class T,int K,class T2>
class KDNode{
private:
    
    KDNode(const Vec<T, K> m,KDNode *l,KDNode *r,const T2 &data):median_(m),left_(l),right_(r),data_(data){
        
    }
    
    ~KDNode(){
        if(left_ != NULL) delete left_;
        if(right_ != NULL) delete right_;
    }
    
    Vec<T, K> median_;
    KDNode *left_;
    KDNode *right_;
    T2 data_;
    friend class KDTree<T,K,T2>;
};


template <class T,int K,class T2>
class KDTree {
private:
    KDNode<T, K,T2> *root_;
protected:

    KDNode<T, K,T2>* init(std::vector< T2 > &points,Vec<T,K> (*coordForData)(const T2 &data),int d) const{
        if (points.size() == 0) {
            return NULL;
        }else if(points.size() == 1){
            return new KDNode<T,K,T2>(coordForData(*points.begin()),NULL,NULL,*points.begin());
        }else{
            //Sort according to current axis
            int axis = d % K;
            std::stable_sort(points.begin(), points.end(), [&](const T2 &a,const T2 &b)->bool{
                return coordForData(a)[axis] < coordForData(b)[axis];
            });
            
            //Remove median
            unsigned long median = points.size()/2;
            Vec<T,K> median_point = coordForData(points[median]);
            T2 data(points[median]);
            points.erase(points.begin()+median);
            
            //Left vector
            std::vector< T2 > left(median);
            std::move(points.begin(),points.begin()+median,left.begin());
            
            //Right vector
            std::vector< T2 > right(points.size()-median);
            std::move(points.begin()+median,points.end(),right.begin());
            
            return new KDNode<T,K,T2>(median_point,init(left,coordForData, d+1),init(right,coordForData, d+1),data);
        }
    }
    
    /**
        Returns square distance
     */
    void search(T &curr_min,Vec<T,K> **currBestNode,const Vec<T, K> &point,KDNode<T,K,T2> *node,int depth) const{
        if(node == NULL) return;

        #define LEFT 0
        #define RIGHT 1
        
        //Flip axis
        int axis = depth % K;
        int dir = -1;
        if (point[axis] < node->median_[axis]) {
            search(curr_min,currBestNode,point,node->left_,depth+1);
            dir = LEFT;
        }else{
            search(curr_min,currBestNode,point,node->right_,depth+1);
            dir = RIGHT;
        }
        //Unroll recursion

        //Check distance to point
        Vec3f d = std::move(point-node->median_);
        T distance = d.dot(d);
        //If the current distance is smallest so far, save it.
        if (distance < curr_min) {
            curr_min = distance;
            *currBestNode = &node->median_;
        }

        //Distance to splitting plane
        //If distance to splitting plane is smaller then current min, we might have a point closer on the other side
        if (SQ(point[axis]-node->median_[axis]) < curr_min) {
            if (dir == LEFT) {
                search(curr_min,currBestNode,point,node->right_,depth+1);
            }else{
                search(curr_min,currBestNode,point,node->left_,depth+1);
            }
        }else{
            //Otherwise just continue...
        }
        
    }
    
    void search_n(const Vec<T, K> &point,int N,T max_dist,std::multimap<T,T2> &s,KDNode<T,K,T2> *node,int depth) const{
        if (node == NULL) return;
        
        #define LEFT 0
        #define RIGHT 1
        
        //Flip axis
        int axis = depth % K;
        int dir = -1;
        if (point[axis] < node->median_[axis]) {
            search_n(point,N,max_dist,s,node->left_,depth+1);
            dir = LEFT;
        }else{
            search_n(point,N,max_dist,s,node->right_,depth+1);
            dir = RIGHT;
        }
        
        Vec3f d = std::move(point-node->median_);
        T distance = d.dot(d);
        
        //curr_min
        T max_of_smallest = s.size() > 0 ? s.rbegin()->first : INFINITY;
        
        //Unroll recursion
        if ((distance < max_of_smallest || s.size() < N) && distance < max_dist) {
            if(s.size() >= N) s.erase(--s.end());
            s.insert(std::pair<T,T2 >(distance, node->data_ ));
            max_of_smallest = (--s.end())->first;
        }

        //Distance to splitting plane
        //If distance to splitting plane is smaller then current min, we might have a point closer on the other side
        if ((s.size() < N || SQ(point[axis]-node->median_[axis]) < max_of_smallest) && SQ(point[axis]-node->median_[axis]) < max_dist) {
            if (dir == LEFT) {
                search_n(point,N,max_dist,s,node->right_,depth+1);
            }else{
                search_n(point,N,max_dist,s,node->left_,depth+1);
            }
        }else{
            //Just continue...
        }
        
    }
    
    bool isInRange(const T min[K],const T max[K],const Vec<T,K> &v) const{
        for(int i = 0; i < K; i++){
            if(min[i] > v[i] || max[i] < v[i]){
                return false;
            }
        }
        return true;
    }
    
    
    void range_search(std::vector<T2> &points,
                      const Vec<T, K> &point,
                      const T radius,
                      KDNode<T,K,T2> *node,
                      int depth) const{
        if(node == NULL) return;
        
        #define LEFT 0
        #define RIGHT 1

        //Flip axis
        int axis = depth % K;
        int dir = -1;
        if (point[axis] < node->median_[axis]) {
            range_search(points,point,radius,node->left_,depth+1);
            dir = LEFT;
        }else{
            range_search(points,point,radius,node->right_,depth+1);
            dir = RIGHT;
        }
        //Unroll recursion
        
        //Check distance to point

        //Distance to splitting plane
        //If distance to splitting plane is smaller then current min, we might have a point closer on the other side
        if (SQ(point[axis]-node->median_[axis]) < radius) {
            points.push_back(node->data_);
            if (dir == LEFT) {
                range_search(points,point,radius,node->right_,depth+1);
            }else{
                range_search(points,point,radius,node->left_,depth+1);
            }
        }else{
            //Otherwise just continue...
        }
    }
    
    void allPoints(std::vector< Vec<T,K> > &points,KDNode<T,K,T2> *node) const{
        if (node == NULL) return;
        points.push_back(node->median_);
        allPoints(points, node->left_);
        allPoints(points, node->right_);
    }
    int computeDepth(KDNode<T,K,T2> *n) const{
        if(n == NULL) return 0;
        return std::max(computeDepth(n->left_),computeDepth(n->right_))+1;
    }
    void printLevelorder(KDNode<T,K,T2> *n,int indent) const{
        int nodeSize = 20;
        int maxDepth = computeDepth(n);
        int currentLevel = 0;
        
        std::queue< std::pair<KDNode<T,K,T2>*, int> > q;
        q.push(std::pair<KDNode<T,K,T2>*, int> (n,0));
        while (!q.empty() && currentLevel != maxDepth) {
            std::pair<KDNode<T,K,T2>*, int> pair = q.front();
            q.pop();
            
            //New level!
            if (currentLevel != pair.second) {
                std::cout << std::endl;
                currentLevel = pair.second;
            }
            
            if (currentLevel != maxDepth) {
                for (int i = 0; i < (pow(2,maxDepth-pair.second-2) - 0.5)*nodeSize; i++) {
                    std::cout << " ";
                }
                
                
                if (pair.first == NULL) {
                    for (int i = 0; i < nodeSize; i++) std::cout << " ";
                }else{
                    
                    std::cout << (pair.first->visited_  ? GREEN : RESET) << pair.first->median_;
                }
                
                for (int i = 0; i < (pow(2,maxDepth-pair.second-2) - 0.5)*nodeSize; i++) {
                    std::cout << " ";
                }
                
                
                
                if (pair.first != NULL) {
                    q.push(std::pair<KDNode<T,K,T2>*, int> (pair.first->left_,pair.second+1));
                    q.push(std::pair<KDNode<T,K,T2>*, int> (pair.first->right_,pair.second+1));
                }else{
                    q.push(std::pair<KDNode<T,K,T2>*, int> (NULL,pair.second+1));
                    q.push(std::pair<KDNode<T,K,T2>*, int> (NULL,pair.second+1));
                }
            }
            
        }

     

    }
    
public:
    KDTree(std::vector< T2 > points,Vec<T,K> (*coordForData)(const T2 &data)):root_(NULL){
        if(points.size() != 0){
            root_ = init(points,coordForData, 0);
        }
    }
    
    /**
        Search for point closest to v
    */
    void search(const Vec<T, K> &point,T &min,Vec<T,K> **v) const{
        
        min = T(INFINITY);
        search(min,v,point, root_, 0);
        min = std::sqrt(min);
    }
    /**
     Search for N point closest to v
     */
    std::multimap<T,T2 > search_n(const Vec<T, K> &point,int N) const{
        std::multimap<T,T2 > s;
        search_n(point,N,INFINITY,s, root_, 0);
        return s;
    }
    
    /**
     Search for N point closest to v with Maximum distance d
     */
    std::multimap<T,T2 > search_n(const Vec<T, K> &point,int N,float max_dist) const{
        std::multimap<T,T2 > s;
        search_n(point,N,max_dist*max_dist,s, root_, 0);
        return s;
    }
    
    
    /**
     Search for all points within range
     */
    std::vector<T2 > range_search(const Vec<T, K> &point,const T radius) const{
        std::vector<T2> points;
        range_search(points,point,SQ(radius),root_, 0);
        return points;
    }
    
    void printLevelorder() const{
        printLevelorder(root_,8);
        std::cout << RESET;
    }
    
    //All points
    void allPoints(std::vector< Vec<T,K> > &points) const{
        if (root_ != NULL) {
            allPoints(points, root_);
        }
    }
    
    ~KDTree(){
        if(root_ != NULL) delete root_;
    }
    
};

#endif









