#include <pspkernel.h>
#include <pspgu.h>
#include <pspgum.h>
#include <math.h>
#include "world.h"
#include "gfx.h"

#define MAX_BOXES 48
#define MAX_VERTS (MAX_BOXES * 36)

typedef struct { float x0, z0, x1, z1; } Rect;

static Vertex __attribute__((aligned(16))) g_verts[MAX_VERTS];
static int  g_count = 0;
static Rect g_solids[MAX_BOXES];
static int  g_nsolids = 0;

static void add_box(float x0, float y0, float z0, float x1, float y1, float z1,
                    unsigned int color, int solid)
{
    if (g_count + 36 > MAX_VERTS) return;
    g_count += gfx_make_cube(&g_verts[g_count], x0, y0, z0, x1, y1, z1, color);
    if (solid && g_nsolids < MAX_BOXES) {
        Rect r = { x0, z0, x1, z1 };
        g_solids[g_nsolids++] = r;
    }
}

void world_init(void)
{
    const unsigned int WALL  = 0xff8a9aa0u;
    const unsigned int WOOD  = 0xff2a4a6au;
    const unsigned int DARK  = 0xff1a2a4au;
    const unsigned int BED   = 0xff404080u;

    g_count = 0;
    g_nsolids = 0;

    /* sol (20 x 16) */
    add_box(0, -0.1f, 0, 20, 0, 16, WOOD, 0);

    /* murs exterieurs, hauteur 3 */
    add_box(0, 0, 0, 20, 3, 0.3f, WALL, 1);            /* nord */
    add_box(0, 0, 15.7f, 8, 3, 16, WALL, 1);           /* sud, gauche de la porte */
    add_box(11, 0, 15.7f, 20, 3, 16, WALL, 1);         /* sud, droite de la porte */
    add_box(0, 0, 0, 0.3f, 3, 16, WALL, 1);            /* ouest */
    add_box(19.7f, 0, 0, 20, 3, 16, WALL, 1);          /* est */

    /* mur interieur avec passage entre z=6 et z=9 */
    add_box(10, 0, 0, 10.3f, 3, 6, WALL, 1);
    add_box(10, 0, 9, 10.3f, 3, 16, WALL, 1);

    /* mobilier */
    add_box(3, 0, 3, 5, 0.9f, 4.5f, DARK, 1);          /* table */
    add_box(14, 0, 1, 18, 0.6f, 4, BED, 1);            /* lit */
    add_box(1, 0, 12, 2.2f, 2, 14.5f, DARK, 1);        /* armoire */
    add_box(13, 0, 12, 17, 0.8f, 13.5f, DARK, 1);      /* canape */

    /* les sommets sont ecrits via le cache : on les envoie en memoire */
    sceKernelDcacheWritebackAll();
}

void world_draw(void)
{
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    sceGumDrawArray(GU_TRIANGLES,
                    GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                    g_count, 0, g_verts);
}

static float clampf(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

void world_collide(float *x, float *z, float radius)
{
    for (int i = 0; i < g_nsolids; i++) {
        const Rect *r = &g_solids[i];
        float cx = clampf(*x, r->x0, r->x1);
        float cz = clampf(*z, r->z0, r->z1);
        float dx = *x - cx;
        float dz = *z - cz;
        float d2 = dx * dx + dz * dz;

        if (d2 >= radius * radius) continue;

        if (d2 > 1e-6f) {
            float d = sqrtf(d2);
            float push = radius - d;
            *x += dx / d * push;
            *z += dz / d * push;
        } else {
            /* centre a l'interieur du bbloc : sortir par le cote le plus proche */
            float l = *x - r->x0, rr = r->x1 - *x;
            float t = *z - r->z0, b = r->z1 - *z;
            float m = l;
            int side = 0;
            if (rr < m) { m = rr; side = 1; }
            if (t  < m) { m = t;  side = 2; }
            if (b  < m) { m = b;  side = 3; }
            if (side == 0) *x = r->x0 - radius;
            else if (side == 1) *x = r->x1 + radius;
            else if (side == 2) *z = r->z0 - radius;
            else *z = r->z1 + radius;
        }
    }
}
