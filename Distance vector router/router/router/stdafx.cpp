// stdafx.cpp : source file that includes just the standard includes
// router.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.hpp"
// variables
extern char name;
extern bool poisonReverse;
extern fd_set master; // contains all sockets 
extern fd_set read; // used when calling select

int readConfig(wstring foldername)
{
	wstring dir = L"..\\..\\proj2-skeleton\\";
	dir += foldername;

	if (!SetCurrentDirectory((LPCWSTR)dir.c_str()))
	{
		MessageBox(NULL, (LPCWSTR)L"Unable to open directory", (LPCWSTR)name, MB_OK);
		return -1;
	}
	string line;
	line =".cfg";
	line.insert(0, &name);
	ifstream config(line);
	if (!config.is_open())
	{
		MessageBox(NULL, (LPCWSTR)L"Unable to open config file", (LPCWSTR)name, MB_OK);
		return -1;
	}
	while (getline(config, line))
	{
		char *context = NULL;
		char *tok = strtok_s(&line[0], " ", &context);
		// first tok name of other router.
		char newName = *tok;
		routingEntry newEntry;
		// next is cost.
		tok = strtok_s(NULL, " ", &context);
		newEntry.distance = atoi(tok);
		// next is port to
		tok = strtok_s(NULL, " ", &context);
		newEntry.portTo = atoi(tok);
		// finally is port from
		tok = strtok_s(NULL, " ", &context);
		newEntry.portFrom = atoi(tok);

		//ADDED: need to give next hop, since there are direclty connected to router, next hop should be the new router itself. 
		newEntry.nextHop = newName;

		// lastly, insert new entry into table
		table[newName] = newEntry;
		
	}
	config.close();
	// reading router file now.
	ifstream rtr("routers");
	if (!rtr.is_open())
	{
		MessageBox(NULL, (LPCWSTR)L"Unable to open config file", (LPCWSTR)name, MB_OK);
		return -1;
	}
	while (getline(rtr, line))
	{
		char *context = NULL;
		char *tok = strtok_s(&line[0], " ", &context);
		// first tok name of other router.
		char newName = *tok;
		tok = strtok_s(NULL, " ", &context);
		tok = strtok_s(NULL, " ", &context);

		table[newName].basePort = atoi(tok);
		if (newName == name)
		{
			table[name].distance = 0;
			table[name].nextHop = 0;
			table[name].portFrom = 0;
			table[name].portTo = 0;
		}
	}

	return 1;
}