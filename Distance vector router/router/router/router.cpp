// router.cpp : Defines the entry point for the console application.
//

#include "stdafx.hpp"
// variables
// NOTE: remeber to add 'extern'
extern char name;
extern bool poisonReverse;
extern fd_set master; // contains all sockets 
extern fd_set read; // used when calling select
extern map<char, routingEntry> table; //this will contain the distance vector routing table.
int main(int argc, char *argv[])
{
	name = argv[1][0];
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	wstring testFolder = converter.from_bytes(argv[2]);
	readConfig(testFolder);
	cout << "Reading config files successful" << endl;

	setupSockets();


    return 0;
}

