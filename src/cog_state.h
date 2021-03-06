#ifndef  COG_STATE_H
#define  COG_STATE_H

#include "cog_types.h"
#include "cog_list.h"

typedef int32_t cog_state;
typedef int32_t cog_event;

#define COG_STATE_ERROR -255

#define COG_E_DUMMY -1
#define COG_E_KEYDOWN -2
#define COG_E_MOUSELDOWN -3

typedef struct cog_state_info {
    bool initial;
} cog_state_info;

typedef struct cog_state_transition {
    /**
     * Unique ID for the current state.
     * */
    cog_state state;
    /**
     * Unique ID for transition event.
     * */
    cog_event event;
    /* *
     * Transition function called when the event occurs while in this state.
     * It returns the new state's type.
     * */
    cog_state(*transition_fn)(cog_state_info);
} cog_state_transition;

typedef struct cog_state_fsm {
    cog_list transitions;
    cog_state currentstate;
    cog_list events;
    bool initial;
} cog_state_fsm;

cog_state_fsm* cog_state_fsm_alloc(void);
void cog_state_fsm_init(cog_state_fsm* fsm);
/* *
 * Add a single transition.
 * */
void cog_state_fsm_add_transition(cog_state_fsm* fsm,
                                  cog_state_transition* transition);
/* *
 * Add an array of transitions.
 * */
void cog_state_fsm_add_transitions(cog_state_fsm* fsm,
                                   cog_state_transition* transitions,
                                   uint32_t size);
/* *
 * Process any new events and transition the state of the fsm accordingly.
 * */
void cog_state_fsm_update(cog_state_fsm* fsm);
/* *
 * Push an event which will cause a transition in the cog_state_fsm when it reaches the front of the fifo.
 * */
void cog_state_fsm_push_event(cog_state_fsm* fsm, cog_event event);
/* *
 * Set the current state of the FSM.
 * */
void cog_state_fsm_set_state(cog_state_fsm* fsm, cog_state state);

void cog_state_global_fsm_push_event(cog_event event);

#endif // COG_STATE_H
