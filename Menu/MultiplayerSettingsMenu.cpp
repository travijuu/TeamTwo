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

#include "MultiplayerSettingsMenu.h"
#include <iostream>

MultiplayerSettingsMenu::MultiplayerSettingsMenu(std::string name, MenuControllerInterface* menuControllerInterface, SettingsCallbackInterface* settingsCallbackInterface)
{
    this->name = name;
    this->menuControllerInterface = menuControllerInterface;
    this->settingsCallbackInterface = settingsCallbackInterface;
    wManager = &CEGUI::WindowManager::getSingleton();
    sheet = wManager->createWindow("DefaultWindow",this->name+"Sheet");

    playerName = "NULL";
    mouseSensivity = 0.5;
    color = 0;
    createMenu();
}

MultiplayerSettingsMenu::~MultiplayerSettingsMenu()
{ 
}

std::string MultiplayerSettingsMenu::getPlayerName()
{
    return playerName;
}

int MultiplayerSettingsMenu::getMouseSensivity()
{
    return mouseSensivity;
}

int MultiplayerSettingsMenu::getColor()
{
	return color==0?1:0;
}

void MultiplayerSettingsMenu::setPlayerName(std::string value)
{
    playerName = value;
    wManager->getWindow(name+"/playerNameEditbox")->setText(value);
}

void MultiplayerSettingsMenu::setMouseSensivity(float value)
{
    mouseSensivity = value;
    ((CEGUI::Scrollbar*) wManager->getWindow(name+"/sensivityScroll"))->setScrollPosition(mouseSensivity);
}

void MultiplayerSettingsMenu::setColor(int value)
{
    color = value;
    if( color == 0 )
        wManager->getWindow(name+"/colorImage")->setProperty("Image", "set:RedColor image:full_image");
    else
        wManager->getWindow(name+"/colorImage")->setProperty("Image", "set:BlueColor image:full_image");
    
    ((CEGUI::Combobox*)wManager->getWindow(name+"/colorCombobox"))->setItemSelectState((size_t)color,true);

}

bool MultiplayerSettingsMenu::buttonClickEvent(const CEGUI::EventArgs& e)
{
     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) e;
     std::string actionName = mouseEventArgs.window->getName().c_str();

     if(actionName == name+"/backButton")
      ;
     else if (actionName == name+"/saveButton")
     {
         playerName = ((CEGUI::Editbox*) wManager->getWindow(name+"/playerNameEditbox"))->getText().c_str();
         mouseSensivity = ((CEGUI::Scrollbar*) wManager->getWindow(name+"/sensivityScroll"))->getScrollPosition();
         wManager->getWindow(name+"/colorCombobox")->getText() == "Red" ? color = 0 : color = 1;
         settingsCallbackInterface->multiplayerSettingsChangedEvent();
     }
     menuControllerInterface->back();
     resetFields();
     return true;
}

bool MultiplayerSettingsMenu::comboboxChangedEvent(const CEGUI::EventArgs& evt)
{
     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     if( wManager->getWindow(name+"/colorCombobox")->getText() == "Red" )
         wManager->getWindow(name+"/colorImage")->setProperty("Image", "set:RedColor image:full_image");
     else
         wManager->getWindow(name+"/colorImage")->setProperty("Image", "set:BlueColor image:full_image");

     return true;
}

void MultiplayerSettingsMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
}
void MultiplayerSettingsMenu::createMenu()
{
    CEGUI::Window* guiLayout = wManager->loadWindowLayout("layouts/MultiplayerSettings.layout");
    wManager->getWindow(name)->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.3f, 0)));
    wManager->getWindow(name+"/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MultiplayerSettingsMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/saveButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MultiplayerSettingsMenu::buttonClickEvent,this));
    ((CEGUI::Scrollbar*)wManager->getWindow(name+"/sensivityScroll"))->setDocumentSize(10);
    ((CEGUI::Scrollbar*)wManager->getWindow(name+"/sensivityScroll"))->setStepSize(1);

    ((CEGUI::Combobox*)wManager->getWindow(name+"/colorCombobox"))->addItem(new CEGUI::ListboxTextItem("Red"));
    ((CEGUI::Combobox*)wManager->getWindow(name+"/colorCombobox"))->addItem(new CEGUI::ListboxTextItem("Blue"));
    ((CEGUI::Combobox*)wManager->getWindow(name+"/colorCombobox"))->setReadOnly(true);
    ((CEGUI::Combobox*)wManager->getWindow(name+"/colorCombobox"))->setItemSelectState((size_t)0,true);
    wManager->getWindow(name+"/colorCombobox")->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&MultiplayerSettingsMenu::comboboxChangedEvent,this));
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("RedColor","images/Red.png");
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("BlueColor","images/Blue.png");
    wManager->getWindow(name+"/colorImage")->setProperty("Image", "set:RedColor image:full_image");

    sheet->addChildWindow(guiLayout);

    resetFields();
}
void MultiplayerSettingsMenu::showMenu()
{
    wManager->getWindow(name)->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);
}
void MultiplayerSettingsMenu::resetFields()
{
     wManager->getWindow(name+"/playerNameEditbox")->setText(playerName);
     ((CEGUI::Scrollbar*) wManager->getWindow(name+"/sensivityScroll"))->setScrollPosition(mouseSensivity);
     if(!color)
        wManager->getWindow(name+"/colorImage")->setProperty("Image", "set:RedColor image:full_image");
     else
        wManager->getWindow(name+"/colorImage")->setProperty("Image", "set:BlueColor image:full_image");

     ((CEGUI::Combobox*)wManager->getWindow(name+"/colorCombobox"))->setItemSelectState((size_t)color,true);
}