// router.cpp : Defines the entry point for the console application.
//

#include "stdafx.hpp"
// variables
// NOTE: remeber to add 'extern'
extern char name;
extern bool poisonReverse;
extern fd_set masterRead; // contains all sockets for reading
extern fd_set masterWrite; // contains all sockets for writing
extern fd_set read; // used when calling select
extern fd_set write; // used when calling select
extern map<char, routingEntry> table; //this will contain the distance vector routing table.
int main(int argc, char *argv[])
{
	if (argc == 4)
	{
		name = argv[3][0];
		cout << "started router " << name << endl;
		wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
		wstring testFolder = converter.from_bytes(argv[2]);
		readConfig(testFolder);
		cout << "Reading config files successful" << endl;
		if (!strcmp("-p", argv[1]))
		{
			poisonReverse = TRUE;
		}
	}
	else
	{
		name = argv[2][0];
		cout << "started router " << name << endl;
		wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
		wstring testFolder = converter.from_bytes(argv[1]);
		readConfig(testFolder);
		cout << "Reading config files successful" << endl;
	}
	setupSockets();
	while (1) // connect to sockets....
	{
		
		memcpy(&write, &masterWrite, sizeof(masterWrite));
		memcpy(&read, &masterRead, sizeof(masterRead));
		timeval timeout = timeval();
		timeout.tv_sec = 3;
		//cout << "calling select..." << endl;
		int ret = select(0, &read, NULL,  NULL, &timeout);
		if (ret < 0)
			cout << "error on select call: " << WSAGetLastError() << endl;
		if (ret == 0)
			sendRoutTableAll();
		else
			processSelect(ret);
		resetFD();
	}

	char inp;
	cin >> inp;
    return 0;
}

