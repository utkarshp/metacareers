//
//  main.cpp
//  metacareers
//
//  Created by Utkarsh on 3/3/24.
//

#include <iostream>
#include "platforms.h"

void hanoi(int n, char x, char y, char z) {
    if (n==1) {
        std::cout << "Move 0 from " <<  x << " to " << y << '\n';
        return;
    }

    hanoi(n-1, x, z, y);
    std::cout << "Move " << n-1 << " from " << x << " to " << y << '\n';
    hanoi(n-1, z, y, x);
}


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
    //hanoi(10, 'A', 'B', 'C');
    /*std::vector<Platform> v = {
        Platform(100'000, 600'000, 10), Platform(400'000, 800'000, 20)
//        Platform(0, 200'000, 15)
    };*/
    /*std::vector<Platform> v = {
        Platform(100'000, 900'000, 10), Platform(400'000, 800'000, 20),
        Platform(0, 300'000, 5), Platform(800'000, 1'000'000, 2)
    };*/

    std::cout << "Optimal length is " << findOptimalExpLen(v) << '\n';

    return 0;
}
