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

#ifndef GAMECLIENT_H
#define	GAMECLIENT_H
#include <iostream>

/* Game Client
 *
 * This entity class is responsible for
 * holding information of a player(client)
 * that is in the game or in the room.
 */

class GameClient
{
public:
    //constructor. takes a client id, its name and team number (if in team-deathmatch mode)
    GameClient(unsigned int clientID, std::string clientName,unsigned char team);
    //returns the name of the client
    std::string getName();
    //sets the name of the client
    void setName(std::string name);
    //returns the id of the client
    unsigned int getID();
    //returns the team number of the client
    unsigned char getTeam();
    //sets the team number of the client
    void setTeam(unsigned char team);
    //sets status of the client; "true" should be passed if s/he's ready to play.
    void setReady(bool ready);
    //returns the status of the client
    bool isReady();
    //sets kill count
    void setKillCount(int killCount);
    //sets death count
    void setDeathCount(int deathCount);
    //returns death count
    int getDeathCount();
    //returns kill count
    int getKillCount();
	//sets character color of the player
	void setColor(unsigned char color);
	//gets character color of the player
	unsigned char getColor();

private:
    unsigned int id;
    unsigned char team;
    std::string name;
    bool ready;
    int kills;
    int deaths;
	unsigned char color;
};

#endif	/* GAMECLIENT_H */

