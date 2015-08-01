#include "stdafx.h"
#include "Settings.h"


CSettings::CSettings()
{
	m_fileName = "settings.ini";
	m_dirty = false;
}

CSettings::CSettings(std::string filename)
{
	m_fileName = filename.c_str();
	m_dirty = false;
}

CSettings::~CSettings()
{
}

const std::list<std::pair<std::string, std::string>> CSettings::GetSection(std::string sectionName)
{
	return m_attributeList[sectionName];
}

bool CSettings::LoadSettings()
{
	bool success = false;
	try
	{
		readFile();
		std::string line = "";
		int index = 0;

		std::string section = "[Default]";
		std::list<std::pair<std::string, std::string>> sectionValues;

		for (std::list<std::string>::const_iterator itr = m_iniText.begin(); itr != m_iniText.end(); itr++)
		{
			line = (*itr);
			std::size_t fPos = line.find(';');
			if (fPos != std::string::npos) // ini file comment
			{
				index = fPos;
				std::size_t qPos = line.find('\"');
				if (qPos != std::string::npos)
				{
					// okay, tricky tricky might be a ; inside of "quoted;need this here"
					if (fPos > qPos)
					{
						std::string afterQuote = line.substr(qPos + 1, line.size());
						index = qPos + 1 + afterQuote.find('\"');
					}
					index++;
				}
				if (fPos < line.size())
					line = line.substr(0, index);
			}
			if (line.size() < 1)
				continue;

			if (line.find('[') == 0)
			{
				// check to see if this [Section] exists.  If so, add new entries to it
				std::map<std::string, std::list<std::pair<std::string, std::string>>>::const_iterator sectionEntry = m_attributeList.find(line);
				if (sectionEntry != m_attributeList.end())
				{
					// section exists, so copy it into the sectionValues pointer and remove it from the attributes list
					std::list<std::pair<std::string, std::string>> temp;
					for (std::list<std::pair<std::string, std::string>>::const_iterator itr = (*sectionEntry).second.begin(); itr != (*sectionEntry).second.end(); itr++)
						temp.push_back((*itr));
					section = line;
					m_attributeList.erase(sectionEntry);
					sectionValues = temp;
					continue;
				}

				// otherwise, create a new [Section]
				section = line;
				sectionValues.clear();
				m_attributeList.insert(m_attributeList.end(), std::pair<std::string, std::list<std::pair<std::string, std::string>>>(section, sectionValues));
				continue;
			}

			std::list<std::string> parts = splitLine(line, "=");
			if (parts.size() < 2)
			{
				std::cout << "Invalid key/value pair in ini file: " << line << std::endl;
				continue;
			}
			std::list<std::string>::iterator it = parts.begin();
			std::string key = (*it++);
			std::string val = (*it);
			
			std::pair<std::string, std::string> attribute(key, val);

			// check for existing attributes.  If we're trying to add the same attribute again we remove the
			// old and add the new - so later definitions override earlier ones.
			bool exists = false;
			for (std::list<std::pair<std::string, std::string>>::iterator attr = sectionValues.begin(); attr != sectionValues.end(); attr++)
			{
				if ((*attr).first.compare(key) == 0)
				{
					exists = true;
					break;
				}
			}
			if (!exists)
				m_attributeList[section].push_back(attribute);
		}

		success = true;
	}
	catch (std::exception ex)
	{
		std::cout << "Unable to parse settins: " << ex.what() << std::endl;
		success = false;
	}

	return success;
}

std::list<std::string> CSettings::splitLine(std::string line, char* delim)
{
	std::list<std::string> parts;
	int pos = line.find(delim);
	std::string first = line.substr(0, pos);
	std::string second = line.substr(pos + 1, line.size() - 1);
	parts.push_back(first);
	parts.push_back(second);
	return parts;
}

bool CSettings::SaveSettings()
{
	std::ofstream file(m_fileName);
	try
	{
		std::map<std::string, std::list<std::pair<std::string, std::string>>>::iterator itr = m_attributeList.begin();
		std::map<std::string, std::list<std::pair<std::string, std::string>>>::iterator end = m_attributeList.end();
		for (; itr != end; itr++)
		{
			std::string def = (*itr).first.c_str();
			file << def << std::endl;
			std::list<std::pair<std::string, std::string>> section = GetSection(def);
			for (std::list<std::pair<std::string, std::string>>::const_iterator it = section.begin(); it != section.end(); it++)
			{
				file << (*it).first << "=" << (*it).second << std::endl;
			}
		}
		file.close();
		return true;
	}
	catch (std::exception e)
	{
		std::cout << "CSettings::SaveSettings() failed - " << e.what() << std::endl;
	}
	return false;
}

bool CSettings::Set(std::string sectionName, std::string key, std::string value)
{
	std::list<std::pair<std::string, std::string>> *section = &m_attributeList[sectionName];
	std::list<std::pair<std::string, std::string>>::const_iterator it = (*section).begin();
	bool found = false;
	for (; it != (*section).end(); it++)
	{
		if ((*it).first.compare(key) == 0)
		{
			found = true;
		}
		if (found)
			break;
	}
	std::pair<std::string, std::string> newpair(key, value);
	if (found)
	{
		(*section).erase(it);
		(*section).push_back(newpair);
		return true;
	}
	else
	{
		(*section).push_back(newpair);
		return true;
	}
	return false;
}

void CSettings::readFile()
{
	std::ifstream file(m_fileName);
	try
	{
		std::string line = "";
		char str[2048];
		while (file)
		{
			file.getline(str, 2048);
			line = str;
			m_iniText.push_back(line);
		}
	}
	catch (std::exception e)
	{
		std::cout << "CSettings::readFile() failed - " << e.what() << std::endl;
	}
}
