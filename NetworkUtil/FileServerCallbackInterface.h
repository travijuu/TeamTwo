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

#ifndef FILESERVERCALLBACKINTERFACE_H
#define FILESERVERCALLBACKINTERFACE_H

/*	FileServerCallbackInterface
	
	This interface needs to be implemented by the
	subscriber class who wants to use FileServer to
	share files.

*/

class FileServerCallbackInterface
{
public:
	//called when the file upload of the given filename has been started
    virtual void fileUploadStarted(std::string fileName)=0;
	//called when the file upload process of the given filename has been changed 
    virtual void fileUploadProgressChanged(std::string fileName,int percentage,float kbSpeed)=0;
	//called when the file upload of the given filename has been finished
    virtual void fileUploadFinished(std::string fileName)=0;
};



#endif