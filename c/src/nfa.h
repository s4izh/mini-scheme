#ifndef __NFA_H__
#define __NFA_H__

#include "ds.h"
#include "types.h"

#define MAX_STATES 100
#define ALPHABET_SIZE 256 // ascii

typedef u32 (*nfa_callback_t)(void*, void*);

// typedef enum {
//     NFA_STATE_START,
//     NFA_STATE_PROCESSING,
//     NFA_STATE_ACCEPT,
//     NFA_STATE_REJECT,
//     NFA_STATE_ERROR,
//     NFA_STATE_COUNT,
// } nfa_engine_state_t;

typedef enum {
    bool accept;
    bool determinist;
    const char non_determinist_symbols[ALPHABET_SIZE];
} nfa_state_t;

typedef struct {
    u32 state_prev;
    const char symbols[ALPHABET_SIZE];
    u32 state_final;
    nfa_callback_t non_determinist_cb;
    nfa_callback_t epsillon_cb;
} nfa_transition_t;

DA_DEFINE(nfa_state_t, da_nfa_state);
DA_DEFINE(nfa_transition_t, da_nfa_transition);

typedef struct {
    u32 prev_state;
    u32 current_state;
    da_nfa_state states;
    da_nfa_transition transitions;
} nfa_engine_t;

void nfa_init(nfa_engine_t* nfa, u32 states);
void nfa_configure_state(nfa_engine_t* nfa, u32 state, bool accept, nfa_callback_t non_determinist_cb, nfa_callback_t epsillon_cb);
void nfa_add_transition(nfa_engine_t* nfa, u32 prev_state, string_view_t symbols, u32 next_state);

#endif // !__NFA_H__
