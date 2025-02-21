#pragma once

// rename problematic 'namespace' keyword
#define namespace ns_name
#include "wlr-layer-shell-unstable-v1-client-protocol.h"
#undef namespace

// warapper function to avoid issues
inline struct zwlr_layer_surface_v1* create_layer_surface(
    struct zwlr_layer_shell_v1* shell,
    struct wl_surface* surface,
    struct wl_output* output,
    uint32_t layer,
    const char* ns)  // Using ns instead of namespace
{
    return zwlr_layer_shell_v1_get_layer_surface(shell, surface, output, layer, ns);
}
