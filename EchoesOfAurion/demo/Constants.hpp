#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// region 2: silverleaf forest
const int FOREST_WORLD_W_MAP = 3840;
const int FOREST_WORLD_H_MAP = 2160;

// region 2 minigame: the lost trail
const int FOREST_SEG_W = 1280;
const int FOREST_SEG_H = 1200;
const int FOREST_POINTS = 3;
const int FOREST_TOP_H = 720;

const int FOREST_WORLD_W = 1280;
const int FOREST_WORLD_H = FOREST_SEG_H * FOREST_POINTS + FOREST_TOP_H;

const int FOREST_SPAWN_X = 620;
const int FOREST_SPAWN_Y = 230;
const int FOREST_MAX_MISTAKES = 5;

const int CLEARING_OBJ_X = 470, CLEARING_OBJ_Y = 330;
const int CLEARING_OBJ_W = 210, CLEARING_OBJ_H = 160;

#endif