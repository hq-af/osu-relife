#define _CRT_SECURE_NO_WARNINGS
#include "factory.h"
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#define BUFSIZE 4096

char** mac_addresses;
int mac_addresses_count;
char* serial_number;
char* uninstallid;
BSTR serial_number_bstr;
int profilesCount = 0;
LPTSTR real_path = NULL;

profile* first_profile = NULL;

profile* GetProfiles() {
	return first_profile;
}

profile* GetSelectedProfile() {
	profile* curr = first_profile;
	while (curr) {
		if (curr->selected)
			return curr;
		curr = curr->next;
	}

	return NULL;
}

void RefreshSelected() {
	profile* selected = GetSelectedProfile();
	ParseMAC(selected);
	serial_number = selected->hwid;
	serial_number_bstr = SysAllocString(CA2W(serial_number));
	uninstallid = selected->uninstallID;
}

void SetSelected(int index) {
	profile* curr = first_profile;
	int i = 0;
	while (curr) {
		if (i == index)
			curr->selected = true;
		else if (curr->selected)
			curr->selected = false;
		curr = curr->next;
		i++;
	}

	RefreshSelected();
}

int GetSelectedProfileIndex() {
	profile* curr = first_profile;
	int i = 0;
	while (curr) {
		if (curr->selected)
			return i;
		curr = curr->next;
		i++;
	}

	return 0;
}

void ParseMAC(profile* selected) {

	auto mac = selected->mac;

	char** tab = (char**)malloc(sizeof(char*) * MAX_MAC_COUNT);
	int n = 0;
	int k = 0;
	int i = 0;
	while (mac[i]) {
		if (i == 0 || mac[i] == '.') {
			tab[n] = (char*)calloc(MAX_MAC_CHARS, sizeof(char));
			k = 0;
			n++;
		}
		if (mac[i] != '.') {
			tab[n - 1][k] = mac[i];
			k++;
		}
		i++;
	}

	n--;

	mac_addresses_count = n;
	mac_addresses = (char**)malloc(sizeof(char*) * mac_addresses_count);

	for (int v = 0; v < n; v++)
		mac_addresses[v] = tab[v];
}

int hex2int(char ch1) {
	if (ch1 >= '0' && ch1 <= '9')
		return ch1 - '0';
	if (ch1 >= 'A' && ch1 <= 'F')
		return ch1 - 'A' + 10;
	if (ch1 >= 'a' && ch1 <= 'f')
		return ch1 - 'a' + 10;
}
int hex2int(char ch1, char ch2)
{
	return hex2int(ch1) * 16 + hex2int(ch2);
}

OLECHAR* RandomGUID() {
	GUID guid;
	CoCreateGuid(&guid);

	OLECHAR* guidString;
	StringFromCLSID(guid, &guidString);

	return guidString;
}

PIP_ADAPTER_ADDRESSES CreateAdapter(const char* physicalAddr) {
	IP_ADAPTER_ADDRESSES_LH* clone = new IP_ADAPTER_ADDRESSES_LH();

	clone->AdapterName = (PCHAR)RandomGUID();

	if (physicalAddr == NULL) {
		clone->PhysicalAddressLength = 0;
	}
	else {
		clone->PhysicalAddressLength = strlen(physicalAddr) / 2;
		for (unsigned int i = 0; i < strlen(physicalAddr); i += 2)
			clone->PhysicalAddress[i / 2] = hex2int(physicalAddr[i], physicalAddr[i + 1]);

	}
	//strcpy((char *)clone->PhysicalAddress, physicalAddr);
	
	return clone;
}

PIP_ADAPTER_ADDRESSES GetMAC() {
	auto first = CreateAdapter(mac_addresses_count == 0 || mac_addresses[0][0] == 0 ? NULL : mac_addresses[0]);

	auto current = first;
	for (int i = 1; i < mac_addresses_count; i++) {
		current->Next = CreateAdapter(mac_addresses[i][0] == 0 ? NULL : mac_addresses[i]);
		current = current->Next;
	}

	return first;
}

BSTR GetSerialNumber() {
	return serial_number_bstr;
}

BYTE* GetUninstallID() {
	return (BYTE*)uninstallid;
}


void Initialize_Factory() {

	real_path = (LPTSTR)malloc(BUFSIZE * sizeof(TCHAR));
	//GetEnvironmentVariable(VARNAME, real_path, BUFSIZE);

	GetEnvironmentVariable(L"LOCALAPPDATA", real_path, BUFSIZE);

	FILE* file = _wfopen(StrCatW(real_path, L"\\relife_profiles.ini"), L"r");

	// File does not exist
	if (!file) {
		first_profile = (profile*)malloc(sizeof(profile));
		
		strcpy(first_profile->name, "profile1");
		first_profile->selected = true;
		RandomizeSelected();
		SaveToDisk();

	}
	
	if (!(file = _wfopen(real_path, L"r")))
		exit(1);

	first_profile = NULL;
	profile* current_profile = first_profile;

	std::ifstream infile(file);
	std::string line;
	profilesCount = 0;
	while (std::getline(infile, line))
	{
		if (line[0] == '>' || line[0] == '[') {

			profilesCount++;

			if (first_profile == NULL) {
				first_profile = (profile*)malloc(sizeof(profile));
				current_profile = first_profile;
			}
			else {
				current_profile->next = (profile*)malloc(sizeof(profile));
				current_profile = current_profile->next;
			}

			current_profile->selected = line[0] == '>';
			current_profile->next = NULL;
			strcpy(current_profile->name, line.substr(line[0] == '>' ? 2 : 1, line.length() - 2 - (line[0] == '>' ? 1 : 0)).c_str());
		}
		else if (line.length() > 1 && line[1] == '_') {
			if (line[0] == 'm')
				strcpy(current_profile->mac, line.substr(2, line.length() - 1).c_str());
			else if (line[0] == 'h')
				strcpy(current_profile->hwid, line.substr(2, line.length() - 1).c_str());
			else if (line[0] == 'u')
				strcpy(current_profile->uninstallID, line.substr(2, line.length() - 1).c_str());
		}
	}

	fclose(file);

	/*profile* curr = first_profile;
	while (curr) {
		std::cout << (curr->selected ? "[S] " : "") << "'" << curr->name << "' mac : '" << curr->mac << "' hwid : '" << curr->hwid << "' uninstallID : '" << curr->uninstallID << "'" << std::endl;
		curr = curr->next;
	}*/

	RefreshSelected();
}

std::string RandomMAC() {
	int count = rand() % 4 + 1;
	int emptyCount = rand() % 3;

	std::string mac;

	for (int i = 0; i < count + emptyCount; i++) {
		if (i < count)
			mac += RandomHEX(12);
		mac += '.';
	}
	return mac;
}

std::string RandomUninstallID() {
	std::string res;

	res += CW2A(RandomGUID());

	return res.substr(1, res.size() - 2);
}

std::string RandomSerial() {
	return RandomHEX(rand() % 7 + 8);
}

std::string RandomHEX(int length) {
	std::string mac;

	for (int i = 0; i<length; i++)
	{
		int x = rand() % 16;
		if (x>9)
		{
			char letter = 'A' + (x-10);
			mac += letter;
		}
		else
		{
			mac += std::to_string(x);
		}
	}
	return mac;
}

void RandomizeSelected() {
	auto selected = GetSelectedProfile();

	strcpy(selected->mac, RandomMAC().c_str());
	strcpy(selected->hwid, RandomSerial().c_str());
	strcpy(selected->uninstallID, RandomUninstallID().c_str());


	RefreshSelected();
}

void SaveToDisk() {
	FILE* file = _wfopen(real_path, L"w");
	std::ofstream profiles(file);
	
	if (profiles)
	{
		profile* curr = first_profile;
		while (curr) {
			profiles << (curr->selected ? ">" : "") << '[' << curr->name << ']' << std::endl
				<< "m_" << curr->mac << std::endl
				<< "h_" << curr->hwid << std::endl
				<< "u_" << curr->uninstallID << std::endl << std::endl;

			curr = curr->next;
		}
		profiles.close();
		fclose(file);
	}
}

void DeleteSelected() {

	if (profilesCount == 1) {
		strcpy(first_profile->name, "profile1");
		RandomizeSelected();
		return;
	}
	
	auto sindex = GetSelectedProfileIndex();
	auto sporfile = GetSelectedProfile();

	if (sindex == 0) {
		first_profile = sporfile->next;
		first_profile->selected = true;
	}
	else {
		int i = 0;
		profile* predec = first_profile;
		while (predec && i < sindex - 1) {
			predec = predec->next;
			i++;
		}

		predec->next = predec->next->next;
		predec->selected = true;
		
	}
	profilesCount--;
	RefreshSelected();
}

void AddProfile(const char name[]) {
	profile* newp = (profile*)malloc(sizeof(profile));


	int i = 0;
	while (name[i]) {
		newp->name[i / 2] = name[i];
		i += 2;
	}
	newp->name[i / 2] = NULL;

	newp->next = first_profile;
	first_profile = newp;
	profile* selec;
	while ((selec = GetSelectedProfile())) {
		selec->selected = false;
	}
	newp->selected = true;
	profilesCount++;
	RandomizeSelected();
}