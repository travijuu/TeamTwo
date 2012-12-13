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

#ifndef CONFIGWRITER_H
#define	CONFIGWRITER_H

#include "Util/ConfigReader.h"

/*  Config Writer
 *
 * This class is responsible for writing
 * configuration parameters that are necessary
 * for the system in order to perform defined
 * actions while running. ConfigWriter has an
 * ability to import ConfigReader which contains
 * the original information of a configuration file,
 * for saving/overwriting the existing parameters
 * without data loss. if the import option is not
 * used, original data of the configuration file
 * is simply lost.


 */

class ConfigWriter
{
public:
    //constructor of the class. takes the file name to be saved
    ConfigWriter(std::string fileName);
    //imports the current attributes of a configuration file
    //from the given ConfigReader object. if overwriteExisting
    //is false, current values saved in ConfigWriter will be purged
    void importConfigReader(ConfigReader* reader,bool overwriteExisting=false);
    //assings a value to the attribute in the indicated field name
    void setValue(std::string fieldName,std::string attribute,std::string value);
    //saves the file. if false is sent as the paramenter, imported ConfigReader
	//object is not going to be reloaded
    void saveFile(bool reloadConfigReader=true);
    
private:
    std::string fileName;
    std::map<std::string,settingMap> titleMap;
    ConfigReader* reader;

};

#endif	/* CONFIGWRITER_H */

