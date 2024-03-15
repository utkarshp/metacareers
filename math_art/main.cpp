//
//  main.cpp
//  math_art
//
//  Created by Utkarsh on 3/13/24.
//
#include "math_art.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include <set>
#include <unordered_set>

using DirMap = std::unordered_map<char, bool>;
using PointToDirMap = std::unordered_map<Point, DirMap>;

constexpr char oppDirection(char d) {
    switch (d) {
        case 'L': return 'R';
        case 'R': return 'L';
        case 'U': return 'D';
        case 'D': return 'U';
        default:
            assert(false);
    }
}

void updateIntersections(PointToDirMap& pointToDirMap,
                         std::set<Segment, SegmentComparator> segments,
                         const Point& start,
                         const Point& end)
{
    std::cout << "Updating intersection for ";
    start.print();
    std::cout << " to ";
    end.print();
    std::cout << '\n';
    auto movementDir = start.getDirection(end);

    Segment dummySegment(start, end);
    Segment startDummy(start, start, !dummySegment.vertical);
    Segment endDummy(end, end, !dummySegment.vertical);
    auto startIt = segments.lower_bound(startDummy);
    auto endIt = segments.upper_bound(endDummy);
    for (auto tempIt = startIt; tempIt != endIt; tempIt++) {
        auto intersection = tempIt->intersection(start, end);
        tempIt->print();
        std::cout << '\n';
        if (intersection != std::nullopt) {
            auto pt = intersection.value();
            auto pointIt = pointToDirMap.find(pt);
            if (pointIt == pointToDirMap.end()) {
                pointIt = pointToDirMap.insert({pt, {}}).first;
            }
            auto segment = *tempIt;
            const auto [dir1, dir2] = segment.getDirections(pt);
            if (dir1 != 'E') pointIt->second[dir1] = true;
            if (dir2 != 'E') pointIt->second[dir2] = true;
            if (pt != start) pointIt->second[oppDirection(movementDir)] = true;
            if (pt != end) pointIt->second[movementDir] = true;

        }
    }
}


long long getPlusSignCountImpl(int N, long long L[], char D[]) {

    /*std::unordered_map<char, std::stack<Segment>> segmentsInDirection{
        {'L', std::stack<Segment>()}, {'R', std::stack<Segment>()},
        {'D', std::stack<Segment>()}, {'U', std::stack<Segment>()}
    };*/
    std::set<Segment, SegmentComparator> horizSegments;
    std::set<Segment, SegmentComparator> vertSegments;

    Point cur(0, 0);
    Point minlevelBegin = cur;
    Point maxLevelEnd = cur;
    PointToDirMap pointToDirMap;

    for (int i=0; i<N; i++) {

        auto nextPoint = cur.getPointInDirection(L[i], D[i]);
        auto levelSegment = Segment(minlevelBegin, maxLevelEnd);

        if (minlevelBegin != maxLevelEnd && levelSegment.isPerp(D[i]))
        {
            if (levelSegment.vertical) vertSegments.insert(levelSegment);
            else horizSegments.insert(levelSegment);

            updateIntersections(pointToDirMap,
                                levelSegment.vertical ? horizSegments : vertSegments,
                                minlevelBegin, maxLevelEnd);
            minlevelBegin = maxLevelEnd = cur;
        }

        if (maxLevelEnd < nextPoint) maxLevelEnd = nextPoint;
        if (nextPoint < minlevelBegin) minlevelBegin = nextPoint;

        cur = nextPoint;
    }
    updateIntersections(pointToDirMap,
                        Segment(minlevelBegin, maxLevelEnd).vertical ? horizSegments : vertSegments,
                        minlevelBegin, maxLevelEnd);

    long long pointSum = 0;
    for (const auto& pointAndDirs: pointToDirMap) {
        auto dirs = pointAndDirs.second;
        pointAndDirs.first.print();
        std::cout << ":\n";

        for (const auto& dirMap: pointAndDirs.second)
            std::cout << dirMap.first << ' ' << dirMap.second << ';';
        std::cout << std::endl;
        if (dirs['L'] && dirs['D'] && dirs['R'] && dirs['U'])
            pointSum++;
    }
    return pointSum;
}

long long getPlusSignCount(int N, std::vector<int>& L, std::string_view D) {

    char curDir = D[0];
    char dir[N];
    long long dists[N];
    int wIndex=0;
    dir[0] = curDir;
    dists[0] = L[0];
    for (int i=1; i<N; i++) {
        if (D[i]==curDir) {
            dists[wIndex] += L[i];
        }
        else {
            dists[++wIndex] = L[i];
            curDir = dir[wIndex] = D[i];
        }
    }
    return getPlusSignCountImpl(wIndex+1, dists, dir);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::vector L = {5, 1, 2, 3, 7, 1, 1, 9, 1};
    std::string D = "LDUDDRLUL";
    std::cout << getPlusSignCount(static_cast<int>(L.size()), L, D) << std::endl;
    return 0;
}
