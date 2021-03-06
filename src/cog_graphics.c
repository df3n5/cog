#include "cog_graphics.h"

#include "stdio.h"

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_defs.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_rect.h"
#include "cog_sprite.h"

#include "cog_graphics_opengl.h"

typedef struct {
    void (*init)(cog_window*);
    void (*clear)(void);
    void (*draw)(void);
    void (*draw_sprite)(cog_sprite* sprite, uint32_t idx);
    void (*draw_rect)(cog_rect* rect);
    void (*draw_text)(cog_text* text);
    uint32_t (*gen_tex_id)(void);
    uint32_t (*load_texture)(const char* filename, int* width, int* height);
    void (*prepare)(uint32_t amount);
    void (*set_camera_pos)(cog_pos2* pos);
    void (*custom_render)();
    void (*flush)(void);
} cog_renderer;

static cog_renderer r;
static cog_map sprite_cache;
static cog_list texture_list;
static cog_vec2 camera_vel;
static cog_pos2 camera_pos;

void cog_graphics_draw_rect(cog_rect* rect) {
    r.draw_rect(rect);
}

/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite, uint32_t idx) {
    r.draw_sprite(sprite, idx);
}

void cog_graphics_draw_text(cog_text* text) {
    r.draw_text(text);
}

uint32_t cog_graphics_load_texture(const char* filename, int* width, int* height) {
    //Cache textures so we don't load the same thing twice.
    void* item = cog_map_get_hash(&sprite_cache, filename);
    if(item != 0) {
        return *(uint32_t*)item;
    } else {
        uint32_t* tex_id = cog_malloc(sizeof(uint32_t));
        (*tex_id) = r.load_texture(filename, width, height);
        cog_map_put_hash(&sprite_cache, filename, (cog_dataptr)tex_id);
        cog_list_append(&texture_list, (cog_dataptr)tex_id);
        cog_debugf("Inserting tex_id %d into list for filename %s", *tex_id, filename);
        return (*tex_id);
    }
}


void cog_graphics_init(cog_window* win) {
    // Default and best supported method is opengl
    r.draw = cog_graphics_opengl_draw;
    r.draw_rect = cog_graphics_opengl_draw_rect;
    r.draw_sprite = cog_graphics_opengl_draw_sprite;
    r.init = cog_graphics_opengl_init;
    r.draw_text = cog_graphics_opengl_draw_text;
    r.gen_tex_id = cog_graphics_opengl_gen_tex_id;
    r.load_texture = cog_graphics_opengl_load_texture;
    r.prepare = cog_graphics_opengl_prepare;
    r.set_camera_pos = cog_graphics_opengl_set_camera_pos;
    r.clear = cog_graphics_opengl_clear;
    r.flush = cog_graphics_opengl_flush;
    r.custom_render = NULL;
    r.init(win);
    cog_map_init(&sprite_cache);
    cog_list_init(&texture_list, sizeof(uint32_t));
    //camera_pos.x = 1.0;
}

void cog_graphics_update(double timestep) {
    camera_pos.x += camera_vel.x * timestep;
    camera_pos.y += camera_vel.y * timestep;
}

void cog_graphics_set_custom_render(void (*custom_render)()) {
    r.custom_render = custom_render;
}

void cog_graphics_render(cog_window* window) {
    //Clear color buffer
    r.clear();
    r.set_camera_pos(&camera_pos);
    for(int i = 0; i < COG_LAYER_MAX; i++) {
        COG_LIST_FOREACH(&texture_list) {
            uint32_t global_idx = 0;
            uint32_t tex_id = *((uint32_t*)curr->data);
            uint32_t cnt = cog_sprite_len(tex_id, i) + cog_anim_len(tex_id, i);
            if(cnt > 0) {
                r.prepare(cnt);
                global_idx += cog_sprite_draw_layer(i, tex_id, global_idx);
                global_idx += cog_anim_draw_layer(i, tex_id, global_idx);
                r.draw();
            }
        }
        cog_rect_draw_layer(i);
        cog_text_draw_layer(i);
    }
    if(r.custom_render != NULL) {
        r.custom_render();
    }
    r.flush();
}

void cog_graphics_cam_set(cog_pos2* pos) {
    camera_pos = (*pos);
}

void cog_graphics_cam_get(cog_pos2* pos) {
    (*pos) = camera_pos;
}

void cog_graphics_cam_vel_set(cog_vec2* vel) {
    camera_vel = (*vel);
}

void cog_graphics_cam_vel_get(cog_vec2* vel) {
    (*vel) = camera_vel;
}

uint32_t cog_graphics_gen_tex_id() {
    uint32_t* tex_id = cog_malloc(sizeof(uint32_t));
    (*tex_id) = r.gen_tex_id();
    char buf[COG_MAX_BUF];
    sprintf(buf, "tex_%d", (*tex_id));
    cog_map_put_hash(&sprite_cache, buf, (cog_dataptr)tex_id);
    cog_list_append(&texture_list, (cog_dataptr)tex_id);
    cog_debugf("Inserting tex_id %d into list for tex %s", *tex_id, buf);
    return (*tex_id);
}
