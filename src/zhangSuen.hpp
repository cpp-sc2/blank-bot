#pragma once

#include <sc2api/sc2_api.h>

#include <algorithm>
#include <iostream>

#include "map2d.hpp"
#include "constants.h"

//https://github.com/linbojin/Skeletonization-by-Zhang-Suen-Thinning-Algorithm
//https://www.researchgate.net/publication/308822048_A_new_thinning_algorithm_for_binary_images

//#define P1(x, y) imRef(map, x, y)
//#define P2(x, y) imRef(map, x, y + 1)
//#define P3(x, y) imRef(map, x + 1, y + 1)
//#define P4(x, y) imRef(map, x + 1, y)
//#define P5(x, y) imRef(map, x + 1, y - 1)
//#define P6(x, y) imRef(map, x, y - 1)
//#define P7(x, y) imRef(map, x - 1, y - 1)
//#define P8(x, y) imRef(map, x - 1, y)
//#define P9(x, y) imRef(map, x - 1, y + 1)

#define P1 imRef(map, i, j)
#define P2 imRef(map, i, j + 1)
#define P3 imRef(map, i + 1, j + 1)
#define P4 imRef(map, i + 1, j)
#define P5 imRef(map, i + 1, j - 1)
#define P6 imRef(map, i, j - 1)
#define P7 imRef(map, i - 1, j - 1)
#define P8 imRef(map, i - 1, j)
#define P9 imRef(map, i - 1, j + 1)

static int N(map2d<int8_t>* map, int i, int j){
    return P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
}

static int S(map2d<int8_t>* map, int i, int j) {
    int count = 0;
    if (!P2 && P3) {
        count++;
    }
    if (!P3 && P4) {
        count++;
    }
    if (!P4 && P5) {
        count++;
    }
    if (!P5 && P6) {
        count++;
    }
    if (!P6 && P7) {
        count++;
    }
    if (!P7 && P8) {
        count++;
    }
    if (!P8 && P9) {
        count++;
    }
    if (!P9 && P2) {
        count++;
    }
    return count;
}

void zhangSuenThinning(map2d<int8_t>* map, Agent *agent) {
    map2d<int8_t>* change = new map2d<int8_t>(map->width(), map->height(), true);
    #define BOX_BORDER 0.02
    while (1) {
        bool changedPixel = false;
        for (int i = 1; i < map->width() - 1; i++) {
            for (int j = 1; j < map->height() - 1; j++) {
                if (!P1) {
                    continue;
                }
                int n = N(map, i, j);
                int s = S(map, i, j);
                if (n < 2 || n > 6) {
                    //printf("1N(%d,%d) = %d\n", i, j, n);
                    continue;
                }
                if (s != 1) {
                    //printf("1S(%d,%d) = %d\n", i, j, s);
                    continue;
                }
                if (P2 && P4 && P6) {
                    continue;
                }
                if (P4 && P6 && P8) {
                    continue;
                }
                imRef(change, i, j) = 1;
                changedPixel = true;
            }
        }
        for (int i = 0; i < map->width(); i++) {
            for (int j = 0; j < map->height(); j++) {
                if (imRef(change, i, j)) {
                    imRef(map, i, j) = 0;
                    imRef(change, i, j) = 0;
                }
            }
        }

        for (int i = 1; i < map->width() - 1; i++) {
            for (int j = 1; j < map->height() - 1; j++) {
                if (!P1) {
                    continue;
                }
                int n = N(map, i, j);
                int s = S(map, i, j);
                if (n < 2 || n > 6) {
                    //printf("2N(%d,%d) = %d\n", i, j, n);
                    continue;
                }
                if (s != 1) {
                    //printf("2S(%d,%d) = %d\n", i, j, s);
                    continue;
                }
                if (P2 && P4 && P8) {
                    continue;
                }
                if (P2 && P6 && P8) {
                    continue;
                }
                imRef(change, i, j) = 1;
                changedPixel = true;
            }
        }
        for (int i = 0; i < map->width(); i++) {
            for (int j = 0; j < map->height(); j++) {
                if (imRef(change, i, j)) {
                    imRef(map, i, j) = 0;
                    imRef(change, i, j) = 0;
                }
                if (imRef(map, i, j)) {
                    agent->Debug()->DebugBoxOut(
                        Point3D(i + BOX_BORDER, j + BOX_BORDER, agent->Observation()->TerrainHeight(Point2D{i + 0.5F, j + 0.5F}) + 0.01),
                        Point3D(i + 1 - BOX_BORDER, j + 1 - BOX_BORDER, agent->Observation()->TerrainHeight(Point2D{i + 0.5F, j + 0.5F})), {250, 200, 210});
                }
            }
        }
        agent->Debug()->SendDebug();

        if (!changedPixel) {
            break;
        }
    }
    
}