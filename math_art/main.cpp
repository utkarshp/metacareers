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

void updateStacks(std::unordered_map<char,  std::stack<Segment>>& segmentsInDirection,
                  const Point& start,
                  const Point& end, char movementDir)
{
    auto& dirSegments = segmentsInDirection[movementDir];
    auto& oppDirSegments = segmentsInDirection[oppDirection(movementDir)];
    while(!dirSegments.empty() && dirSegments.top().onTheWay(start, end))
    {
        oppDirSegments.push(dirSegments.top());
        dirSegments.pop();
    }
}

void updateIntersections(PointToDirMap& pointToDirMap,
                         std::unordered_map<char, std::stack<Segment>>& segmentsInDirection,
                         const Point& start,
                         const Point& end, char movementDir)
{
    auto& dirSegments = segmentsInDirection[movementDir];
    auto& oppDirSegments = segmentsInDirection[oppDirection(movementDir)];

    while(!dirSegments.empty() && dirSegments.top().onTheWay(start, end)) {
        auto intersection = dirSegments.top().intersection(start, end);
        if (intersection != std::nullopt) {
            auto pt = intersection.value();
            auto pointIt = pointToDirMap.find(pt);
            if (pointIt == pointToDirMap.end()) {
                pointIt = pointToDirMap.insert({pt, {}}).first;
            }
            auto segment = dirSegments.top();
            const auto [dir1, dir2] = segment.getDirections(pt);
            if (dir1 != 'E') pointIt->second[dir1] = true;
            if (dir2 != 'E') pointIt->second[dir2] = true;
            if (pt != start) pointIt->second[oppDirection(movementDir)] = true;
            if (pt != end) pointIt->second[movementDir] = true;

        }
        oppDirSegments.push(dirSegments.top());
        dirSegments.pop();
    }
}

long long getPlusSignCountImpl(int N, long long L[], char D[]) {

    std::unordered_map<char, std::stack<Segment>> segmentsInDirection{
        {'L', std::stack<Segment>()}, {'R', std::stack<Segment>()},
        {'D', std::stack<Segment>()}, {'U', std::stack<Segment>()}
    };

    Point cur(0, 0);
    Point minlevelBegin = cur;
    Point maxLevelEnd = cur;
    PointToDirMap pointToDirMap;

    for (int i=0; i<N; i++) {

        auto nextPoint = cur.getPointInDirection(L[i], D[i]);
        auto levelSegment = Segment(minlevelBegin, maxLevelEnd);

        if (minlevelBegin != maxLevelEnd && levelSegment.isPerp(D[i]))
        {

            segmentsInDirection[D[i]].push(levelSegment);
            minlevelBegin = maxLevelEnd = cur;
        }
        updateIntersections(pointToDirMap, segmentsInDirection,
                            cur, nextPoint, D[i]);
        if (maxLevelEnd < nextPoint) maxLevelEnd = nextPoint;
        if (nextPoint < minlevelBegin) minlevelBegin = nextPoint;

        cur = nextPoint;
    }

    long long pointSum = 0;
    for (const auto& pointAndDirs: pointToDirMap) {
        auto dirs = pointAndDirs.second;
        /*std::cout << pointAndDirs.first.x << ' ' << pointAndDirs.first.y << ":\n";

        for (const auto& dirMap: pointAndDirs.second)
            std::cout << dirMap.first << ' ' << dirMap.second << ';';
        std::cout << std::endl;*/
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
    /*std::vector L = {5, 1, 2, 3, 7, 1, 1, 9, 1};
    std::string D = "LDUDDRLUL";*/
    std::vector L = {5, 1};
    std::string D = "LD";
    std::cout << getPlusSignCount(static_cast<int>(L.size()), L, D) << std::endl;
    return 0;
}
