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

#include "ConfigReader.h"

std::map<std::string,ConfigReader*> ConfigReader::ReaderMap;

ConfigReader::ConfigReader(std::string filename)
{
    this->filename=filename;
}

bool ConfigReader::readFile()
{
    bool result=false;
	std::ifstream is(filename.c_str(),std::ios::in);
    if(is.is_open())
    {
		Logger::getSingleton()->addLine("ConfigReader: reading config file");
        result=true;
        char buffer[1000];
        is.getline(buffer,1000);
        bool readHeader=false;
        while(!is.eof())
        {
            if(buffer[0]=='[')
            {
                int index=0;
                std::string parameter;
                std::string str(buffer);
                std::string headerName=str.substr(1,is.gcount()-3);
                settingMap smap;
                while(!is.eof())
                {
                    is.getline(buffer,1000);
                    if(buffer[0]!='[' && !is.eof())
                    {
                        parameter=std::string(buffer);
                        index=parameter.find("=");
                        if(index>-1)
                            smap.insert(std::pair<std::string,std::string>(parameter.substr(0,index),parameter.substr(index+1)));
                    }
                    else
                    {
                        readHeader=true;
                        titleMap.insert(std::pair<std::string,settingMap>(headerName,smap));
                        break;
                    }
                }
            }
            if(!readHeader)
                is.getline(buffer,1000);
            else
            {
                readHeader=false;
            }
        }
    }

    is.close();
    return result;
}

bool ConfigReader::reload()
{
    titleMap.clear();
    return readFile();
}

std::string ConfigReader::getFieldValueAsString(std::string title, std::string subtitle)
{
    std::string value="NULL";
    if(titleMap.count(title)>0)
    {
       settingMap sMap=(settingMap)titleMap[title];
       if(sMap.count(subtitle)>0)
            value=sMap[subtitle];
    }
    return value;
}

int ConfigReader::getFieldValueAsInteger(std::string title, std::string subtitle)
{
    int value=-1;
    std::string data=getFieldValueAsString(title,subtitle);
    if(data!="NULL")
    {
        std::istringstream iss(data);
        iss>>value;
    }
    return value;
}

double ConfigReader::getFieldValueAsDouble(std::string title, std::string subtitle)
{
    double value=-1;
    std::string data=getFieldValueAsString(title,subtitle);
    if(data!="NULL")
    {
        std::istringstream iss(data);
        iss>>value;
    }
    return value;
}

void ConfigReader::pushConfigFile(std::string configName)
{
	ReaderMap[configName]=this;
}

ConfigReader* ConfigReader::getConfigFile(std::string configName)
{
    if(ReaderMap.find(configName)!=ReaderMap.end())
		return ReaderMap[configName];
    else
        return NULL;
}

bool ConfigReader::getFieldValueAsBool(std::string title, std::string subtitle)
{
    std::string _value=getFieldValueAsString(title,subtitle);
    return (_value=="yes" || _value=="Yes"?true:false);
}