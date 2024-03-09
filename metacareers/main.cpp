//
//  main.cpp
//  metacareers
//
//  Created by Utkarsh on 3/3/24.
//

#include <iostream>
#include "platforms.h"


int main(int argc, const char * argv[]) {
    // insert code here...
    std::vector<Platform> v = {
        Platform(5000, 7000, 2), Platform(2000, 8000, 8),
        Platform(7000, 11000, 5), Platform(9000, 11000, 9),
        Platform(0, 4000, 4)
    };
    /*std::vector<Platform> v = {
        Platform(5, 7, 2), Platform(2, 8, 8),
        Platform(7, 11, 5), Platform(9, 11, 9),
        Platform(0, 4, 4)
    };*/
    /*std::vector<Platform> v = {
        Platform(100'000, 800'000, 10), Platform(400'000, 800'000, 20),
        Platform(0, 200'000, 15)
    };*/

    auto probs = getPlatformStartProbabilities(v);
    for (int i=0; i < v.size(); i++) {
        std::cout << "Platform " << v[i].index << ": x1=" << v[i].x1 << ",x2=" << v[i].x2 << ",y=" << v[i].y << "; Left=" << v[i].left_index << ", Right=" << v[i].right_index << '\n';
    }

    std::cout << "Optimal length is " << findOptimalExpLen(v) << '\n';

    return 0;
}
