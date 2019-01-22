#include <iostream>
#include <cstddef>	
#include <cstring>
#include <fstream>
#include <vector>
#include <unistd.h>
#include "cmd.hpp"

void parse_command(string command)
{
	if (strlen(command.c_str()) > 0)
	{
		ushort result = 0;
		if (strncmp(command.c_str(), "help", 4) == 0)
		{
			result = help();
			if (result == 0)
				return;
		}
		if (strncmp(command.c_str(), "add", 3) == 0)
		{
			result = add(command.c_str());
			if (result == 0)
				return;
		}
		else if (strncmp(command.c_str(), "count", 5) == 0)
		{
			cout << "Count: " << DB::Instance().count << std::endl;
			return;
		}
		else if (DB::Instance().count == 0)
		{
			cout << "DB is free" << std::endl;
			return;
		}
		else if (strncmp(command.c_str(), "edit", 4) == 0)
		{
			result = edit(command.c_str());
			if (result == 0)
				return;
		}
		else if (strncmp(command.c_str(), "del", 3) == 0)
		{
			result = del(command.c_str());
			if (result == 0)
				return;
		}
		else if (strncmp(command.c_str(), "find", 4) == 0)
		{
			result = find(command.c_str());
			return;
		}
		else if (strncmp(command.c_str(), "list", 4) == 0)
		{
			result = list(command.c_str());
			if (result == 0)
				return;
		}

		else
			cout << "Wrong command or result.\nPlease use `help` command or communicate with author." << std::endl;
	}
	return;
}

int main(int args, char *argv[])
{
	DB::Instance().connect();
	DB::Instance().getIF();

	if (args > 1)
	{
		char ch_command[512];
		sprintf(ch_command, "%s", argv[1]);
		for (ushort i = 2; i < args; i++)
		{
			sprintf(ch_command, "%s %s", ch_command, argv[i]);
		}
		std::string command = ch_command;
		if (command.find("-it") == std::string::npos)
		{
			std::cout << "CLI-mode. Your command = \"" << command << "\"" << std::endl;
			parse_command(command);
			DB::Instance().closeIF();
			return 0;
		}
		else
		{
			std::cout << "Welcome to Interactive Mode!!!" << std::endl
					  << "For exit press 'q' or use Unix signals" << std::endl;
			std::string command;
			for (;;)
			{
				cout << "> ";
				std::getline(std::cin, command);
				if (strcmp(command.c_str(), "q") == 0)
					break;
				cout << "Command: " << command << std::endl;
				parse_command(command);
				command = "";
			}
			DB::Instance().closeIF();
			return 0;
		}
	}
	help();
	return 1;
}
