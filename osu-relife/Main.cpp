#include "Modes.h"

int main(int argc, char* argv[]) {

	SetProcessDPIAware();

	if (GetProcessByName("osu!.exe") != NULL && argc > 2)
		return ForwardToOsuMode(argc, argv);
	else if (argc < 2)
		return SwitchMode(argc, argv);
	else if (strcmp(argv[1], "/install") == 0)
		return InstallMode(argc, argv);
	else if (strcmp(argv[1], "/uninstall") == 0)
		return UninstallMode(argc, argv);
	else if (strcmp(argv[1], "/inject") == 0)
		return InjectMode(argc, argv);
	else
		return PrepareInjectMode(argc, argv);
}
