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
#include <cassert>
#include "platforms.h"

constexpr int tot_length = 1e6;
using std::ssize;
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

double getLeftExpLen(int open_x1, int open_x2, int total_x1, int total_x2, bool sub=true) {
    double mult = static_cast<double>(open_x2 - open_x1) / (2 * tot_length);
    double sum = open_x1 + open_x2;
    double retval;
    if (sub) retval = mult * (sum - 2 * total_x1) - mult * (total_x2 - total_x1);
    else retval = mult * (sum - 2 * total_x1);

    return retval;
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
    platforms[cur_ep.platform_index].last_open = platforms[cur_ep.platform_index].x1;

    for (int i=1; i<endPoints.size(); i++) {
        auto new_ep = endPoints[i];
        if (cur_platform == std::nullopt) {
            cur_platform = std::make_optional(platforms[new_ep.platform_index]);
            cur_ep = new_ep;
            platforms[cur_platform.value().index].last_open = platforms[cur_platform.value().index].x1;
            continue;
        }
        probabilities[cur_platform.value().index] += static_cast<double>(new_ep.x - cur_ep.x) / tot_length;
        int cur_platform_index = cur_platform.value().index;
        // Starting a new platform, we can identify its left:
        // If higher, current is to the left of new.
        // If lower, the left of new is a platform that is highest among the even lower ones.
        if (new_ep.isStart) {
            if (platforms[new_ep.platform_index].y > cur_platform.value().y) {
                occludedPlatforms.insert(platforms[cur_platform_index]);

                platforms[cur_platform_index].accumulated_left_open_exp_len_diff += getLeftExpLen(platforms[cur_platform_index].last_open, new_ep.x, platforms[cur_platform_index].x1, platforms[cur_platform_index].x2);
                platforms[cur_platform_index].accumulated_left_open_exp_len += getLeftExpLen(platforms[cur_platform_index].last_open, new_ep.x, platforms[cur_platform_index].x1, platforms[cur_platform_index].x2, false);
                assert(cur_platform.value().x2 != new_ep.x);
                platforms[new_ep.platform_index].left_index = cur_platform_index;
                cur_platform = std::make_optional(platforms[new_ep.platform_index]);
                cur_platform_index = cur_platform.value().index;
                platforms[cur_platform_index].last_open = new_ep.x;
            }
            else if (platforms[new_ep.platform_index].y < cur_platform.value().y) {
                auto it = occludedPlatforms.upper_bound(platforms[new_ep.platform_index]);
                if (it != occludedPlatforms.end()) {
                    platforms[new_ep.platform_index].left_index = it->index;
                }
                occludedPlatforms.insert(platforms[new_ep.platform_index]);
            }
        }
        // Ending a platform, we can identify its right.
        // If it is not a current platform the right is the next highest platform.
        else if (platforms[new_ep.platform_index].index != cur_platform_index) {
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
                platforms[cur_platform_index].right_index = right_it->index;
            }
            platforms[top_it->index].last_open = new_ep.x;
            platforms[cur_platform_index].accumulated_left_open_exp_len_diff += getLeftExpLen(platforms[cur_platform_index].last_open, new_ep.x, platforms[cur_platform_index].x1, platforms[cur_platform_index].x2);
            platforms[cur_platform_index].accumulated_left_open_exp_len += getLeftExpLen(platforms[cur_platform_index].last_open, new_ep.x, platforms[cur_platform_index].x1, platforms[cur_platform_index].x2, false);
            cur_platform = std::make_optional(*top_it);
            cur_platform_index = cur_platform.value().index;
            occludedPlatforms.erase(top_it);
        }
        else {
            platforms[cur_platform_index].accumulated_left_open_exp_len_diff += getLeftExpLen(platforms[cur_platform_index].last_open, new_ep.x, platforms[cur_platform_index].x1, platforms[cur_platform_index].x2);
            platforms[cur_platform_index].accumulated_left_open_exp_len += getLeftExpLen(platforms[cur_platform_index].last_open, new_ep.x, platforms[cur_platform_index].x1, platforms[cur_platform_index].x2, false);
            cur_platform = std::nullopt;
            cur_platform_index = -1;
        }
        cur_ep = new_ep;
    }

    return probabilities;
}


double getBestPlatformLenDiff(const std::vector<Platform>& platforms,
                                               const std::vector<double>& probabilities)
{
    std::vector<double> left_dir(platforms.size(), 0.0);
    std::vector<double> no_dir_expected(platforms.size(), 0.0);

    for (long i=ssize(platforms)-1; i>=0; i--) {
        double left_total = 0.0;
        double right_total = 0.0;
        if (platforms[i].left_index != -1) {
            const auto& left_plt = platforms[platforms[i].left_index];
            auto left_add = probabilities[i] * (platforms[i].x1 - left_plt.x1) * 0.25;
            auto right_add = probabilities[i] * (left_plt.x2 - platforms[i].x1) * 0.25;
            left_dir[left_plt.index] += (left_add - right_add);
            left_total = no_dir_expected[left_plt.index];
        }
        if (platforms[i].right_index != -1) {
            const auto& right_plt = platforms[platforms[i].right_index];
            auto left_add = probabilities[i] * (platforms[i].x2 - right_plt.x1) * 0.25;
            auto right_add = probabilities[i] * (right_plt.x2 - platforms[i].x2) * 0.25;
            left_dir[right_plt.index] += (left_add - right_add);
            right_total = no_dir_expected[right_plt.index];
        }

        left_dir[i] = 0.5 * probabilities[i] * (left_total - right_total) + platforms[i].accumulated_left_open_exp_len_diff;
        no_dir_expected[i] = (left_total + right_total + platforms[i].getLength()) * 0.5;

    }
    
    double min_add = 1e6 + 1.0;
    for (int i=0; i<platforms.size(); i++) {
        // std::cout << "Platform " << i << ": " << left_dir[i] << " " << no_dir_expected[i] << '\n';
        double cur_min = std::min(left_dir[i], -left_dir[i]);
        if (cur_min < min_add) {
            min_add = cur_min;
        }
    }
    
    return min_add;
}


void updatePlatformProbabilities(const std::vector<Platform>& platforms,
                                 std::vector<double>& probabilities) {
    for (int i=0; i<platforms.size(); i++) {
        auto indices = std::array<int, 2>{platforms[i].left_index, platforms[i].right_index};
        for (int j=0; j<2; j++) {
            auto idx = indices[j];
            if (idx != -1)
                probabilities[idx] += 0.5 * probabilities[i];
        }
    }
}


double getExpLength(const std::vector<Platform>& platforms, const std::vector<double>& probabilities) {
    double len = 0.0;
    for (int i=0; i<platforms.size(); i++) {
        //std::cout << "At " << i << " length is " << len << " " << probabilities[i] << '\n';
        len += platforms[i].getLength() * 0.5 * probabilities[i];
       // std::cout << "Len updated to " << len << '\n';
    }

    return len;
}


double findOptimalExpLen(std::vector<Platform>& platforms) {
    auto probs = getPlatformStartProbabilities(platforms);
    /* for (const auto& plt: platforms) {
        std::cout << "Platform " << plt.index << ": left=" << plt.left_index << ", right=" << plt.right_index << '\n';
    }*/
    updatePlatformProbabilities(platforms, probs);
    const auto best_platform_direction_len = getBestPlatformLenDiff(platforms, probs);
    double no_dir_exp = getExpLength(platforms, probs);

    return no_dir_exp + best_platform_direction_len;
}
