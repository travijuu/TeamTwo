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

#ifndef CONVERTER_H
#define CONVERTER_H

#include <iostream>
#include <sstream>

/* Converter
 *
 * this static class is used for performing
 * simple conversion operations.
 */

class Converter
{
public:
    //converts given string in to an integer and returns it
    static int stringToInt(std::string strNumber);
    //converts the given integer to a std::string and returns it
    static std::string intToString(int number);
};

#endif