#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//	Made this so target for log and error can be changed later as needed
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_>
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\>
namespace OutputHandler
{
	inline void ErrorOut(const string &message, int line = -1, const string &file = "")
	{
		std::string s = message + " ";
		std::cerr << "ERROR::" << message << " ";
		if (line > -1)
		{
			std::cerr << " || At Line : " << line << " ";
			s += to_string(line) + " ";
		}
		if (!file.empty())
		{
			std::cerr << "|| In File : " << file << " ";
			s += file + " ";
		}
		std::cerr << std::endl;

		MessageBox(nullptr, s.c_str(), nullptr, MB_ICONEXCLAMATION);
	}

	inline void Log(const string &message)
	{
		std::cout << message << std::endl;
	}
}