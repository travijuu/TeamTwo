/*
	Copyright (C) 2011-2012 Erkin Cakar

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

#include "NetworkSettingsMenu.h"

NetworkSettingsMenu::NetworkSettingsMenu(std::string name, MenuControllerInterface *menuControllerInterface, SettingsCallbackInterface* settingsCallbackInterface)
{
    this->name = name;
    this->menuControllerInterface = menuControllerInterface;
    this->settingsCallbackInterface = settingsCallbackInterface;
    this->wManager = &CEGUI::WindowManager::getSingleton();
    this->sheet = this->wManager->createWindow("DefaultWindow",this->name+"Sheet");
 
    ipAddress = "192.168.1.1";
    serverPort = "35610";
    TTFTPPort = "35611";
    createMenu();
    
}

NetworkSettingsMenu::~NetworkSettingsMenu() {
}

std::string NetworkSettingsMenu::getServerPort()
{
    return serverPort;
}
std::string NetworkSettingsMenu::getTTFTP()
{
    return TTFTPPort;
}
std::string NetworkSettingsMenu::getIPAddress()
{
    return ipAddress;
}
void NetworkSettingsMenu::setTTFTP(std::string value)
{
    TTFTPPort = value;
    wManager->getWindow(name+"/ttftpEditbox")->setText(value);
}
void NetworkSettingsMenu::setServerPort(std::string value)
{
    serverPort = value;
    wManager->getWindow(name+"/serverEditbox")->setText(value);
}
void NetworkSettingsMenu::setIPAddress(std::string value)
{
    ipAddress = value;
      wManager->getWindow(name+"/ipAddressEditbox")->setText(value);
}
void NetworkSettingsMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
}

bool NetworkSettingsMenu::buttonClickEvent(const CEGUI::EventArgs& evt)
{
     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     std::string actionName = mouseEventArgs.window->getName().c_str();

     if(actionName == name+"/backButton")
         ;
     else if(actionName == name+"/saveButton")
     {
         ipAddress = ((CEGUI::Editbox*) wManager->getWindow(name+"/ipAddressEditbox"))->getText().c_str();
         TTFTPPort = ((CEGUI::Editbox*) wManager->getWindow(name+"/ttftpEditbox"))->getText().c_str();
         serverPort = ((CEGUI::Editbox*) wManager->getWindow(name+"/serverEditbox"))->getText().c_str();
         
         settingsCallbackInterface->networkSettingsChangedEvent();
     }

     menuControllerInterface->back();
     resetFields();
	return true;
}

void NetworkSettingsMenu::createMenu()
{
    CEGUI::Window* guiLayout = wManager->loadWindowLayout("layouts/NetworkSettings.layout");
    wManager->getWindow(name)->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.3f, 0)));
    wManager->getWindow(name+"/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&NetworkSettingsMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/saveButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&NetworkSettingsMenu::buttonClickEvent,this));
    sheet->addChildWindow(guiLayout);
    resetFields();
}

void NetworkSettingsMenu::showMenu()
{
    wManager->getWindow(name)->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);
}

void NetworkSettingsMenu::resetFields()
{
     wManager->getWindow(name+"/ipAddressEditbox")->setText(ipAddress);
     wManager->getWindow(name+"/ttftpEditbox")->setText(TTFTPPort);
     wManager->getWindow(name+"/serverEditbox")->setText(serverPort);
}
CEGUI::Window* NetworkSettingsMenu::getSheet()
{
    return sheet;
}