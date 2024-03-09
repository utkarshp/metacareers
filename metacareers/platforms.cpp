//
//  platforms.cpp
//  platforms
//
//  Created by Utkarsh on 3/3/24.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <optional>
#include "platforms.h"

constexpr int tot_length = 1e6;
//constexpr int tot_length = 10;

std::vector<EndPoint> getEndpoints(std::vector<Platform>& platforms) {
    std::vector<EndPoint> endPoints;
    for(auto& platform : platforms) {
        endPoints.push_back(EndPoint(platform.x1, platform.index, true));
        endPoints.push_back(EndPoint(platform.x2, platform.index, false));
    }
    std::sort(endPoints.begin(), endPoints.end());
    return endPoints;
}

std::vector<double> getPlatformStartProbabilities(std::vector<Platform>& platforms) {
    std::sort(platforms.begin(), platforms.end(), std::greater<Platform>());
    for (int i=0; i<platforms.size(); i++) {
        platforms[i].index = i;
    }
    std::vector<EndPoint> endPoints = getEndpoints(platforms);
    std::vector<double> probabilities(platforms.size(), 0.0);
    std::set<Platform, std::greater<Platform>> occludedPlatforms;
    auto cur_ep = endPoints[0];
    auto cur_platform = std::make_optional(platforms[cur_ep.platform_index]);

    for (int i=1; i<endPoints.size(); i++) {
        auto new_ep = endPoints[i];
        if (cur_platform == std::nullopt) {
            cur_platform = std::make_optional(platforms[new_ep.platform_index]);
            cur_ep = new_ep;
            continue;
        }
        probabilities[cur_platform.value().index] += static_cast<double>(new_ep.x - cur_ep.x) / tot_length;
        // Starting a new platform, we can identify its left:
        // If higher, current is to the left of new.
        // If lower, the left of new is a platform that is highest among the even lower ones.
        if (new_ep.isStart) {
            if (platforms[new_ep.platform_index].y > cur_platform.value().y) {
                occludedPlatforms.insert(platforms[cur_platform.value().index]);
                platforms[cur_platform.value().index].open_x2 = new_ep.x;
                if (cur_platform.value().x2 != new_ep.x) {
                    platforms[new_ep.platform_index].left_index = cur_platform.value().index;
                } else {
                    auto top_it = occludedPlatforms.begin();
                    while (top_it != occludedPlatforms.end() && top_it->x2 == new_ep.x) ++ top_it;
                    if (top_it != occludedPlatforms.end()) {
                        platforms[new_ep.platform_index].left_index = top_it->index;
                    }
                }
                cur_platform = std::make_optional(platforms[new_ep.platform_index]);
            }
            else if (platforms[new_ep.platform_index].y < cur_platform.value().y) {
                auto it = occludedPlatforms.upper_bound(platforms[new_ep.platform_index]);
                while (it != occludedPlatforms.end() && it->x2 == new_ep.x) {
                    ++it;
                }
                if (it != occludedPlatforms.end()) {
                    platforms[new_ep.platform_index].left_index = it->index;
                }
                occludedPlatforms.insert(platforms[new_ep.platform_index]);
            }
        }
        // Ending a platform, we can identify its right.
        // If it is not a current platform the right is the next highest platform.
        else if (platforms[new_ep.platform_index].index != cur_platform.value().index) {
            auto it = occludedPlatforms.upper_bound(platforms[new_ep.platform_index]);
            while (it != occludedPlatforms.end() && it->x2 == new_ep.x) ++it;
            if (it != occludedPlatforms.end()) {
                platforms[new_ep.platform_index].right_index = it->index;
            }
            occludedPlatforms.erase(platforms[new_ep.platform_index]);
        } // If the current platform is ending, the next highest occluded becomes the current.
        // The new platform is also the right of the current platform.
        else if (!occludedPlatforms.empty()) {
            auto top_it = occludedPlatforms.begin();
            auto right_it = top_it;
            while (right_it != occludedPlatforms.end() && right_it->x2 == new_ep.x) ++right_it;
            if (right_it != occludedPlatforms.end()) {
                platforms[cur_platform.value().index].right_index = right_it->index;
                platforms[right_it->index].open_x1 = new_ep.x;
            }
            cur_platform = std::make_optional(*top_it);
            occludedPlatforms.erase(top_it);
        }
        else {
            cur_platform = std::nullopt;
        }
        cur_ep = new_ep;
    }

    return probabilities;
}

double getExpLength(const std::vector<Platform>& platforms, std::vector<double> probabilities,
                    Direction direction, int chosen_index) {
    double len = 0.0;
    std::cout << "Choosing " << chosen_index << " dir " << static_cast<int>(direction) << '\n';
    for (int i=0; i<platforms.size(); i++) {
        std::cout << "At " << i << " length is " << len << " " << probabilities[i] << '\n';
        if (i==chosen_index) {
            int idx = (direction == Direction::left) ? platforms[i].left_index : platforms[i].right_index;
            double mult = static_cast<double>(platforms[i].open_x2 - platforms[i].open_x1) / (2 * tot_length);
            double sum = platforms[i].open_x1 + platforms[i].open_x2;
            len += (direction == Direction::left) ? mult * (sum - 2 * platforms[i].x1) :
                  mult * (2 * platforms[i].x2 - sum);
            if (idx != -1) probabilities[idx] += probabilities[i];
        }
        else {
            len += platforms[i].getLength() * 0.5 * probabilities[i];
            auto indices = std::array<int, 2>{platforms[i].left_index, platforms[i].right_index};
            auto xpts = std::array<int, 2>{platforms[i].x1, platforms[i].x2};
            for (int j=0; j<2; j++) {
                auto idx = indices[j];
                auto x = xpts[j];
                if (idx == chosen_index) {
                    auto addLen = (direction == Direction::left) ? (x - platforms[chosen_index].x1) :
                                                            (platforms[chosen_index].x2 - x);
                    std::cout << "Adding len " << addLen * 0.5 * probabilities[i] << '\n';
                    len += addLen * 0.5 * probabilities[i];
                }
                else if (idx != -1)
                    probabilities[idx] += 0.5 * probabilities[i];
            }

        }
        std::cout << "Len updated to " << len << '\n';
    }

    return len;
}


double findOptimalExpLen(std::vector<Platform>& platforms) {
    double minLen = 1e6;
    auto probs = getPlatformStartProbabilities(platforms);
    for (const auto& platform: platforms) {
        double newLen1  = getExpLength(platforms, probs, Direction::left, platform.index);
        double newLen2  = getExpLength(platforms, probs, Direction::right, platform.index);
        minLen          = std::min({newLen1, newLen2, minLen});
        std::cout << "For platform " << platform.index << " got lengths " << newLen1 << ", " << newLen2 << '\n';
    }

    return minLen;
}
