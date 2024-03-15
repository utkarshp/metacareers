//
//  math_art.cpp
//  metacareers
//
//  Created by Utkarsh on 3/13/24.
//

#include "math_art.hpp"

Point Point::getPointInDirection(long long dist, char d) const {
    switch (d) {
        case 'L': return Point(x-dist, y);
        case 'D': return Point(x, y-dist);
        case 'U': return Point(x, y+dist);
        case 'R': return Point(x+dist, y);
        default:
            assert(false);
    }
}

char Point::getDirection(const Point& other) const {
    if (x == other.x) {
        if (y < other.y) {
            return 'U';
        }
        else if (y == other.y) {
            return 'E';
        }
        else return 'D';
    }
    if (x < other.x) return 'R';
    return 'L';
}

bool Segment::belongs(const Point& p) const {
    if (vertical && p.x != p1.x) return false;
    if (!vertical && p.y != p1.y) return false;
    return (p1.x <= p.x && p.x <= p2.x) && (p1.y <= p.y && p.y <= p2.y);
}

std::optional<Point> Segment::intersection(const Point& begin, 
                                           const Point& end) const
{
    if (belongs(end)) {
        return std::make_optional(end);
    }
    if (belongs(begin)) {
        return std::make_optional(begin);
    }
    if (!onTheWay(begin, end)) return std::nullopt;
    if (!Segment(begin, end).onTheWay(p1, p2)) return std::nullopt;
    if (vertical) return std::make_optional(Point(p1.x, begin.y));
    return std::make_optional(Point(begin.x, p1.y));
}

bool Segment::isPerp(char d) const {
    if (vertical && (d=='L' || d == 'R')) return true;
    else if ((!vertical) && (d=='U' || d=='D')) return true;
    return false;
}


bool Segment::onTheWay(const Point &begin, const Point &end) const {
    if (!vertical) {
        return (end.y <= axisDistance && axisDistance <= begin.y) ||
        (end.y >= axisDistance && axisDistance >= begin.y);
    }
    else {
    return (end.x <= axisDistance && axisDistance <= begin.x) ||
        (end.x >= axisDistance && axisDistance >= begin.x);
    }
}

std::pair<char, char> Segment::getDirections(const Point& p) const {
    auto dir1 = p.getDirection(p1);
    auto dir2 = p.getDirection(p2);
    return {dir1, dir2};
}

