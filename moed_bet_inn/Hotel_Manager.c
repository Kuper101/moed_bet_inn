/*
Project: TestManager
Description: This module does the guests to rooms allocation and tracking

*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <Windows.h>
#include "Hotel_Manager.h"


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


/*
Description: The function is reponsible for updating the room_loog every time a guest is enterin or leaving a room
Parameters: guest *guest_list - an array of guest type struct.
			int guest_index - the index of the current guest in the array.
			bool in - if in = true the guests is entering the room, otherwise, he's exiting it.
			int day - represents which day is it.
			char *path_to_room_loog - path to room_loog file.
Returns:	Retruns -1 if there was any kind of error during the process, 0 otherwise.
*/
int update_room_log(guest guest, bool in, int day, char *path_to_room_loog) {
	FILE *room_log;
	char line_to_put_in_file[MAX_LINE_LENGTH], day_str[MAX_LENGTH_OF_DAY_STRING];
	int error_handler = 0;
	strcpy(line_to_put_in_file, guest.roomype);
	strcat(line_to_put_in_file, " ");
	strcat(line_to_put_in_file, guest.guest_name);
	strcat(line_to_put_in_file, " ");
	if (in == true) {
		strcat(line_to_put_in_file, "IN");
	}
	else{ strcat(line_to_put_in_file, "OUT"); }
	strcat(line_to_put_in_file, " ");
	_itoa(day, day_str, 10);
	strcat(line_to_put_in_file, day_str);
	strcat(line_to_put_in_file, "\n");
	room_log = fopen(path_to_room_loog, "a");
	if (room_log == NULL) {
		printf("Error occured while trying to open a file! Exiting");
		error_handler = -1;
	}
	fputs(line_to_put_in_file, room_log);
	fclose(room_log);
	return(error_handler);
}






/*
Description: The thread is reponsible for finding a room for all the guests according to their budget.
Parameters: guest *guest_list - an array of guest type struct.
			room *rooms_list - an array of room type struct.s
			int guest_index - the index of the current guest in the array.
Returns:	exit code
*/


DWORD singleGuestManager(LPVOID Argument)
{
	BOOL ReleaseRes;
	ThreadFuncInput_t* guest_and_room = (ThreadFuncInput_t*)Argument;

	DWORD WaitRes = WaitForSingleObject(guest_and_room->MutexHandle, INFINITE);

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
	if (guest_and_room->guest->room->current_capacity > 0)
	{
		guest_and_room->guest->room->current_capacity -= 1;
		
		guest_and_room->guest->is_guest_in_room = true;
		update_room_log((*guest_and_room->guest), true, guest_and_room->day, guest_and_room->path_to_file);
	}
	/* ........Critical Section End.................. */
	ReleaseRes = ReleaseMutex(guest_and_room->MutexHandle);

	if (ReleaseRes == FALSE)
		return (ISP_MUTEX_RELEASE_FAILED);

	return (ISP_SUCCESS);

}
/*
Description: The function is reponsible for taking guests out of rooms and
			 updating the room_log.
Parameters: guest *guest_list - an array of guest type struct.
			int num_of_guests_in_hotel - the nuber of total guests in hotel, including the ones waiting for a room.
			room *rooms_list - an array of room type struct.
			char *path_to_hotel_main_directory - path to main input directory.
Returns:	Retruns -1 if there was any kind of error during the process, 0 otherwise.
*/
DWORD get_guest_out_of_room(LPVOID Argument)
{	
	BOOL ReleaseRes;
	ThreadFuncInput_t* guest_and_room = (ThreadFuncInput_t*)Argument;
	int i = 0;

	DWORD WaitRes = WaitForSingleObject(guest_and_room->MutexHandle, INFINITE);

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
	guest_and_room->guest->is_guest_in_room = false;
	guest_and_room->guest->room->current_capacity += 1;
	update_room_log((*guest_and_room->guest), false, guest_and_room->day, guest_and_room->path_to_file);
	/* ........Critical Section End.................. */
	ReleaseRes = ReleaseMutex(guest_and_room->MutexHandle);

	if (ReleaseRes == FALSE)
		return (ISP_MUTEX_RELEASE_FAILED);

	return (ISP_SUCCESS);

}

/*
Description: The function is reponsible for finding a room for all the guests according to their budget.
Parameters: guest *guest_list - an array of guest type struct.
			int num_of_guests_in_hotel - the nuber of total guests in hotel, including the ones waiting for a room.
			room *rooms_list - an array of room type struct.
Returns:	NONE
*/
void find_room_for_guests(guest *guest_list, int num_of_guests_in_hotel, room *rooms_list) {
	int guest_index = 0, i = 0;
	bool found = false;
	for (guest_index = 0; guest_index < num_of_guests_in_hotel; guest_index++) {
		while (!found) {
			if (guest_list[guest_index].money_left % rooms_list[i].price_per_night == 0) {
				strcpy(guest_list[guest_index].roomype, rooms_list[i].room_name);
				found = true;
				guest_list[guest_index].room = &rooms_list[i];
			}
			i++;
		}
		i = 0;
		found = false;
	}
}
/*
Description: The function is reponsible for taking guests in and out of rooms, checking for available rooms, and 
		     updating the room_log.
Parameters: guest *guest_list - an array of guest type struct.
			int num_of_guests_in_hotel - the nuber of total guests in hotel, including the ones waiting for a room.
			room *rooms_list - an array of room type struct.
			char *path_to_hotel_main_directory - path to main input directory.
Returns:	Retruns -1 if there was any kind of error during the process, 0 otherwise.
*/
int main_hotel_manager(guest *guest_list, int num_of_guests_in_hotel, room *rooms_list,char *path_to_hotel_main_directory) 
{
	
	bool seasonOver = false;
	char path_to_rooms_log_file[MAX_PATH];
	int error_handler = 0, day = 0, guest_index = 0,  num_of_checkedout_guests = 0;
	DWORD WaitRes;
	DWORD* p_thread_ids_leave;
	DWORD* p_thread_ids_find;
	HANDLE* p_thread_handles_leave;
	HANDLE* p_thread_handles_find;
	HANDLE MutexHandle = NULL;
	ThreadFuncInput_t ThreadFuncInput;
	
	strcpy(path_to_rooms_log_file, path_to_hotel_main_directory);
	strcat(path_to_rooms_log_file, "\\roomLog.txt");
	find_room_for_guests(guest_list, num_of_guests_in_hotel, rooms_list);
	

	p_thread_ids_leave = (DWORD*)malloc(num_of_guests_in_hotel * sizeof(DWORD));
	p_thread_ids_find = (DWORD*)malloc(num_of_guests_in_hotel * sizeof(DWORD));

	p_thread_handles_leave = (HANDLE*)malloc(num_of_guests_in_hotel * sizeof(HANDLE));
	p_thread_handles_find = (HANDLE*)malloc(num_of_guests_in_hotel * sizeof(HANDLE));
	MutexHandle = CreateMutexSimple(MutexHandle);

	while (!seasonOver) {
		day++;

		for (guest_index = 0; guest_index < num_of_guests_in_hotel; guest_index++)
		{
			
			ThreadFuncInput.guest = &guest_list[guest_index];
			ThreadFuncInput.MutexHandle = MutexHandle;
			ThreadFuncInput.day = day;
			strcpy(&(ThreadFuncInput.path_to_file), &path_to_rooms_log_file);
			if (!guest_list[guest_index].checkedout)
				{
				// If the guest is out of cash
				if (guest_list[guest_index].money_left == 0 )
				{
					get_guest_out_of_room(guest_list, rooms_list, guest_index);
					p_thread_handles_leave[guest_index] = CreateThreadSimple((LPTHREAD_START_ROUTINE)get_guest_out_of_room, &ThreadFuncInput, &p_thread_ids_leave[guest_index]);
					WaitRes = WaitForSingleObject(p_thread_handles_leave[guest_index], INFINITE);
					if (WAIT_FAILED == WaitRes)
					{
						printf("Error when waiting");
						return ERROR_CODE;
					}
					guest_list[guest_index].checkedout = true;
					num_of_checkedout_guests++;

				}
				else
				{
					if (guest_list[guest_index].is_guest_in_room == false )
					{
						//Find the man a room for crying out loud
						p_thread_handles_find[guest_index] = CreateThreadSimple((LPTHREAD_START_ROUTINE)singleGuestManager, (LPVOID)&ThreadFuncInput, &p_thread_ids_find[guest_index]);
						WaitRes = WaitForSingleObject(p_thread_handles_find[guest_index], INFINITE);
						if (WAIT_FAILED == WaitRes)
						{
							printf("Error when waiting");
							return ERROR_CODE;
						}
						if (guest_list[guest_index].is_guest_in_room == true)
							guest_list[guest_index].money_left -= guest_list[guest_index].room->price_per_night;

					}
					else
						guest_list[guest_index].money_left -= guest_list[guest_index].room->price_per_night;

				}
			}
		}
		if (error_handler == ERROR_CODE) {
			return ERROR_CODE;
		}
		seasonOver = (num_of_checkedout_guests == num_of_guests_in_hotel); // check if there are guests still waiting
	}

	free(p_thread_handles_find);
	free(p_thread_handles_leave);
	free(p_thread_ids_find);
	free(p_thread_ids_leave);
	if (!(error_handler == ERROR_CODE))
		return(day);
	else
		return(ERROR_CODE);
}




