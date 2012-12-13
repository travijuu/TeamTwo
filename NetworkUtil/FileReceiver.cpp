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

#include "FileReceiver.h"
#include <sys/stat.h>

FileReceiver::FileReceiver(std::string ip, std::string port,FileReceiverCallbackInterface* callbackObject)
{
    tcp = new TcpConnector(ip, port);
    tcp->enableCallback(this);
    ready = false;
    fileSize = kbps = 0;
    currentTime = time(NULL);
    requestAccepted = false;
    this->callbackObject=callbackObject;
    writingInProgress=false;
}

void FileReceiver::connect()
{
    if (tcp->connectToHost())
        sendBasicMessage(FileTransferMessage::CONNECTION_REQUEST);
}

void FileReceiver::requestFile(std::string fileName)
{
    if (ready)
    {
        FileTransferRequest_t req;
        req.header = createHeader(FileTransferMessage::FILE_TRANSFER_REQUEST);
        strncpy(req.fileName, fileName.c_str(), FILE_NAME_SIZE);
        tcp->sendData(&req, sizeof (FileTransferRequest_t));
    }
}

void FileReceiver::startReceiving(std::string fileName,std::string filePath)
{
    if (ready)
    {
#ifdef WINDOWS_PLATFORM
        if (CreateDirectoryA(filePath.c_str(), NULL))
            std::cout << "directory created\n";
#else
        mkdir(filePath.c_str(),0777);
#endif
		fileStream.open((filePath+"/"+fileName).c_str(), std::ios::out|std::ios::binary);
        if (fileStream.is_open())
            sendBasicMessage(FileTransferMessage::START_SENDING);
    }
	else
		std::cout<<"file receiver not ready\n";
}

void FileReceiver::tcpMessageReceivedEvent(std::string ip, int socket, char* msg, size_t size)
{
    Header_t* header;
    char* _message = msg;
    header = (Header_t*) _message;
    if (header->protocolID == PROTOCOLID)
    {
        switch (header->messageType)
        {
            case FileTransferMessage::CONNECTION_REQUEST_ACCEPT:
			{
                ready = true;
                callbackObject->fileServerConnectionEstablished();
                break;
            }
            case FileTransferMessage::FILE_TRANSFER_REQUEST_ACCEPT  :
            {
                FileTransferRequestAccept* accept=(FileTransferRequestAccept*)_message;
                fileSize=accept->fileSize;
                requestAccepted = true;
                callbackObject->fileFound();
                break;
            }
            case FileTransferMessage::FILE_TRANSFER_REQUEST_DENY:
            {
				requestAccepted = false;
                callbackObject->fileNotFound();
                break;
            }
            case FileTransferMessage::START_RECEIVING:
            {
                callbackObject->fileTransferStarted();
                break;
            }
            case FileTransferMessage::FILE_PIECE:
            {
                FilePiece_t* piece = (FilePiece_t*) _message;
                handleIncomingFilePiece(piece);
                callbackObject->fileTransferProgressChanged(0,0);
                break;
            }
            case FileTransferMessage::SENDING_FINISHED:
            {
                handleTransferComplete();
                callbackObject->fileTransferCompleted();
                break;
            }
            default:
            {
                std::cout << "unknown message! " << (int) header->messageType << std::endl;
                break;
            }
        }
    } 
	else
        std::cout << "non-FileReceiver message! " << (int) header->messageType << std::endl;
    //delete msg;
}

void FileReceiver::connectionTerminatedEvent(int socket)
{
    ready = false;
}

void FileReceiver::handleTransferComplete()
{
    fileStream.close();
    tcp->stop();
}

void FileReceiver::handleIncomingFilePiece(FilePiece_t* piece)
{
    fileStream.seekg(piece->pieceID*BUFFER_SIZE,std::ios::beg);
    fileStream.write(piece->buffer, piece->pieceSize);
    fileStream.flush();
    totalReceived += piece->pieceSize;
    if (time(NULL) == currentTime)
        kbps += (float) piece->pieceSize / 1024.;
    else
    {
        kbps = 0;
        currentTime = time(NULL);
    }
    delete piece;

}

Header_t FileReceiver::createHeader(unsigned char messageType)
{
    Header_t header;
    header.protocolID = PROTOCOLID;
    strncpy(header.protocolName, PROTOCOL_NAME, 5);
    header.messageType = messageType;
    return header;
}

void FileReceiver::sendBasicMessage(unsigned int message)
{
    Header_t header = createHeader(message);
    tcp->sendData(&header,sizeof(header));
}

void FileReceiver::stop()
{
    if(tcp!=NULL)
        tcp->stop();
}
