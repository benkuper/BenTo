#pragma once

#define PROTO_V1 1
#define PROTO_V2 2

#ifndef MODEL
#define MODEL PROTO_V1
#endif

#if MODEL == PROTO_V1
#include "config_protov1.h"
#elif MODEL == PROTO_V2
#include "config_protov2.h"
#endif
