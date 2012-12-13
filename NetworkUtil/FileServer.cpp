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

#include <iosfwd>
#include <sys/stat.h>

#include "FileServer.h"
//#include "Logger/Logger.h"

FileServer::FileServer(std::string path,std::string port,FileServerCallbackInterface* callbackInterface)
{
    tcp = new TcpConnector(port);
    tcp->enableCallback(this);
    this->bufferSize = 1024;
    this->callbackObject=callbackInterface;
    this->path=path+"/";
#ifdef WINDOWS_PLATFORM
    if (CreateDirectoryA((path).c_str(), NULL))
        std::cout << "directory created\n";
#else
    mkdir(path.c_str(),0777);
#endif
    ready = false;
}

bool FileServer::isFileAdded(std::string fileName)
{
    bool found=false;
    std::map<int,std::string>::iterator iter=fileListMap.begin();
    while(iter!=fileListMap.end())
    {
        if(iter->second==fileName)
        {
            found=true;
            break;
        }
        iter++;
    }
    return found;
}

void FileServer::addFile(std::string file)
{
    static int fileID = 0;
    if(!isFileAdded(file))
    {
        std::ifstream fStream;
		fStream.open((path+file).c_str(),std::ios::in|std::ios::binary);
        if(fStream.is_open())
        {
            fileListMap[fileID++] = file;
            fStream.close();
        }
   //     else
   //         Logger::getSingleton()->addLine("FileServer: File '"+file+"' not found!",true);
    }    
}

void FileServer::startServer()
{
    tcp->startListening();
}

void FileServer::tcpMessageReceivedEvent(std::string ip, int socket, char* message, size_t size)
{
    Header_t* header;
    TTFTPClientInfo_t* client;
    header = (Header_t*) message;
    if (header->protocolID == PROTOCOLID)
    {
        if (header->messageType == FileTransferMessage::CONNECTION_REQUEST)
            handleConnectionRequest(socket);
        else if (clientMap.count(socket) > 0)
        {
            client = clientMap[socket];
            switch (header->messageType)
            {
                case FileTransferMessage::FILE_TRANSFER_REQUEST:
                {
                    FileTransferRequest_t* request = (FileTransferRequest_t*) message;
                    handleFileTransferRequest(client, std::string(request->fileName));
                    break;
                }
                case FileTransferMessage::START_SENDING:
                {
                    handleStartSendingRequest(client);
                    break;
                }
                default:
                {
                    std::cout << "unknown message! " << (int) header->messageType << std::endl;
                }
            }
        }
		else
			std::cout<<"client is not connected!\n";
    }
    else
        std::cout << "non-ttftp message!" << (int) header->messageType << std::endl;
    delete message;
}

void FileServer::connectionTerminatedEvent(int socket)
{
    if (clientMap.count(socket) > 0)
    {
        TTFTPClientInfo_t* client = clientMap[socket];
        clientMap.erase(socket);
        delete client;
    }
}

void FileServer::sendBasicMessage(int socket,unsigned int message)
{
    Header_t header = createHeader(message);
    tcp->sendData(socket, &header, sizeof (Header_t));
}

Header_t FileServer::createHeader(unsigned char messageType)
{
    Header_t header;
    header.protocolID = PROTOCOLID;
    strncpy(header.protocolName, PROTOCOL_NAME, 5);
    header.messageType = messageType;
    return header;
}

void FileServer::startFileTransfer(TTFTPClientInfo_t* info)
{
    std::string fileName = fileListMap[info->reqFileID];
    std::ifstream fileStream;
    fileStream.open((path+fileName).c_str(),std::ios::in|std::ios::binary);
    if (fileStream.is_open())
    {
        FilePiece_t piece;
        int readSize;
        int sent = 0;
        int pieceID=0;
        piece.header = createHeader(FileTransferMessage::FILE_PIECE);
        while (!fileStream.eof())
        {
            fileStream.read((char*) piece.buffer, (int) BUFFER_SIZE);
            readSize = fileStream.gcount();
            if (readSize > 0)
            {
                piece.pieceSize = readSize;
                piece.pieceID=pieceID++;
                tcp->sendData(info->socket, &piece, sizeof (FilePiece_t));
                sent += readSize;
            }
            else
                break;
        }
        fileStream.close();
        sendBasicMessage(info->socket, FileTransferMessage::SENDING_FINISHED);
        callbackObject->fileUploadFinished(fileListMap[info->reqFileID]);
        info->reqFileID = -1;
    }
    else
        sendBasicMessage(info->socket, FileTransferMessage::SERVER_ERROR);
}

void FileServer::handleFileTransferRequest(TTFTPClientInfo_t* info,std::string fileName)
{
    bool found = false;
    std::map<int, std::string>::iterator iter = fileListMap.begin();
    while (iter != fileListMap.end())
    {
        if (iter->second == fileName)
                                     
        {
            info->reqFileID = iter->first;
            found = true;
            break;
        }
        iter++;
    }
    if(found)
    {
        long fileSize=getFileSize(path+fileName);
        FileTransferRequestAccept message={createHeader(FileTransferMessage::FILE_TRANSFER_REQUEST_ACCEPT),fileSize};
        tcp->sendData(info->socket,&message,sizeof(FileTransferRequestAccept));
    }
    else
        sendBasicMessage(info->socket, FileTransferMessage::FILE_TRANSFER_REQUEST_DENY);
}

void FileServer::handleConnectionRequest(int socket)
{
    TTFTPClientInfo_t* info = new TTFTPClientInfo_t;
    info->socket = socket;
    info->reqFileID = -1;
    clientMap[socket] = info;
    sendBasicMessage(socket, FileTransferMessage::CONNECTION_REQUEST_ACCEPT);
}

void FileServer::handleStartSendingRequest(TTFTPClientInfo_t* info)
{
    if (info->reqFileID>-1)
    {
        sendBasicMessage(info->socket, FileTransferMessage::START_RECEIVING);
        sendingThread=new boost::thread(&FileServer::startFileTransfer,this,info);
        callbackObject->fileUploadStarted(fileListMap[info->reqFileID]);
    }
    else
        sendBasicMessage(info->socket, FileTransferMessage::NO_FILE_REQUESTED);
}

void FileServer::stop()
{
    tcp->stop();
}

long FileServer::getFileSize(std::string fileName)
{
    std::ifstream ifStream;
    ifStream.open(fileName.c_str());
    ifStream.seekg(0,std::ios::end);
    return ifStream.tellg();
}
