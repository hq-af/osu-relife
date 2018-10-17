#ifndef _H_MODES
#define _H_MODES
#define _CRT_SECURE_NO_WARNINGS
#include "Raymond.h"
#include "Utils.h"
#include <chrono>
#include <thread>


int SwitchMode(int argc, char* argv[]);
int InstallMode(int argc, char* argv[]);
int UninstallMode(int argc, char* argv[]);
int PrepareInjectMode(int argc, char* argv[]);
int InjectMode(int argc, char* argv[]);
int ForwardToOsuMode(int argc, char* argv[]);

#endif