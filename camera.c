#include <math.h>
#include "camera.h"
#include "gfx.h"
#include "world.h"

#define TURN_SPEED 2.0f /* rad/s */

void camera_init(Camera *c, float yaw)
{
    c->yaw = yaw;
    c->dist = 4.0f;
    c->height = 2.4f;
}

void camera_update(Camera *c, float yaw_input, float dt)
{
    c->yaw += yaw_input * TURN_SPEED * dt;
}

static float clampf(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

void camera_apply(const Camera *c, float tx, float tz)
{
    float fx = sinf(c->yaw), fz = cosf(c->yaw);
    float ex = tx - fx * c->dist;
    float ez = tz - fz * c->dist;

    /* camera gardee a l'interieur de la maison (collision simplifiee) */
    ex = clampf(ex, WORLD_MIN_X, WORLD_MAX_X);
    ez = clampf(ez, WORLD_MIN_Z, WORLD_MAX_Z);

    gfx_set_camera(ex, c->height, ez, tx, 1.4f, tz);
}
