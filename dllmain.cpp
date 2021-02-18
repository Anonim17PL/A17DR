// dllmain.cpp : Definiuje punkt wejścia dla aplikacji DLL.
#include "pch.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <math.h>
#include <string>
#include <sstream>
using namespace std;

int secondprev=0;
unsigned short varindexprev = -1;
intptr_t Sval[5];

ofstream plik;
string filename;

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

	}


	__declspec(dllexport)void __stdcall AccessVariable(unsigned short varindex, float* value, bool* write)
	{
		unsigned int seconds;
		seconds = Sval[3];
		intptr_t val[999];
		val[varindex] = (intptr_t)*value;
		plik.open(filename, ios::out | ios::app);

		int hour = seconds / 3600;
		int minute = (seconds % 3600) / 60;
		int second = seconds % 60;

		if (varindex == 0) {
			plik << endl << Sval[0] << "." << Sval[1] << "." << Sval[2] << "," << hour << ":" << minute << ":" << second << "," << Sval[4] << "," << val[varindex];
		}
		else {
			plik << "," << val[varindex];
		}

		plik.close();
	}


	__declspec(dllexport)void __stdcall AccessTrigger(unsigned short triggerindex, bool* active)
	{

	}

	__declspec(dllexport)void __stdcall AccessSystemVariable(unsigned short varindex, float* value, bool* write)
	{
		Sval[varindex] = (intptr_t)*value;
	}

	__declspec(dllexport)void __stdcall AccessStringVariable(unsigned short varindex, char* value, bool* write)
	{

	}

	__declspec(dllexport)void __stdcall PluginFinalize()
	{

	}
}