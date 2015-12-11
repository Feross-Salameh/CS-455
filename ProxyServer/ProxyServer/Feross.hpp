#ifndef FEROSS_HPP
#define FEROSS_HPP

#pragma once
#include "stdHeaders.hpp"


//function prototypes

/*
	initilize/ setup the listening socket to default address and default port listed in stdHeaders.
	returns 1 is sucess, less than 0 for failure.

*/
int setupLisSok();


/*
	main loop for where parent process will reside. This will call select, fork a child process if nessessary, clean up child process using wait
	From here, this process should not return unless error has happened
*/
int selectLoop();

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void ErrorHandler(LPTSTR lpszFunction);

#endif // FEROSS_HPP