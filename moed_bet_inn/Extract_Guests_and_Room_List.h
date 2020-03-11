#ifndef EXTRACT_GUESTS_AND_ROOM_LIST_H
#define EXTRACT_GUESTS_AND_ROOM_LIST_H
#define GUEST_AND_ROOM_NAME_LENGTH 20
#define MAX_LINE_LENGTH 50
#define MAX_ROOMS_IN_HOTEL 5
#define MAX_GUESTS_IN_HOTEL 15
#define NUM_OF_PARAMETERS_FOR_GUEST 2
#define NUM_OF_PARAMETERS_FOR_ROOM 3
#define ERROR_CODE -1
#include <Windows.h>
#include <stdbool.h>




typedef struct _room {
	char room_name[GUEST_AND_ROOM_NAME_LENGTH];
	int price_per_night;
	int current_capacity;
}room;

typedef struct _guest {
	char guest_name[GUEST_AND_ROOM_NAME_LENGTH];
	int money_left;
	bool is_guest_in_room;
	bool checkedout;
	char roomype[GUEST_AND_ROOM_NAME_LENGTH];
	room* room;

}guest;

typedef struct _ThreadFuncInput_t {
	guest *guest;
	HANDLE MutexHandle;
	int day;
	char path_to_file[MAX_PATH];

} ThreadFuncInput_t;	

typedef enum {
	ISP_SUCCESS,
	ISP_FILE_OPEN_FAILED,
	ISP_FILE_SEEK_FAILED,
	ISP_FILE_READING_FAILED,
	ISP_FTELL_FAILED,
	ISP_MUTEX_OPEN_FAILED,
	ISP_MUTEX_CREATE_FAILED,
	ISP_MUTEX_WAIT_FAILED,
	ISP_MUTEX_ABANDONED,
	ISP_MUTEX_RELEASE_FAILED,
	ISP_ILLEGAL_LETTER_WAS_READ
} ErrorCode_t;

int get_rooms_and_guests_from_file(char* path_to_hotel_main_file);
void print_guest(guest *guest, int i);
void assert_all_functions();

#endif
