#ifndef MonteCarlo_Photon_h
#define MonteCarlo_Photon_h

class Photon {
protected:
    Vec3f pos_;
    Vec3f dir_;
    float flux_;
    float wavelength_;
public:
    Photon(){}
    Photon(const Photon &p):pos_(p.pos()),dir_(p.dir()),flux_(p.flux()){}
    Photon(const Vec3f &pos,const Vec3f &dir,float flux):pos_(pos),dir_(dir),flux_(flux){}
    Vec3f pos() const{
        return pos_;
    }
    Vec3f& pos(){
        return pos_;
    }
    Vec3f dir() const{
        return dir_;
    }
    Vec3f& dir(){
        return dir_;
    }
    float flux() const{
        return flux_;
    }
    float& flux(){
        return flux_;
    }
    
    float wavelength() const{
        return wavelength_;
    }
    
    float& wavelength(){
        return wavelength_;
    }
};

#endif
