#include "cog_state.h"

#include "cog_core.h"
#include "cog_log.h"
#include "cog_types.h"

static cog_state_fsm* global_fsm;

cog_state_fsm* cog_state_fsm_alloc(void) {
    cog_state_fsm* obj = COG_STRUCT_MALLOC(cog_state_fsm);
    cog_state_fsm_init(obj);
    if(!global_fsm) {
        global_fsm = obj; //Assuming global_fsm is first fsm created
    }
    return obj;
}

void cog_state_fsm_init(cog_state_fsm* fsm) {
    cog_list_init(&fsm->transitions, sizeof(cog_state_transition));
    fsm->currentstate = COG_STATE_ERROR;
    cog_list_init(&fsm->events, sizeof(cog_event));
    fsm->initial = true;
}

void cog_state_fsm_add_transition(cog_state_fsm* fsm,
                                  cog_state_transition* transition) {
    cog_list_append(&fsm->transitions, transition);
}

void cog_state_fsm_add_transitions(cog_state_fsm* fsm,
                                   cog_state_transition* transitions,
                                   uint32_t size) {
    int32_t i;
    for(i = 0; i < size; i++) {
        cog_state_fsm_add_transition(fsm, &(transitions[i]));
    }
}

void cog_state_fsm_update(cog_state_fsm* fsm) {
    cog_event* event = cog_list_pop(&fsm->events);
    // when no event fired last update, continue in current state
    if(event == COG_NULL) {
        COG_LIST_FOREACH(&fsm->transitions) {
            cog_state_transition* transition =
                (cog_state_transition*) curr->data;
            if(transition->state == fsm->currentstate) {
                // For the very first event of a fsm, say it is initial
                bool initial = false;
                if(fsm->initial) {
                    initial = true;
                    fsm->initial = false;
                }
                fsm->currentstate = transition->transition_fn((cog_state_info) {
                    .initial=initial
                });
                break;
            }
        }
    } else {
        bool was_transition = false;
        // search for a matching transition for this event
        COG_LIST_FOREACH(&fsm->transitions) {
            cog_state_transition* transition =
                (cog_state_transition*) curr->data;
            if(transition->state == fsm->currentstate) {
                if(transition->event == (*event)) {
                    fsm->currentstate = transition->transition_fn((cog_state_info) {
                        .initial=true
                    });
                    was_transition = true;
                    break;
                }
            }
        }
        // default transition if no event found
        if(!was_transition) {
            COG_LIST_FOREACH(&fsm->transitions) {
                cog_state_transition* transition =
                    (cog_state_transition*) curr->data;
                if(transition->event == COG_E_DUMMY) {
                    if(transition->state == fsm->currentstate) {
                        fsm->currentstate = transition->transition_fn((cog_state_info) {
                            .initial=false
                        });
                    }
                }
            }
        }
    }
}

void cog_state_fsm_push_event(cog_state_fsm* fsm, cog_event event) {
    cog_list_append(&fsm->events, &event);
}

void cog_state_fsm_set_state(cog_state_fsm* fsm, cog_state state) {
    fsm->currentstate = state;
}

void cog_state_global_fsm_push_event(cog_event event) {
    if(global_fsm) {
        cog_state_fsm_push_event(global_fsm, event);
    }
}
