//
//  math_art.hpp
//  metacareers
//
//  Created by Utkarsh on 3/13/24.
//

#ifndef math_art_hpp
#define math_art_hpp

#include <iostream>
#include <array>
#include <compare>
#include <cassert>
#include <optional>
#include <unordered_map>

class Point;
class Segment;
struct SegmentComparator;

class Point {
    long long x;
    long long y;

public:
    Point(long long x, long long y) : x(x), y(y) {

    }
    Point() {
    }
    Point(const Point& p) = default;

    auto operator==(const Point& p2) const {
        return (x==p2.x) && (y==p2.y);
    }
    auto operator<(const Point& p2) const {
        return (x==p2.x) ? (y<p2.y) : (x<p2.x);
    }
    auto operator!=(const Point& p2) const {
        return !((*this)==p2);
    }

    Point getPointInDirection(long long dist, char d) const;
    char getDirection(const Point& other) const;

    friend Segment;

    friend std::hash<Point>;

    void print() const {
        std::cout << x << ", " << y;
    }
};

template<>
struct std::hash<Point> {
    std::size_t operator()(const Point& p) const noexcept {
        auto h1 = std::hash<long long>{}(p.x);
        auto h2 = std::hash<long long>{}(p.y);

        return h1 + 0x9e3779b9 + (h2 << 6) + (h2 >> 2);
    }
};

class Segment {
    Point p1, p2;
    long long axisDistance;
public:
    bool vertical;
    Segment(const Point& p1, const Point& p2) : p1(p1), p2(p2)
    {
        vertical = (p1.x == p2.x);
        axisDistance = vertical ? p1.x : p1.y;
    }
    Segment(const Point& p1, const Point& p2, bool vertical) : p1(p1), p2(p2),
                    vertical(vertical)
    {
        axisDistance = vertical ? p1.x : p1.y;
    }
    Segment() = default;
    Segment(const Segment& ) = default;
    bool belongs(const Point& p) const;
    bool onTheWay(const Point& begin, const Point& end) const;
    bool isPerp(char d) const;
    std::optional<Point> intersection(const Point& begin, const Point& end) const;
    std::pair<char, char> getDirections(const Point& p) const;
    friend Point;
    friend SegmentComparator;
    void print() const {
        p1.print();
        std::cout << "<->";
        p2.print();
    }
};


struct SegmentComparator {
    bool operator()(const Segment& a, const Segment& b) const {
        return a.axisDistance < b.axisDistance;
    }
};

#endif /* math_art_hpp */
