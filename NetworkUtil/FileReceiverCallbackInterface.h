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

#ifndef FILERECIVEVERCALLBACKINTERFACE_H
#define FILERECIVEVERCALLBACKINTERFACE_H

/* FileReceiverCallbackInterface

   This interface is used by file receiver in order to
   notify the subscriber class about the important events.
   This interface must be implemented by the user of the
   FileReceiver class.

*/

class FileReceiverCallbackInterface
{
public:
	//this method is called when the requested file is found on the file server.
    virtual void fileFound()=0;
	//this method is called when the requested file is not found on the file server.
    virtual void fileNotFound()=0;
	//this method is called when the transfer of the requested file has been started
    virtual void fileTransferStarted()=0;
	//this method is called when the file transfer has been completed and the file
	//is ready for usage
    virtual void fileTransferCompleted()=0;
	//this method is called when the connection between FileReceiver and FileServer
	//has been established
    virtual void fileServerConnectionEstablished(){};
	//this method is called when the file transfer progress is changed
    virtual void fileTransferProgressChanged(int percentage,float kbSpeed){};
};

#endif