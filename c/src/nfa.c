#include "nfa.h"

void nfa_init(nfa_engine_t* nfa, u32 states)
{
    da_init_with_preallocation(nfa->states, states);
}

void nfa_configure_state(nfa_engine_t* nfa, u32 state, bool accept, nfa_callback_t non_determinist_cb, nfa_callback_t epsillon_cb)
{
    if (state < da_size(nfa->states))
    {
        return;
    }
    nfa_state_t* s = &da_at(nfa->states, state);
    s->accept = accept;
    s->non_determinist_cb = non_determinist_cb;
    s->epsillon_cb = epsillon_cb;
}

void nfa_add_transition(nfa_engine_t* nfa, u32 state_prev, string_view_t symbols, u32 state_next)
{
    da_append(nfa->transitions, (nfa_transition) { state_prev, symbols, state_next });
}
