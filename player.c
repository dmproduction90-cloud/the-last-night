#include <pspkernel.h>
#include <pspgu.h>
#include <pspgum.h>
#include <math.h>
#include "player.h"
#include "gfx.h"
#include "world.h"

#define PI_F      3.14159265f
#define WALK_SPEED 2.2f
#define RUN_SPEED  4.5f
#define RADIUS     0.35f

/* Ethan, version provisoire : personnage articule en boites.
   Sera remplace par un vrai modele Blender (etape 8). */
static Vertex __attribute__((aligned(16))) m_skin[36];
static Vertex __attribute__((aligned(16))) m_shirt[36];
static Vertex __attribute__((aligned(16))) m_pants[36];

void player_init(Player *p, float x, float z)
{
    p->x = x; p->z = z;
    p->heading = PI_F;
    p->phase = 0.0f;
    p->amount = 0.0f;

    gfx_make_cube(m_skin,  -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0xff90b0d0u);
    gfx_make_cube(m_shirt, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0xff705030u);
    gfx_make_cube(m_pants, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0xff303030u);
    sceKernelDcacheWritebackAll();
}

void player_update(Player *p, float sx, float sy, float cam_yaw, float dt)
{
    float mag = sqrtf(sx * sx + sy * sy);
    float target_amount = 0.0f;

    if (mag > 0.2f) {
        if (mag > 1.0f) mag = 1.0f;

        float fx = sinf(cam_yaw), fz = cosf(cam_yaw); /* avant (vers ou regarde la camera) */
        float rx = -fz,           rz = fx;            /* droite */

        /* stick vers le haut = sy negatif = avancer */
        float dx = fx * (-sy) + rx * sx;
        float dz = fz * (-sy) + rz * sx;
        float len = sqrtf(dx * dx + dz * dz);
        if (len > 0.0001f) { dx /= len; dz /= len; }

        float speed = (mag > 0.85f) ? RUN_SPEED : WALK_SPEED;
        p->x += dx * speed * dt;
        p->z += dz * speed * dt;
        world_collide(&p->x, &p->z, RADIUS);

        /* rotation douce vers la direction de marche */
        float target = atan2f(dx, dz);
        float diff = target - p->heading;
        while (diff >  PI_F) diff -= 2.0f * PI_F;
        while (diff < -PI_F) diff += 2.0f * PI_F;
        float k = 12.0f * dt; if (k > 1.0f) k = 1.0f;
        p->heading += diff * k;

        p->phase += speed * 2.6f * dt;
        target_amount = 1.0f;
    }

    float a = 10.0f * dt; if (a > 1.0f) a = 1.0f;
    p->amount += (target_amount - p->amount) * a;
}

static void part(const Vertex *mesh, float px, float py, float pz, float rx,
                 float sx, float sy, float sz, float oy)
{
    ScePspFVector3 t  = { px, py, pz };
    ScePspFVector3 o  = { 0.0f, oy, 0.0f };
    ScePspFVector3 s  = { sx, sy, sz };

    sceGumPushMatrix();
    sceGumTranslate(&t);
    sceGumRotateX(rx);
    sceGumTranslate(&o);
    sceGumScale(&s);
    sceGumDrawArray(GU_TRIANGLES,
                    GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                    36, 0, (void *)mesh);
    sceGumPopMatrix();
}

void player_draw(const Player *p)
{
    float swing  = sinf(p->phase) * 0.8f * p->amount;
    float breath = sinf(p->phase * 0.3f) * 0.01f * (1.0f - p->amount);
    ScePspFVector3 pos = { p->x, 0.0f, p->z };

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    sceGumTranslate(&pos);
    sceGumRotateY(p->heading);

    part(m_pants, -0.12f, 0.9f, 0.0f,  swing, 0.20f, 0.90f, 0.22f, -0.45f); /* jambe G */
    part(m_pants,  0.12f, 0.9f, 0.0f, -swing, 0.20f, 0.90f, 0.22f, -0.45f); /* jambe D */
    part(m_shirt,  0.0f, 1.2f + breath, 0.0f, 0.0f, 0.50f, 0.70f, 0.28f, 0.0f); /* torse */
    part(m_shirt, -0.33f, 1.5f, 0.0f, -swing, 0.14f, 0.65f, 0.14f, -0.32f);  /* bras G */
    part(m_shirt,  0.33f, 1.5f, 0.0f,  swing, 0.14f, 0.65f, 0.14f, -0.32f);  /* bras D */
    part(m_skin,   0.0f, 1.72f + breath, 0.0f, 0.0f, 0.26f, 0.28f, 0.26f, 0.0f); /* tete */
}
