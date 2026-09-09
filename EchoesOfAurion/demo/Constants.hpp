#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Region 2: Silverleaf Forest
const int FOREST_WORLD_W_MAP = 3840;
const int FOREST_WORLD_H_MAP = 2160;

// Region 2 minigame: The Lost Trail
const int FOREST_SEG_W = 1280;
const int FOREST_SEG_H = 1200;
const int FOREST_POINTS = 3;
const int FOREST_TOP_H = 700;

const int FOREST_WORLD_W = 1280;
const int FOREST_WORLD_H = FOREST_SEG_H * FOREST_POINTS + FOREST_TOP_H;

const int FOREST_SPAWN_X = 620;
const int FOREST_SPAWN_Y = 120;
const int FOREST_MAX_MISTAKES = 5;

#endif