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

#ifndef FILERECEIVER_H
#define	FILERECEIVER_H

#include "TcpConnector.h"
#include <time.h>
#include <fstream>
#include "FileTransferStructure.h"
#include "FileReceiverCallbackInterface.h"

/* File Receiver
 * 
 * This class is responsible for connecting
 * to a FileServer, request and download a file
 * specified by the programmer. It utilizes
 * from the TcpConnector interface for the 
 * communication. It's an event based structure
 * that is able to notify the subscriber object
 * about the status and the progress of the file
 * transfer.
 */


class FileReceiver:public TcpCallbackInterface
{
public:
    //constructor of the class. takes the server ip, its port and a callbackObject
    //for notifying the subscriber class that an event has happened
    FileReceiver(std::string ip,std::string port,FileReceiverCallbackInterface* callbackObject);
    //tries to connect to the server. if connected, the callback method is invoked
    void connect();
    //terminates the tcp connection
    void stop();
    //requests the file from the connected FileServer. if request is accepted/denied
    //callback method is invoked to notify the subscriber class
    void requestFile(std::string fileName);
    //starts to download the file from the FileServer. fileName passed to this
    //method is the name that the requested file will be saved.
    void startReceiving(std::string fileName, std::string filePath = "\\");
    //callback method of TcpConnector to receive and handle incoming TCP messages
    void tcpMessageReceivedEvent(std::string ip,int socket,char* msg,size_t size);
    //callback method of TcpConnector for getting notified that the connection has been terminated
    void connectionTerminatedEvent(int socket);

private:
    FileReceiverCallbackInterface* callbackObject;
    //invoked after the file has been downloaded
    void handleTransferComplete();
    //gets the incoming file piece and writes it to the file stream
    void handleIncomingFilePiece(FilePiece_t* piece);
    //creates a protocol header for FileServer/FileReceiver
    Header_t createHeader(unsigned char messageType);
    //sends a simple (1 byte) message by using the TcpConnector. 
    void sendBasicMessage(unsigned int message);


    std::fstream fileStream;
    TcpConnector* tcp;
    bool ready;
    bool requestAccepted;
    long fileSize;
    long totalReceived;
    time_t currentTime;
    double kbps;
    bool writingInProgress;
};

#endif	/* FILERECEIVER_H */

