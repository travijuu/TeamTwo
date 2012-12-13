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

#include "ConfigWriter.h"

ConfigWriter::ConfigWriter(std::string fileName)
{
    this->fileName=fileName;
    this->reader=NULL;
}

void ConfigWriter::importConfigReader(ConfigReader* reader,bool overwriteExisting)
{
    this->reader=reader;
    if(overwriteExisting)
    {
        std::map<std::string,settingMap> _backup=titleMap;
        this->titleMap=reader->titleMap;
        std::map<std::string,settingMap>::iterator iter=_backup.begin();
        while(iter!=_backup.end())
        {
            if(titleMap.count(iter->first)>0)
            {
                std::map<std::string,std::string>::iterator iter2=iter->second.begin();
                while(iter2!=iter->second.end())
                {
                    titleMap[iter->first][iter2->first]=iter2->second;
                    iter2++;
                }
            }
            else
            {
                settingMap _settingMap;
                std::map<std::string,std::string>::iterator iter2=iter->second.begin();
                while(iter2!=iter->second.end())
                {
                    _settingMap[iter2->first]=iter2->second;
                    iter2++;
                }
                titleMap[iter->first]=_settingMap;
            }
            iter++;
        }
    }
    else
        this->titleMap=reader->titleMap;    
    
    std::map<std::string,settingMap>::iterator iter=titleMap.begin();
    while(iter!=titleMap.end())
    {
        std::cout<<"["<<iter->first<<"]"<<std::endl;
        settingMap _s=iter->second;
        std::map<std::string,std::string>::iterator iter2=_s.begin();
        while(iter2!=_s.end())
        {
            std::cout<<iter2->first<<"="<<iter2->second<<std::endl;
            iter2++;
        }
        iter++;
    }
    
}

void ConfigWriter::setValue(std::string titleName,std::string attributeName,std::string value)
{
    if(titleMap.count(titleName)>0)
        titleMap[titleName][attributeName]=value;
    else
    {
        settingMap _s;
        _s[attributeName]=value;
        titleMap[titleName]=_s;
    }
}

void ConfigWriter::saveFile(bool reloadConfigReader)
{
    std::ofstream ostream;
    ostream.open(fileName.c_str(),std::ios::out);
    if(ostream.is_open())
    {
        std::map<std::string, settingMap>::iterator iter = titleMap.begin();
        std::map<std::string,std::string>::iterator iter2;
        while(iter!=titleMap.end())
        {
            ostream<<"["<<iter->first<<"]"<<std::endl;
            iter2=iter->second.begin();
            while(iter2!=iter->second.end())
            {
                std::string line=iter2->first+"="+iter2->second;
                ostream<<line<<std::endl;
                iter2++;
            }
            iter++;
            ostream<<std::endl;
        }
        ostream.close();

        if(reader!=NULL && reloadConfigReader)
            reader->reload();
    }
}
