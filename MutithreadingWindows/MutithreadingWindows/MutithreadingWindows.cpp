// MutithreadingWindows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "windows.h"

using namespace std;

#define THREADCOUNT 10
#define COUNT 10


int counter = 0;
CRITICAL_SECTION cs;
HANDLE gmutex;


DWORD WINAPI inc(LPVOID lpParam) {
    for (int i = 0; i < COUNT; i++) {
        counter++;
        Sleep(10);
    }

    return 1;
}

void cs_init() {
    InitializeCriticalSection(&cs);
}

DWORD WINAPI cs_inc(LPVOID lpParam)
{
    EnterCriticalSection(&cs);
    for (int i = 0; i < COUNT; i++) {
        counter++;
        Sleep(10);
    }
    LeaveCriticalSection(&cs);
    return 2;
}

void cs_close() {
    DeleteCriticalSection(&cs);
}

void mutex_init()
{
    gmutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        L"My Mutex");             // named mutex
}
DWORD WINAPI mutex_inc(LPVOID lpParam)
{
    WaitForSingleObject(gmutex, INFINITE);
    for (int i = 0; i < COUNT; i++) {
        counter++;
        Sleep(10);
    }
    ReleaseMutex(gmutex);
    return 3;
}
void mutex_close()
{
    CloseHandle(gmutex);
}

void worker(LPTHREAD_START_ROUTINE func) {
    HANDLE athread[THREADCOUNT];
    DWORD dwThreadId[THREADCOUNT];

    for (int i = 0; i < THREADCOUNT; i++) {
        athread[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            func,       // thread function name
            NULL,          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadId[i]);   // returns the thread identifier 
        if (athread[i] == NULL) {
            printf("CreateThread error: %d\n", GetLastError());

            return;
        }
    }

    WaitForMultipleObjects(THREADCOUNT, athread, TRUE, INFINITE);

    for (int i = 0; i < THREADCOUNT; i++)
        CloseHandle(athread[i]);
}

int main()
{
    counter = 0;
    worker(inc);
    cout << "Increment counter: " << counter << endl;

    counter = 0;
    cs_init();
    worker(cs_inc);
    cs_close();
    cout << "Critical section counter: " << counter << endl;

    counter = 0;
    mutex_init();
    worker(mutex_inc);
    mutex_close();
    cout << "Mutex counter: " << counter << endl;
}
