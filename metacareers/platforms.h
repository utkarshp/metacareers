//
//  platforms.h
//  metacareers
//
//  Created by Utkarsh on 3/5/24.
//

#ifndef platforms_h
#define platforms_h

class Platform {
public:
    int x1, x2, y;
    long double accumulated_left_open_exp_len_diff;
    long double accumulated_left_open_exp_len;
    int last_open;
    int index;
    int left_index;
    int right_index;

    Platform(int x1, int x2, int y) : x1(x1), x2(x2), y(y) {
        accumulated_left_open_exp_len_diff = 0.0;
        accumulated_left_open_exp_len = 0.0;
        left_index = -1;
        right_index = -1;
    }
    constexpr int getLength() const {
        return x2 - x1;
    }
    bool contains(int x) const {
        return x1 < x && x < x2;
    }
    bool operator<(const Platform& b) const {
        return (y==b.y) ? (x1 < b.x1) : (y < b.y);
    }
    bool operator>(const Platform& b) const {
        return b < *(this);
    }
};

class EndPoint {
public:
    int x;
    int platform_index;
    bool isStart;
    EndPoint() : x(-1), platform_index(-1), isStart(true) {}
    
    EndPoint(int x, int p, bool isStart) : platform_index(p), x(x), isStart(isStart) {}
    bool operator<(const EndPoint& other) const {
        return (x==other.x) ? (
                               (isStart != other.isStart) ? (!isStart) : (platform_index < other.platform_index)
                               ) : x < other.x;
    }
};

enum class Direction {left = -1, right = 1};
std::vector<EndPoint> getEndpoints(std::vector<Platform>& platforms);

std::vector<double> getPlatformStartProbabilities(std::vector<Platform>& platforms);
double findOptimalExpLen(std::vector<Platform>& platforms);
#endif /* platforms_h */
