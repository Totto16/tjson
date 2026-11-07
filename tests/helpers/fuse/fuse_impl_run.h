
#pragma once

#define FUSE_USE_VERSION FUSE_MAKE_VERSION(3, 12)
#include <fuse3/fuse.h>

#include <fuse3/fuse_lowlevel.h>

#include "./fuse_shared.h"

// main fn
[[nodiscard]] FuseHandleResult fuse_start_fn(FuseSharedState* state, const FuseStaticData* data);
