#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
    float yaw;     /* direction ou regarde la camera */
    float dist;    /* distance derriere le joueur */
    float height;  /* hauteur de la camera */
} Camera;

void camera_init(Camera *c, float yaw);
void camera_update(Camera *c, float yaw_input, float dt);
void camera_apply(const Camera *c, float target_x, float target_z);

#endif
