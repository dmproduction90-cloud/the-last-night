#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include "gfx.h"

#define BUF_WIDTH  512
#define SCR_WIDTH  480
#define SCR_HEIGHT 272
#define FOG_COLOR  0xff0a0806u

static unsigned int __attribute__((aligned(16))) list[262144];

void gfx_init(void)
{
    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, (void *)0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void *)0x88000, BUF_WIDTH);
    sceGuDepthBuffer((void *)0x110000, BUF_WIDTH);
    sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
    sceGuDepthRange(65535, 0);
    sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuDisable(GU_CULL_FACE); /* etape 1 : pas de culling, on fixera l'ordre des faces plus tard */
    sceGuShadeModel(GU_SMOOTH);
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

void gfx_begin_frame(float fog_far)
{
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(FOG_COLOR);
    sceGuClearDepth(0);
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

    sceGuFog(1.0f, fog_far, FOG_COLOR);
    sceGuEnable(GU_FOG);

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(75.0f, 16.0f / 9.0f, 0.3f, 100.0f);
}

void gfx_end_frame(void)
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

void gfx_set_camera(float ex, float ey, float ez, float cx, float cy, float cz)
{
    ScePspFMatrix4 view;
    ScePspFVector3 eye = { ex, ey, ez };
    ScePspFVector3 ctr = { cx, cy, cz };
    ScePspFVector3 up  = { 0.0f, 1.0f, 0.0f };

    gumLookAt(&view, &eye, &ctr, &up);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadMatrix(&view);
}

static unsigned int shade(unsigned int c, float f)
{
    unsigned int r = (unsigned int)((c & 0xff) * f);
    unsigned int g = (unsigned int)(((c >> 8) & 0xff) * f);
    unsigned int b = (unsigned int)(((c >> 16) & 0xff) * f);
    return 0xff000000u | (b << 16) | (g << 8) | r;
}

static void quad(Vertex *v, const float p[4][3], unsigned int c)
{
    static const int idx[6] = { 0, 1, 2, 0, 2, 3 };
    for (int i = 0; i < 6; i++) {
        v[i].color = c;
        v[i].x = p[idx[i]][0];
        v[i].y = p[idx[i]][1];
        v[i].z = p[idx[i]][2];
    }
}

int gfx_make_cube(Vertex *o, float x0, float y0, float z0,
                  float x1, float y1, float z1, unsigned int c)
{
    const float top[4][3]    = {{x0,y1,z0},{x0,y1,z1},{x1,y1,z1},{x1,y1,z0}};
    const float bottom[4][3] = {{x0,y0,z0},{x1,y0,z0},{x1,y0,z1},{x0,y0,z1}};
    const float front[4][3]  = {{x0,y0,z1},{x1,y0,z1},{x1,y1,z1},{x0,y1,z1}};
    const float back[4][3]   = {{x1,y0,z0},{x0,y0,z0},{x0,y1,z0},{x1,y1,z0}};
    const float right[4][3]  = {{x1,y0,z1},{x1,y0,z0},{x1,y1,z0},{x1,y1,z1}};
    const float left[4][3]   = {{x0,y0,z0},{x0,y0,z1},{x0,y1,z1},{x0,y1,z0}};

    quad(o,      top,    shade(c, 1.0f));
    quad(o + 6,  bottom, shade(c, 0.4f));
    quad(o + 12, front,  shade(c, 0.8f));
    quad(o + 18, back,   shade(c, 0.6f));
    quad(o + 24, right,  shade(c, 0.7f));
    quad(o + 30, left,   shade(c, 0.5f));
    return 36;
}
