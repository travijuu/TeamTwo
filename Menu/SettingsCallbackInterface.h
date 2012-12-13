/*
	Copyright (C) 2011-2012  Erkin Cakar 

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

#ifndef SETTINGSCALLBACKINTERFACE_H
#define	SETTINGSCALLBACKINTERFACE_H

/* Settings Callback Interface
 *
 * This class is responsible for dealing with the callbacks
 * of settings menus
 *
 */

class SettingsCallbackInterface {
public:
    // This is triggered when multiplayer settings saved
    virtual void multiplayerSettingsChangedEvent() = 0;
    // This is triggered when network settings saved
    virtual void networkSettingsChangedEvent() = 0;
    // This is triggered when graphic settings saved
    virtual void graphicSettingsChangedEvent() = 0;
};

#endif	/* SETTINGSCALLBACKINTERFACE_H */

