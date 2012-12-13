/*
	Copyright (C) 2011-2012 Alican Sekerefe

	TeamTwo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TeamTwo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Contact: projectteamtwo@gmail.com
*/

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "Util/Logger.h"

class ConfigWriter;

typedef std::map<std::string,std::string> settingMap;

/*	Config Reader

	This class is responsible for reading configuration files
	which are coded in the right plain text format. During the
	read operation, it creates a configuration settings tree
	by using maps. When a field attribute is requested, it uses
	the configuration type title and its field, then returns the
	requested value. ConfigReader is able to return four different
	variable types: int, double, std::string and bool. 

	Note: this class is generally used with ConfigWriter and it
		  can be imported.

*/

class ConfigReader
{
public:
	//Constructor of the class. takes the file name that is going
	//to be read as the configuration file
    ConfigReader(std::string filename);
	//returns the field attribute as std::string by cosidering the title and field(subtitle)
    std::string getFieldValueAsString(std::string title,std::string subtitle);
	//returns the field attribute as integer by considering the title andits subtitle
    int getFieldValueAsInteger(std::string title,std::string subtitle);
	//returns the field attribute as double by considering the title andits subtitle
    double getFieldValueAsDouble(std::string title,std::string subtitle);
	//returns the field attribute as boolean by considering the title andits subtitle
    bool getFieldValueAsBool(std::string title,std::string subtitle);
	//reads the file. when success, it returns true. otherwise, false is returned
    bool readFile();
	//resets the configuration map and reads the given file one more time
    bool reload();
	//pushes the file and its object into a static map which can be going to be used
	//without reinitializing it. useful for calling ConfigReader from multiple places
    void pushConfigFile(std::string configName);
	//returns the ConfigReader object which was pushed before
    static ConfigReader* getConfigFile(std::string configName);

    friend class ConfigWriter;
private:
    std::string filename;
    std::ifstream is;
    std::map<std::string,settingMap> titleMap;
    static std::map<std::string, ConfigReader*> ReaderMap;
};

#endif