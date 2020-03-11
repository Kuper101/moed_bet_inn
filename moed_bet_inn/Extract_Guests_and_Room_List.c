//Authors: Yuval Kuperberg 200832525
//		   Yaniv Boneh 201444403
//Project: Moed Bet inn.
//Description: This module extracts the required data from the input files


#include "Extract_Guests_and_Room_List.h"
#include "Hotel_Manager.h"
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <string.h>
#include <assert.h>

/*
Description: This function takes a string of words and decomposes it words.
Parameters: char* string_to_split - a string of words.
			char **seperated_words - an array strings, to be filled with the seperated words.
Returns:	NONE.
*/
void split_string_by_space(char *string_to_split, char **seperated_words) {
	int num_of_words = 0;
	while (*string_to_split != '\0') {
		while (*string_to_split == ' ') {
			string_to_split++;
			if (*string_to_split == '\0') {
				return;
			}
		}
		seperated_words[num_of_words] = string_to_split;
		num_of_words++;
		while ((*string_to_split != ' ') && (*string_to_split != '\0')) {
			string_to_split++;
			if (*string_to_split == '\0') {
				return;
			}
		}
		*string_to_split = '\0';
		string_to_split++;
	}
	return;
}
/*
Description: The function is responsible for updating the parameters of all the rooms.
Parameters: char* room_line_in_file - a string that contains all the information about the room.
			room *rooms_list - an array of room type struct.
			int num_of_roomypes_available - the number of room types in the hotel.
Returns:	NONE.
*/
void create_new_room(char* room_line_in_file, room *rooms_list, int num_of_roomypes_available) {
	char *line_ptr = (char*)malloc((strlen(room_line_in_file)+1) * sizeof(char));
	char *split_string[NUM_OF_PARAMETERS_FOR_ROOM];
	strcpy(line_ptr, room_line_in_file);
	if ((rooms_list != NULL) && (line_ptr != NULL)) {
		split_string_by_space(line_ptr, split_string);
		strcpy(rooms_list[num_of_roomypes_available].room_name, split_string[0]);
		rooms_list[num_of_roomypes_available].price_per_night = atoi(split_string[1]);
		rooms_list[num_of_roomypes_available].current_capacity = atoi(split_string[2]);
	}
	else {
		printf("Memory allocation failed while trying to create a new guest! Exiting\n");
	}
	free(line_ptr);
	return;
}
/*
Description: The function is responsible for updating the parameters of the new guest.
Parameters: char* guest_line_in_file - a string that contains all the information about the guest.
			guest *guest_list - an array of guest type struct.
			int num_of_guests_in_hotel - the nuber of total guests in hotel, including the ones waiting for a room.
Returns:	NONE.
*/
void create_new_guest(char *guest_line_in_file, guest *guest_list, int guest_idx) {
	char *line_ptr = (char*)malloc((strlen(guest_line_in_file)+1) * sizeof(char));
	char *split_string[NUM_OF_PARAMETERS_FOR_GUEST]; 
	strcpy(line_ptr, guest_line_in_file);
	if ((guest_list != NULL)&&(line_ptr!=NULL)) {
		split_string_by_space(line_ptr, split_string);
		strcpy((guest_list[guest_idx]).guest_name, split_string[0]);
		(guest_list[guest_idx]).is_guest_in_room = false;
		(guest_list[guest_idx]).money_left = atoi(split_string[1]);
		(guest_list[guest_idx]).checkedout = false;
	}
	else{ printf("Memory allocation of new guest failed! Exiting\n"); 
	}
	free(line_ptr);
	return;
}

void free_guests_and_rooms_lists(guest **guest_list, room **rooms_list, int num_of_guests_in_hotel,
	int num_of_roomypes_occupied) {
	int i = 0;
	for (i = 0; i < num_of_guests_in_hotel; i++) {
		free(guest_list[i]);
	}
	for (i = 0; i < num_of_roomypes_occupied; i++) {
		free(rooms_list[i]);
	}
}

void print_guest(guest *guest, int i) {
	printf("Guest num %d:\n", i);
	printf("%s\n", guest[i].guest_name);
	printf("%d\n", guest[i].money_left);
	printf("%s\n", guest[i].roomype);
	printf(guest[i].is_guest_in_room ? "true\n\n" : "false\n\n");
}
/*
Description: The function is responsible for creating arrays of guests and rooms to work with. It extracts the 
			parameters for guests and rooms from the input files and turn them into arrays of structs.
Parameters: char* path_to_hotel_main_file - path to main input directory.
Returns: Retruns -1 if there was any kind of error during the process, 0 otherwise.
*/
int get_rooms_and_guests_from_file(char* path_to_hotel_main_file) {
	FILE *names_and_rooms_file_handler;
	guest guest_list[MAX_GUESTS_IN_HOTEL];
	room rooms_list[MAX_ROOMS_IN_HOTEL];
	int guest_idx = 0;
	char *current_line_in_file, path_to_names_file[MAX_PATH], path_to_rooms_file[MAX_PATH];
	int error_handler = 0, num_of_guests_in_hotel = 0, num_of_roomypes_occupied = 0;
	strcpy(path_to_names_file, path_to_hotel_main_file);
	strcat(path_to_names_file, "\\names.txt");
	strcpy(path_to_rooms_file, path_to_hotel_main_file);
	strcat(path_to_rooms_file, "\\rooms.txt");
	current_line_in_file = (char*)malloc((MAX_LINE_LENGTH + 1) * sizeof(char));
	names_and_rooms_file_handler = fopen(path_to_names_file, "r");
	while (fgets(current_line_in_file, MAX_LINE_LENGTH, names_and_rooms_file_handler) != NULL) {
		create_new_guest(current_line_in_file, guest_list, guest_idx);
		num_of_guests_in_hotel++;
		guest_idx++;
		//print_guest(guest_list, num_of_guests_in_hotel);
	}
	fclose(names_and_rooms_file_handler);
	names_and_rooms_file_handler = fopen(path_to_rooms_file, "r");
	while (fgets(current_line_in_file, MAX_LINE_LENGTH, names_and_rooms_file_handler) != NULL) {
		create_new_room(current_line_in_file, rooms_list, num_of_roomypes_occupied);
		num_of_roomypes_occupied++;
	}
	fclose(names_and_rooms_file_handler);
	free(current_line_in_file);
	error_handler = main_hotel_manager(guest_list, num_of_guests_in_hotel, rooms_list, path_to_hotel_main_file);
	return(error_handler);
}
