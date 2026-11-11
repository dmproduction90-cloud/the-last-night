#ifndef WORLD_H
#define WORLD_H

#define WORLD_MIN_X 0.5f
#define WORLD_MAX_X 19.5f
#define WORLD_MIN_Z 0.5f
#define WORLD_MAX_Z 15.5f

void world_init(void);
void world_draw(void);
/* Repousse (x,z) hors des obstacles. */
void world_collide(float *x, float *z, float radius);

#endif
