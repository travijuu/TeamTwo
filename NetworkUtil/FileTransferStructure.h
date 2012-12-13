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

#ifndef FILETRANSFERSTRUCTURE_H
#define	FILETRANSFERSTRUCTURE_H

#define PROTOCOLID 321456987
#define PROTOCOL_NAME "TTFTP"
#define BUFFER_SIZE 4096
#define FILE_NAME_SIZE 100

/*
	FileTransferMessage

	This structure collection contains the 
	important structs which are used by both
	FileServer and FileReceiver classes for
	messaging over the network using TCP

*/

namespace FileTransferMessage
{
    enum TransferMessages
    {
        READY_TO_SEND=0,
        READY_TO_RECEIVE,
        START_SENDING,
        START_RECEIVING,
        FILE_PIECE,
        SENDING_FINISHED,
        FILE_TRANSFER_REQUEST,
        FILE_TRANSFER_REQUEST_DENY,
        FILE_TRANSFER_REQUEST_ACCEPT,
        CONNECTION_REQUEST,
        CONNECTION_REQUEST_ACCEPT,
        NO_FILE_REQUESTED,
        SERVER_ERROR
    };
}

typedef struct Header
{
    unsigned int protocolID;
    char protocolName[5];
    unsigned int messageType;
}Header_t;


typedef struct FileTransferRequest
{
    Header_t header;
    char fileName[FILE_NAME_SIZE];
}FileTransferRequest_t;

typedef struct FileTransferRequestAccept
{
    Header_t header;
    long fileSize;
}FileTransferRequestAccept_t;



typedef struct FilePiece
{
    Header_t header;
    char buffer[BUFFER_SIZE];
    int pieceID;
    int pieceSize;
}FilePiece_t;


#endif	/* FILETRANSFERSTRUCTURE_H */

