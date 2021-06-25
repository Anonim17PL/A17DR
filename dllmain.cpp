// dllmain.cpp : Definiuje punkt wejścia dla aplikacji DLL.
#include "pch.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <math.h>
#include <string>
#include <sstream>
#include <string>
using namespace std;

int secsprev;
int secsprev_schedule;
unsigned short varindexmax;
int secondprev=0;
double val[999];
unsigned short varindexprev = -1;
intptr_t Sval[5];
string Stval[20];

ofstream plik;
ofstream schedule;
string filename;
string filename_schedule;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


std::string encode(const std::wstring& wstr, int codePage = GetACP())
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(codePage, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(codePage, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

extern "C" {

	__declspec(dllexport)void __stdcall PluginStart(void* aOwner)
	{
		char bufor[64];
		time_t czas;
		time(&czas);
		tm czasTM;
		localtime_s(&czasTM, &czas);
		strftime(bufor, sizeof(bufor), "%#d.%m.%y %H.%M", &czasTM);
		filename = string(bufor) + string(".csv");
		filename_schedule = string(bufor) + string("_schedule.csv");

	}


	__declspec(dllexport)void __stdcall AccessVariable(unsigned short varindex, float* value, bool* write)
	{
		unsigned int seconds;
		seconds = Sval[3];
		val[varindex] = (double)*value;
		plik.open(filename, ios::out | ios::app);
		int hour = seconds / 3600;
		int minute = (seconds % 3600) / 60;
		int second = seconds % 60;

		if (varindexmax < varindex) {
			varindexmax = varindex;
		}

		if ((seconds - secsprev) >= 1) {
			secsprev = seconds;
			unsigned short i = 0;
			for (i = 0; i <= varindexmax; i++) {

				if (i == 0) {
					plik << endl << Sval[0] << "." << Sval[1] << "." << Sval[2] << "," << hour << ":" << minute << ":" << second << "," << Sval[4] << "," << val[i];
				}
				else {
					plik << "," << fixed << val[i];
				}
			}
		}
		plik.close();
		// schedule file
		schedule.open(filename_schedule, ios::binary | ios::app);
		if ((seconds - secsprev_schedule) >= 2) {
			secsprev_schedule = seconds;
			schedule << endl << Sval[0] << "." << Sval[1] << "." << Sval[2] << "," << hour << ":" << minute << ":" << second << "," << Stval[0] << "," << Stval[1] << "," << Stval[2] << "," << Stval[3];
		}
		schedule.close();
	}


	__declspec(dllexport)void __stdcall AccessTrigger(unsigned short triggerindex, bool* active)
	{

	}

	__declspec(dllexport)void __stdcall AccessSystemVariable(unsigned short varindex, float* value, bool* write)
	{
		Sval[varindex] = (intptr_t)*value;
	}

	__declspec(dllexport)void __stdcall AccessStringVariable(unsigned short varindex, wchar_t* value, bool* write)
	{
		Stval[varindex] = encode(value);
	}

	__declspec(dllexport)void __stdcall PluginFinalize()
	{

	}

}