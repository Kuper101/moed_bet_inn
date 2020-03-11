#define _CRT_SECURE_NO_WARNINGS /* to suppress compiler warnings (VS 2010 ) */
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include<synchapi.h>
#include "Extract_Guests_and_Room_List.h"
#include"Hotel_Manager.h"

#define OUT 0
#define IN 1

/*oOoOoOoOoOoOoOoOoOoOoOoOoO Types oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
typedef struct {
	Range_t FilePosition;
	HANDLE MutexHandleA;
} ThreadFuncInput_t;

/*oOoOoOoOoOoOoOoOoOoOoOoOo Function Declarations oOoOoOoOoOoOoOoOoOoOoOoO*/

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);

HANDLE CreateMutexSimple(LPCTSTR MutexName);

DWORD LockUpdateAndUnlock(Bases_t Base, HANDLE MutexHandle);
/*oOoOoOoOoOoOoOoOoOoOoOoOo main() oOoOoOoOoOoOoOoOoOoOoOoO*/
void main()
{
	HANDLE ThreadHandles[NUM_OF_THREADS];
	ThreadFuncInput_t ThreadInput[NUM_OF_THREADS];
	DWORD ExitCode;
	DWORD WaitRes;
	int i;
	int ThreadInd;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOo Function Defenitions oOoOoOoOoOoOoOoOoOoOoOoO*/

/**
 * This function creates a thread by calling Win32 Api's CreateThread()
 * function, and setting some of the parameters to default value.
 *
 * parameters:
 * ----------
 * StartAddress - a pointer to the function that will be run by the thread.
 * ParameterPtr - a pointer to the parameter that will be supplied to the
 *                function run by the thread.
 * ThreadIdPtr - return argument: a pointer to a DWORD variable into which
 *               the function will write the created thread's ID.
 *
 * returns:
 * --------
 * On success, a handle to the created thread. On Failure - NULL.
 */
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr)
{
	return CreateThread(
		NULL,            /*  default security attributes */
		0,                /*  use default stack size */
		StartAddress,    /*  thread function */
		ParameterPtr,    /*  argument to thread function */
		0,                /*  use default creation flags */
		ThreadIdPtr);    /*  returns the thread identifier */
}

/**
 * This function uses the win32 api function CreateMutex() to create
 * a named mutex, without accepting the ownership for it (in other words,
 * without locking it).
 */
HANDLE CreateMutexSimple(LPCTSTR MutexName)
{
	return CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		MutexName);
}
	/**
	 * This function implements the basic lock, critical section, unlock
	 * part of the algorithm. In the critical section, this function
	 * writes into the global variable Histogram.
	 */
	DWORD SingleGuestManager(guest person, room* RoomsArray, HANDLE RoomMutexHandle)
	{
		BOOL ReleaseRes;

		DWORD WaitRes = WaitForSingleObject(RoomMutexHandle, INFINITE);

		if (WaitRes != WAIT_OBJECT_0)
		{
			if (WaitRes == WAIT_ABANDONED)
			{
				printf("Some thread has previously exited without releasing a mutex."
					" This is not good programming. Please fix the code.\n");
				return (ISP_MUTEX_ABANDONED);
			}
			else
				return(ISP_MUTEX_WAIT_FAILED);
		}

		/* ........Critical Section Start................ */
		if (RoomsArray[RoomEnum].CurrentCapacity != 0)
		{
			RoomsArray[RoomEnum].CurrentCapacity -= 1;
			person.Money -= RoomsArray[RoomEnum].Price;
			UpdateLogFilew(RoomsArray[RoomEnum].RoomName, person.Name, IN, day);
		}
		/* ........Critical Section End.................. */



		ReleaseRes = ReleaseMutex(RoomMutexHandle);

		if (ReleaseRes == FALSE)
			return (ISP_MUTEX_RELEASE_FAILED);

		return (ISP_SUCCESS);
	}