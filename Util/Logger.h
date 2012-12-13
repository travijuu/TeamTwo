/*
	Copyright (C) 2011-2012  Alican Sekerefe 

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

#ifndef LOGGER_H
#define	LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <boost/thread/mutex.hpp>

/* Logger
 *
 * The purpose of this class is to
 * provide a logging mechanism that
 * both supports file writing and
 * console printing operations in
 * order to simplify the debugging
 * process. It works according to
 * the Singleton design pattern.
 */

class Logger
{
public:
    //sets the title that is going to be used at the beggining of the log file
    //or console print
    void setTitle(std::string title);
    //enables/disables console printing
    void setConsolePrint(bool flag);
    //enables/disables file printing
    void setFilePrint(bool flag);
    //sets the path of the log file (if enabled)
    void setFilePath(std::string filepath);
    //sets the file name of the log file
    void setFileName(std::string filename);
    //sets the file name and path of the log file
    void setFileName(std::string filename,std::string filepath);
    //adds a line to the log mechanism. the given message is printed to the
    //file and/or console. by default, given message is considered as a normal
    //message. however, if the message is important, "true" should be sent as
    //the second parameter to notify the debugger that a special event has happened
    //by using seperators such as **********[MESSAGE HERE]**********
    void addLine(std::string log,bool important=false);
    //begins writing/printing operations
    void begin();
    //ends writing/printing operations
    void end();
    //returns the singleton of the Logger class
    static Logger* getSingleton();
private:
    //private constructor of Logger
    Logger();
    //initializes the default values of the Logger
    void initializeDefaultValues();
    //opens a log file
    void openFile();
    //closes the log file
    void closeFile();
    //writes a line into the console and/or file
    void writeLine(std::string line);
    //returns the current time as string in HH:MM:SS format
    std::string getTime();

    mutable boost::mutex _mutex;
    static Logger* singleton;
    std::string title;
    std::string filename;
    std::string filepath;
    bool writeConsole;
    bool writeFile;
    bool enabled;
    std::ofstream ostream;

};

#endif