#include <cog.h>

/*-----------------------------------------------------------------------------
 *  Should see sprite in centre of screen
 *-----------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    cog_init();
    cog_sprite_id id = cog_sprite_add("../assets/test0.png");
    cog_sprite_set(id, (cog_sprite) {
        .dim=(cog_dim2) {
            .w=1.0, .h=1.0
        },
        .rot=COG_PI/2
    });
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

