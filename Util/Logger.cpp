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

#include "Logger.h"

Logger* Logger::singleton=NULL;

Logger* Logger::getSingleton()
{
    if(singleton==NULL)
        singleton=new Logger();
    return singleton;
}

Logger::Logger()
{
    initializeDefaultValues();
}

void Logger::initializeDefaultValues()
{
    title="-=Logger=-";
    filename="AppLog.log";
    filepath="./";
    enabled=writeConsole=!(writeFile=true);
}

void Logger::openFile()
{
    if(!ostream.is_open())
        ostream.open((filepath+filename).data(),std::ios::ate);
    if(writeFile)
        writeLine("*** "+title+" ***\n");
    if(writeConsole)
        std::cout<<"*** "+title+" ***"<<std::endl;
}

void Logger::closeFile()
{
    if(ostream.is_open())
        ostream.close();
}

void Logger::writeLine(std::string line)
{
    if(ostream.is_open())
    {
        ostream<<line<<std::endl;
        ostream.flush();
    }
}

void Logger::setTitle(std::string title)
{
    this->title=title;
}

void Logger::setConsolePrint(bool flag)
{
    writeConsole=flag;
}
void Logger::setFilePrint(bool flag)
{
    writeFile=flag;
}
void Logger::setFilePath(std::string filepath)
{
    this->filepath=filepath;
}
void Logger::setFileName(std::string filename)
{
    this->filename=filename;
}

void Logger::setFileName(std::string filename,std::string filepath)
{
    this->filename=filename;
    this->filepath=filepath;
}

void Logger::begin()
{
    enabled=true;
    openFile();
}

void Logger::end()
{
    enabled=false;
    closeFile();
}

std::string Logger::getTime()
{
    time_t timepassed;
    time(&timepassed);
    struct tm* info;
    info=localtime(&timepassed);
    std::ostringstream ss;
    if(info->tm_hour<10)
        ss<<"0";
    ss<<info->tm_hour<<":";
    if(info->tm_min<10)
        ss<<"0";
    ss<<info->tm_min<<":";
    if(info->tm_sec<10)
        ss<<"0";
    ss<<info->tm_sec;
    return ss.str();
}

void Logger::addLine(std::string log,bool important)
{
	boost::mutex::scoped_lock lock(_mutex);
    if(enabled)
    {
        if(important)
            log="*********"+log+"*********";
        if(writeFile)
        {
            if(!ostream.is_open())
                openFile();
            writeLine(getTime()+"> "+log);
        }
        if(writeConsole)
            std::cout<<getTime()<<"> "<<log<<std::endl;
    }
}