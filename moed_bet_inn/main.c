/*
Project: Moed Bet inn.
Description: Main function

*/

#include "Extract_Guests_and_Room_List.h"
#include <stdio.h>


int main(int argc, char* argv[]) {
	int pass_fail_flag = 0;

	pass_fail_flag = get_rooms_and_guests_from_file(argv[1]);
	if (pass_fail_flag	!= ERROR_CODE) {
		printf("Number of days:%d\n", pass_fail_flag);	}
	else {
		printf("An error accuerd\n");
	}
	return(pass_fail_flag);
}