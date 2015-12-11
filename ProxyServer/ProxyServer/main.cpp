#include "main.hpp"

int main()
{
	setupLisSok();
	selectLoop();
	return 0;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	HANDLE hStdout;
	SOCKET *recSok = (SOCKET *)lpParam;

	// Make sure there is a console to receive output results. 
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE)
		return 1;
	messageHandler(*recSok, "HTML");

	return 0;
}