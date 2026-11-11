#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    float x, z;        /* position au sol */
    float heading;     /* direction du regard (radians) */
    float phase;       /* phase d'animation de marche */
    float amount;      /* 0 = immobile, 1 = en mouvement (lisse) */
} Player;

void player_init(Player *p, float x, float z);
void player_update(Player *p, float stick_x, float stick_y, float cam_yaw, float dt);
void player_draw(const Player *p);

#endif
