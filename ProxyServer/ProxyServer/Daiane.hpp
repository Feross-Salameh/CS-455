#ifndef DAIANE_HPP
#define DAIANE_HPP

#pragma once
#include "stdHeaders.hpp"

void log();
void logging(struct sockaddr_in *sockaddr_in);
void entrylogformat(char *stringlogging, struct sockaddr_in *sockaddr_in, char *url, int size);


#endif // DAIANE_HPP

