#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <windows.h>
#include <process.h>

#define TIME_LIFE_THREADS 5

using namespace std;

struct message{
	char path[32];
	unsigned short clients;
	unsigned short ms;
	char msg[28];
};

HANDLE hMutex;

unsigned __stdcall TheadFunc(void *arg) {
	message* msg = (message*)arg;
	DWORD dwWaitResult;
	SYSTEMTIME time;
	dwWaitResult = WaitForSingleObject(hMutex, 100l);
	ofstream file;
	unsigned short TimeLife = TIME_LIFE_THREADS * 60 / msg->ms;
	while(TimeLife--) {
		if(dwWaitResult != WAIT_TIMEOUT) {
			GetLocalTime(&time);
			file.open(msg->path, ios_base::app); 
			file << time.wHour << ":" << time.wMinute << ":" << time.wSecond << "; " << msg->clients << ": " << msg->msg << " " << TimeLife << endl;
			file.close();
			ReleaseMutex(hMutex);	
			Sleep(msg->ms * 1000);
		}
	}	
}

int main(int argc, char const *argv[]) {
    string path;
    unsigned short clients;

    cin >> path;
    cin >> clients;

	while(path.find("\\") != string::npos) path.replace(path.find("\\"), 1, "/");

	hMutex = CreateMutex(NULL, false, NULL);
	unsigned uThreadIDs[clients];

	HANDLE hThreads[clients];

	message* msg[clients];
	unsigned short ms;
	string m;

	for(int i(0); i < clients; i++) {
		cin >> ms >> m;
		msg[i] = new message;
		msg[i]->clients = i;
		msg[i]->ms = ms;
		strcpy(msg[i]->msg, m.c_str());
		strcpy(msg[i]->path, path.c_str());	
	}

	for(int i(0); i < clients; i++)
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, &TheadFunc, msg[i], 0, &uThreadIDs[i]);	

    WaitForMultipleObjects(clients, hThreads, true, INFINITE);
    
    for(int i(0); i < clients; i++)
    	CloseHandle(hThreads[i]);
    
    CloseHandle(hMutex);
    	
    for(int i(0); i < clients; i++)
    	delete msg[i];

    return 0;
}
