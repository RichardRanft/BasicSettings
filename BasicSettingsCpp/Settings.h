#pragma once
#include <exception>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <utility>
#include <string>
#include <vector>

class CSettings
{
private:
	std::list<std::string> m_iniText;
	std::map<std::string, std::list<std::pair<std::string, std::string>>> m_attributeList;
	std::string m_fileName;
	bool m_dirty;

	void readFile();
	std::list<std::string> splitLine(std::string line, char* delim);

public:
	CSettings();
	CSettings(std::string fileName);
	~CSettings();

	std::list<std::string> GetConfiguration()
	{
		return m_iniText;
	}

	const std::map<std::string, std::list<std::pair<std::string, std::string>>> GetAttributes()
	{
		return m_attributeList;
	}

	const std::list<std::pair<std::string, std::string>> GetSection(std::string sectionName);
	bool LoadSettings();
	bool SaveSettings();
	bool Set(std::string sectionName, std::string key, std::string value);
};

