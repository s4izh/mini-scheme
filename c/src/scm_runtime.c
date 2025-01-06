#include "scm_runtime.h"
#include "ds.h"

void scm_runtime_init(scm_runtime_t* runtime, scm_resources_t* resources, scm_runtime_mode_t mode)
{
    runtime->resources = resources;
    runtime->mode = mode;
}

scm_binding_t* scm_runtime_lookup_binding(scm_runtime_t* runtime, const char* name)
{
    // we start looking from the last environment created
    // (the most nested one)
    for (u32 i = da_size(&runtime->environments - 1); i >= 0; i--) {
        scm_environment_t* env = &da_at(&runtime->environments, i);
        for (u32 j = 0; j < da_size(&env->bindings); j++) {
            scm_binding_t* binding = &da_at(&env->bindings, j);
            if (strcmp(binding->name, name) == 0) {
                return binding;
            }
        }
    }
    return NULL;
}

void scm_runtime_push_environment(scm_runtime_t* runtime)
{
    da_append(
        &runtime->environments,
        ((scm_environment_t) { .bindings = { NULL, 0, 0 } }));
}

void scm_binding_free(scm_binding_t* binding)
{
    switch (binding->type) {
    case SCM_BINDING_FUNCTION:
        // free(binding->data.function.sexpr);
        break;
    case SCM_BINDING_LIST:
        // free(binding->data.list.sexpr);
        break;
    case SCM_BINDING_BUILTIN:
        break;
    }
}

void scm_runtime_pop_environment(scm_runtime_t* runtime)
{
    scm_environment_t* env = &da_at(&runtime->environments, da_size(&runtime->environments) - 1);
    for (u32 i = 0; i < da_size(&env->bindings); i++) {
        scm_binding_t* binding = &da_at(&env->bindings, i);
        scm_binding_free(binding);
    }
    da_free(&env->bindings);
    da_remove_last(&runtime->environments);
}
