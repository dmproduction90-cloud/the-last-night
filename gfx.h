#ifndef GFX_H
#define GFX_H

/* Format de sommet : couleur 0xAABBGGRR puis position (ordre exige par le GU) */
typedef struct {
    unsigned int color;
    float x, y, z;
} Vertex;

void gfx_init(void);
void gfx_begin_frame(float fog_far);
void gfx_end_frame(void);
void gfx_set_camera(float ex, float ey, float ez, float cx, float cy, float cz);

/* Ecrit 36 sommets (une boite ombree par face). Retourne 36. */
int gfx_make_cube(Vertex *out, float x0, float y0, float z0,
                  float x1, float y1, float z1, unsigned int color);

#endif
