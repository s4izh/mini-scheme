#ifndef __NFA_H__
#define __NFA_H__

#include "ds.h"
#include "types.h"

#define MAX_STATES 100
#define ALPHABET_SIZE 256 // ascii

typedef struct _nfa_engine_t nfa_engine_t;
typedef struct _nfa_state_t nfa_state_t;
typedef struct _nfa_transition_t nfa_transition_t;

typedef u32 (*nfa_callback_t)(nfa_engine_t*, void*);
typedef const char* (*state_to_str_callback_t)(u32);

DA_DEFINE(nfa_state_t, da_nfa_state);
DA_DEFINE(nfa_transition_t, da_nfa_transition);

typedef enum {
    NFA_ACCEPT,
    NFA_CONTINUE,
    NFA_REJECT,
} nfa_conclusion_t;

struct _nfa_state_t {
    nfa_conclusion_t conclusion;
    bool determinist;
    da_char non_determinist_symbols;
    nfa_callback_t non_determinist_cb;
    nfa_callback_t epsillon_cb;
};

struct _nfa_transition_t {
    u32 state_prev;
    da_char symbols;
    u32 state_next;
};

struct _nfa_engine_t {
    u32 state_prev;
    u32 state_current;
    da_nfa_state states;
    da_nfa_transition transitions;
    state_to_str_callback_t state_to_str_cb;
    void* user_data;
};

void nfa_init(nfa_engine_t* nfa, u32 states, state_to_str_callback_t cb, void* user_data);
void nfa_configure_state(nfa_engine_t* nfa, u32 state, nfa_conclusion_t accept, nfa_callback_t non_determinist_cb, nfa_callback_t epsillon_cb);

void nfa_restart(nfa_engine_t* nfa, u32 state);
nfa_conclusion_t nfa_process(nfa_engine_t* nfa, char c);

void nfa_add_transition(nfa_engine_t* nfa, u32 state_prev, const char* symbols, u32 state_next);
void nfa_add_transition_safe(nfa_engine_t* nfa, u32 state_prev, u32 state_next, const char* symbols, u32 size);

void nfa_generated_register_transitions(nfa_engine_t* nfa);

void nfa_check_indeterminations(nfa_engine_t* nfa);

#endif // !__NFA_H__
