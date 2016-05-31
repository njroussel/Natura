#pragma once

/* Here are all the globals const used in the project. */

/* Time of a tick in seconds. */
#define TICK (1.f / 60.f)
/* Scale of the terrain. */
#define TERRAIN_SCALE 2.0f
/* Size of the terrain in chunks. */
#define TERRAIN_CHUNK_SIZE 10
/* Height of the water grid. */
#define WATER_HEIGHT -3.75f
/* Number of sub-tiles per chunk. */
#define CHUNK_SIDE_TILE_COUNT 4
/* Maximum time a ball can stay in a frozen state. */
#define BALL_MAX_FROZEN_TIME 30.f

glm::vec2 TERRAIN_OFFSET;
GLuint grass_program_id;
