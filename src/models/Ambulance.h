#ifndef AMBULANCE_H
#define AMBULANCE_H

#include "Vehicle.h"

#ifdef _WIN32
#include <windows.h>
DWORD WINAPI thread_ambulance(LPVOID arg);
#else
void *thread_ambulance(void *arg);
#endif

#endif /* AMBULANCE_H */
