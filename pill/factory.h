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
#define NB_SERV 8
#define MAX_HOST 128

extern const char* servers_labels[NB_SERV];
extern const char* servers[NB_SERV];

PIP_ADAPTER_ADDRESSES GetMAC();
BSTR GetSerialNumber();
BYTE* GetUninstallID();
void Initialize_Factory();
void SetSelected(int index);
void DeleteSelected();
void RefreshSelected();
void SetServer(int serv);

typedef struct profile {
	bool selected;
	char name[MAX_NAME];
	char mac[MAX_MAC];
	char hwid[MAX_HWID];
	char uninstallID[MAX_UNINSTALLID];
	int serverID;
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
extern profile* last_selected;
extern wchar_t server_host[MAX_HOST];

#endif