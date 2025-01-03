#include "nfa.h"
#include "ds.h"

void nfa_init(nfa_engine_t* nfa, u32 states, state_to_str_callback_t cb, void* user_data)
{
    da_init_with_preallocation(&nfa->states, states);
    nfa->state_to_str_cb = cb;
    nfa->user_data = user_data;
}

void nfa_configure_state(nfa_engine_t* nfa, u32 state, nfa_conclusion_t conclusion, nfa_callback_t non_determinist_cb, nfa_callback_t epsillon_cb)
{
    if (state < da_size(&nfa->states)) {
        return;
    }
    nfa_state_t* s = &da_at(&nfa->states, state);
    s->conclusion = conclusion;
    s->non_determinist_cb = non_determinist_cb;
    s->epsillon_cb = epsillon_cb;
}

void nfa_add_transition(nfa_engine_t* nfa, u32 state_prev, const char* symbols, u32 state_next)
{
    u32 size = strlen(symbols);
    da_char transition_symbols;
    da_create_from_c_array(&transition_symbols, symbols, size);

    da_append(&nfa->transitions, ((nfa_transition_t) { state_prev, transition_symbols, state_next }));
}

void nfa_restart(nfa_engine_t* nfa, u32 state)
{
    nfa->state_current = state;
}

static bool nfa_symbol_in_transition(nfa_transition_t* transition, char c)
{
    for (u32 i = 0; i < da_size(&transition->symbols); ++i) {
        if (da_at(&transition->symbols, i) == c) {
            return true;
        }
    }
    return false;
}

static const char* conclusion_to_str(nfa_conclusion_t conclusion)
{
    switch (conclusion) {
        case NFA_ACCEPT: return "accept";
        case NFA_CONTINUE: return "continue";
        case NFA_REJECT: return "reject";
    }
    return "unknown";
} 

nfa_conclusion_t nfa_process(nfa_engine_t* nfa, char c)
{
    // TODO: mirar si el simbolo no es determinista para el estado
    // en el que estamos
    // if ()
    //     if (!da_at(&nfa->states, nfa->state_current).determinist)
    //     {
    //             printf("call non determinist function");
    //     }

    bool found_transition = false;

    nfa->state_prev = nfa->state_current;

    for (u32 i = 0; i < da_size(&nfa->transitions); ++i) {
        nfa_transition_t* transition = &da_at(&nfa->transitions, i);
        if (transition->state_prev != nfa->state_current) {
            continue;
        }
        if (nfa_symbol_in_transition(transition, c)) {
            found_transition = true;
            nfa->state_current = transition->state_next;

            nfa_state_t* state = &da_at(&nfa->states, nfa->state_current);
            if (state->epsillon_cb != NULL)
                nfa->state_current = state->epsillon_cb(nfa, nfa->user_data);
        }
    }

    nfa_conclusion_t conclusion;

    if (!found_transition)
        conclusion = NFA_REJECT;
    else
        conclusion = da_at(&nfa->states, nfa->state_current).conclusion;

    // printf("%-*.*s -> %c -> %s (%s)\n",
    //         28, 28, nfa->state_to_str_cb(nfa->state_prev),
    //         c,
    //         nfa->state_to_str_cb(nfa->state_current),
    //         conclusion_to_str(conclusion));

    return conclusion;
}
