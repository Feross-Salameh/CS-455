// router.cpp : Defines the entry point for the console application.
//

#include "stdafx.hpp"
// variables
// NOTE: remeber to add variable to stdaf.cpp plus 'extern'
char name;
bool poisonReverse = false;
fd_set master; // contains all sockets 
fd_set read; // used when calling select

int main(int argc, char *argv[])
{

	name = argv[1][0];

	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	wstring testFolder = converter.from_bytes(argv[2]);
	readConfig(testFolder);
    return 0;
}

