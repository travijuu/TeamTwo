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

#ifndef FILESERVER_H
#define	FILESERVER_H

#include "TcpConnector.h"
#include "FileTransferStructure.h"
#include <fstream>
#include <boost/thread.hpp>
#include <NetworkUtil/FileServerCallbackInterface.h>

/* File Server
 *
 * FileServer enables sharing given files
 * with a FileReceiver instance. It's an
 * event based class that uses callback methods
 * to notify important events to the subscriber
 * object. The class uses TcpConnector and
 * boost::thread for non-blocking file transfer.
 */

typedef struct TTFTPClientInfo
{
    int socket;
    int reqFileID;
}TTFTPClientInfo_t;

class FileServer : public TcpCallbackInterface
{
public:
    //constructor of the class. takes root path, server tcp port and callback object
    FileServer(std::string path,std::string port,FileServerCallbackInterface* callbackObject);
    //adds a file that is going to be shared. before adding it, it checks its existence
    void addFile(std::string file);
    //starts the server by enabling accepting incoming connections
    void startServer();
    //callback method of TcpConnector. called after a message has been received
    void tcpMessageReceivedEvent(std::string ip,int socket,char* message,size_t size);
    //callback method of TcpConnector. called after a connection has been terminated
    void connectionTerminatedEvent(int socket);
    //returns true if the given filename is being shared
    bool isFileAdded(std::string fileName);
    //stops accepting the connections and kills networking functionality
    void stop();

private:
    //creates a TTFTPClientInfo_t type client structure and accepts the connection
    void handleConnectionRequest(int socket);
    //handles the incoming file transfer request according to the existence of the
    //specified filename. it sends a response to the client according to the search result
    void handleFileTransferRequest(TTFTPClientInfo_t* info,std::string fileName);
    //handles file send request taken by the client application (FileReceiver)
    void handleStartSendingRequest(TTFTPClientInfo_t* info);
    //starts sending the file to the given client by creating a new boost::thread
    void startFileTransfer(TTFTPClientInfo_t* info);
    //send a basic message (1 byte) through the given socket
    void sendBasicMessage(int socket,unsigned int message);
    //creates a header according to the communication protocol between Server and Receiver
    Header_t createHeader(unsigned char messageType);
    //returns the size of the requested file
    long getFileSize(std::string fileName);

    TcpConnector* tcp;
    int bufferSize;
    bool ready;
    int clientSocket;
    std::string path;
    std::map<int,std::string> fileListMap;
    std::map<int,TTFTPClientInfo_t*> clientMap;
    boost::thread* sendingThread;
    FileServerCallbackInterface* callbackObject;
};

#endif	/* FILESERVER_H */

