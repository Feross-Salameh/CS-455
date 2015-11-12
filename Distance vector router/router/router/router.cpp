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
	readConfig(L"test1");

    return 0;
}

