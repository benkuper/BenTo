#pragma once

#define SERIAL_DEBUG 1

#if SERIAL_DEBUG
#define DBG(msg) Serial.println(msg)
#define NDBG(msg) Serial.println("["+name+"] "+msg)
#else
#define DBG(msg)
#endif