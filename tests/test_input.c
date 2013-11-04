#include "cog.h"
#include "cog_anim.h"
#include "cog_log.h"
#include "cog_math.h"

static const int N_ENTITIES = 1;

typedef struct entity {
    cog_anim_id anim;
} entity;

void entity_init(entity* obj) {
    obj->anim = cog_anim_add("media/kitten_anim.png", 3);
    cog_anim_set(obj->anim, (cog_anim) {
        .dim = (cog_dim2) {.w=0.3, .h=0.3},
        .pos = (cog_pos2) {.x=0, .y=0},
        .vel = (cog_vec2) {.x=0.0001, .y=0.0003},
        .transition_millis = 150,
         .looped = COG_TRUE,
          .ang_vel = COG_PI/14000
    });
    cog_anim_set_frames(obj->anim, 0, 1, 2);
}

void entity_update(entity* obj) {
    cog_anim* anim = cog_anim_get(obj->anim);
    anim->pos.x = cog_input_mouse_x();
    anim->pos.y = cog_input_mouse_y();
}

int main(void) {
    cog_init();
    entity myentity[N_ENTITIES];
    for(int i = 0; i < N_ENTITIES; i++) {
        entity_init(&myentity[i]);
    }
    while(!cog_hasquit()) {
        for(int i = 0; i < N_ENTITIES; i++) {
            entity_update(&myentity[i]);
        }
        cog_loopstep();
        cog_debugf("x is %lf, y is %lf", cog_input_mouse_x(), cog_input_mouse_y());
    }
}