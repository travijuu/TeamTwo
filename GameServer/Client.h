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

#ifndef CLIENT_H
#define	CLIENT_H

#include <TcpConnector.h>
#include <CallbackTimer.h>
#include <ClientCallbackInterface.h>

/*	Client class

	This class is responsible for holding
	data about the clients who are connected
	to the server. Utilizes from CallbackTimer
	for holding a spawn timer
*/

class Client:public CallbackTimerInterface
{
public:
	//constructor of the class. takes name, id , ip and the lobby socket of the client
    Client(std::string name,int id,std::string ipAddress,int socket);
	//returns the id
    int getId();
	//returns the lobby socket of the client
    int getLobbySocket();
	//returns the game socket of the client
    int getGameSocket();
	//sets the game socket of the client
    void setGameSocket(int socket);
	//sets the name of the client
    void setName(std::string name);
	//returns the name of the client
    std::string getName();
	//returns the ip address of the client
    std::string getIp();
	//returns true if the client has joined a room
    bool isJoined();
	//returns true if the client has started to play
    bool isPlaying();
	//sets the join status of the client
    void setJoined(bool joined);
	//sets the play status of the client
    void setPlaying(bool playing);
	//returns true if the client is ready to start playing
    bool isReady();
	//sets the ready status of the client
    void setReady(bool ready);
	//sets the team of the client
    void setTeam(unsigned char team);
	//sets the color of the client
	void setColor(unsigned char color);
	//returns the team of the client
    unsigned char getTeam();
	//returns the color of the client
	unsigned char getColor();
	//starts the spawn timer according to the given milliseconds.
	//after the timer expiration, it calls the given callbackObject
    void startSpawnTimer(int milliseconds,ClientCallbackInterface* callbackObject);
	//stops spawn timer
    void stopSpawnTimer();
	//event method of CallbackTimer
    void timeElapsedEvent();
	//returns true if the client is holding the map file of the current game
    bool hasMap();
	//sets the map ownership status
    void setHasMap(bool value);
	//increments the death count of the client
    void increaseDeathCount();
	//increments the kill count of the client
    void increaseKillCount();
	//returns the kill count of the client
    int getKillCount();
	//returns the death count of the client
    int getDeathCount();
	//resets the death/kill counts
    void zeroRatio();
private:
    int deaths;
    int kills;
    bool joined;
    bool playing;
    std::string name;
    std::string ipAddress;
    int id;
    int lobbySocket;
    int gameSocket;
    bool ready;
    bool hadMapDownloaded;
    unsigned char team;
    TcpConnector* connector;
    CallbackTimer* spawnTimer;
    ClientCallbackInterface* callbackObject;
	unsigned char color;
};

#endif

