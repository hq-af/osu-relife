#ifndef _H_FACTORY
#define _H_FACTORY
#include "hooks.h"
#include <atlconv.h>
#include <atlstr.h>
#define MAX_NAME 128
#define MAX_MAC 512
#define MAX_MAC_CHARS 16
#define MAX_MAC_COUNT 20
#define MAX_HWID 128
#define MAX_UNINSTALLID 36



PIP_ADAPTER_ADDRESSES GetMAC();
BSTR GetSerialNumber();
BYTE* GetUninstallID();
void Initialize_Factory();
void SetSelected(int index);
void DeleteSelected();
void RefreshSelected();

typedef struct profile {
	bool selected;
	char name[MAX_NAME];
	char mac[MAX_MAC];
	char hwid[MAX_HWID];
	char uninstallID[MAX_UNINSTALLID];
	profile* next;
} profile;

profile* GetProfiles();
profile* GetSelectedProfile();
int GetSelectedProfileIndex();
void ParseMAC(profile* selected);
std::string RandomMAC();
std::string RandomSerial();
std::string RandomUninstallID();
std::string RandomHEX(int length);
void RandomizeSelected();
void SaveToDisk();
void AddProfile(const char name[]);

extern char** mac_addresses;
extern int mac_addresses_count;
extern char* serial_number;
extern char* uninstallid;
extern BSTR serial_number_bstr;
extern int profilesCount;
extern profile* first_profile;

#endif