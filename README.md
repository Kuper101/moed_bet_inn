# moed_bet_inn
School project for introduction to system programming course.

The program will maxsimize the profit for the hotel by getting as much guests as possible into an hotel room.
It does so by constantly going over the guests and rooms list and matching guest to a room' according to the guest's budget.

The program uses inter thread communication and mutexs in order to manage the hotel properly such that only one guest can 
stay in each room at once.

## How to compile and run
Initialize the program with the following arguments: 
- rooms.txt - a file containing the room names, price per night and available rooms.
- names.txt - a file containing the guests names and budget for the intire vacation

The output of the program will be a room_log file, each line in it will give the following info:
room_name, guest_name, IN/OUT (has the guest just entered the room or is he just leaving, day of entering/leaving

## IDE
Written and compiled in Visual Studio 2017
