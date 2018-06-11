#ifndef random_kit_H
#define random_kit_H

#include <random>
typedef struct random_kit{
    std::uniform_real_distribution<float> zero_to_two_pi;
    std::uniform_real_distribution<float> zero_to_one;
    std::mt19937 generator;
private:
    random_kit():zero_to_one(0.f,1.f),zero_to_two_pi(0.f,M_PI*2.f){
        std::random_device rd;
        generator = std::mt19937(rd());
    }
public:
    static random_kit create_kit(){
        return random_kit();
    }
} random_kit_t;
#endif