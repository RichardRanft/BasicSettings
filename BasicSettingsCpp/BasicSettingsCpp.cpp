// BasicSettingsCpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Settings.h"

int _tmain(int argc, _TCHAR* argv[])
{
	std::string filename = "settings.ini";
	CSettings* settings = new CSettings(filename);
	if (!settings->LoadSettings())
	{
		std::cout << "Unable to load settings.ini";
		return 1;
	}

	std::map<std::string, std::list<std::pair<std::string, std::string>>> attribmap = settings->GetAttributes();
	std::map<std::string, std::list<std::pair<std::string, std::string>>>::iterator itr = attribmap.begin();
	std::map<std::string, std::list<std::pair<std::string, std::string>>>::iterator end = attribmap.end();
	for (; itr != end; itr++)
	{
		std::string def = (*itr).first.c_str();
		std::cout << def << std::endl;
		std::list<std::pair<std::string, std::string>> section = settings->GetSection(def);
		for (std::list<std::pair<std::string, std::string>>::const_iterator it = section.begin(); it != section.end(); it++)
		{
			std::cout << (*it).first << " = " << (*it).second << std::endl;
		}
	}
	return 0;
}

