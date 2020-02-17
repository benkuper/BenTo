#pragma once

#define MODEL PROTO_V1

#if MODEL == PROTO_V1
#include "config_protov1.h"
#elif MODEL == PROTO_V2
#include "config_protov2.h"
#endif