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
#include "SimpleIni/SimpleIni.h"
#include "SimpleIni/ConvertUTF.h"
#include <stdio.h>
#include <string.h>
#include "curl/curl.h"
#pragma comment(lib, "curl/lib/libcurl.a")
#pragma comment(lib, "curl/lib/libcurl.dll.a")
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


using namespace std;


/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
static size_t read_callback(char* ptr, size_t size, size_t nmemb, FILE* stream)
{
	curl_off_t nread;
	/* in real-world cases, this would probably get this data differently
	   as this fread() stuff is exactly what the library already would do
	   by default internally */
	size_t retcode = fread(ptr, size, nmemb, stream);

	nread = (curl_off_t)retcode;

	fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
		" bytes from file\n", nread);
	return retcode;
}


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

int timewait;
int timewait_schedule;

string filename;
string filename_schedule;

string ftp_enabled = "0";

string sftp_on;
string sftp_SetInsecure;
string ftp_activemode;
string ftp_passwd;
string ftp_username;
string ftp_port;
string ftp_host;

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

void loadconfigfile() {
	CSimpleIniA ini;
	ini.SetUnicode();
	SI_Error rc = ini.LoadFile(".\\plugins\\DR.opl");
	if (rc >= 0) {
		ftp_enabled = ini.GetValue("FTP", "enabled");
		if (ftp_enabled=="1") {
			ftp_host = ini.GetValue("FTP", "host");
			ftp_port = ini.GetValue("FTP", "port");
			ftp_username = ini.GetValue("FTP", "username");
			ftp_passwd = ini.GetValue("FTP", "passwd");
			timewait = stoi(ini.GetValue("PLUGIN", "timewait"));
			timewait_schedule = stoi(ini.GetValue("PLUGIN", "timewait_schedule"));
//			ftp_activemode = ini.GetValue("FTP", "activemode");
//			sftp_on = ini.GetValue("SFTP", "enabled");
//			sftp_SetInsecure = ini.GetValue("SFTP", "SetInsecure");
		}
	}


}

int uploadFTP(string filename) {
	struct stat file_info;
	int errorcode=0;
	if (stat(filename.c_str(), &file_info) == 0)
	if (ftp_enabled == "1") {
		CURL* curl;
		CURLcode res;
		FILE* hd_src;
		const char * REMOTE_URL = ("ftp://"+ftp_host+"/"+filename).c_str();
		struct curl_slist* headerlist = NULL;

		curl_off_t fsize = (curl_off_t)file_info.st_size;
		/* get a FILE * of the same file */
		hd_src = fopen(filename.c_str(), "rb");
		/* In windows, this will init the winsock stuff */
		curl_global_init(CURL_GLOBAL_ALL);
		/* get a curl handle */
		curl = curl_easy_init();
		if (curl) {
			/* build a list of commands to pass to libcurl */
			headerlist = curl_slist_append(headerlist, ("RNFR " + filename).c_str());
			/* we want to use our own read function */
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
			/* enable uploading */
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			/* specify target */
			curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);
			curl_easy_setopt(curl, CURLOPT_PORT, ftp_port.c_str());
			/* pass in that last of FTP commands to run after the transfer */
			curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);
			/* now specify which file to upload */
			curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

			curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
				(curl_off_t)fsize);

			curl_easy_setopt(curl, CURLOPT_USERPWD, (ftp_username+":"+ftp_passwd).c_str());

			/* Now run off and do what you've been told! */
			res = curl_easy_perform(curl);
			/* Check for errors */
			if (res != CURLE_OK) {
				const char * error = curl_easy_strerror(res);
				errorcode = MessageBox(NULL, ("Failed to send csv to FTP server!\n"+string(error)).c_str(), (string(REMOTE_URL) + " - FTP Client").c_str(), MB_RETRYCANCEL+MB_ICONERROR+MB_SYSTEMMODAL);
			} else
				errorcode = MessageBox(NULL, "Successfully uploaded csv file to FTP server!", (string(REMOTE_URL) + " - FTP Client").c_str(), MB_OK + MB_SYSTEMMODAL);

			/* clean up the FTP commands list */
			curl_slist_free_all(headerlist);

			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		fclose(hd_src); /* close the local file */

		curl_global_cleanup();
	}
	return (errorcode);
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
		loadconfigfile();
		ifstream test;
		test.open(".\\plugins\\DR.opl", ios::in);
		string aaa = ("ftp enabled:" + ftp_enabled +  " " + ftp_host + ":" + ftp_port);
		string test2((istreambuf_iterator<char>(test)), istreambuf_iterator<char>());
		MessageBox(NULL,aaa.c_str(), ("Hello " + ftp_username + "!" + " - FTP Client").c_str(), MB_OK + MB_SYSTEMMODAL);

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

		if ((seconds - secsprev) >= timewait) {
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
		schedule.open(filename_schedule, ios::out | ios::binary | ios::app);
		if ((seconds - secsprev_schedule) >= timewait_schedule) {
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
		int errorcode;
		do{
			errorcode = uploadFTP(filename);
		} while (errorcode == IDRETRY);
		do {
			errorcode = uploadFTP(filename_schedule);
		} while (errorcode == IDRETRY);
		
	}

}
