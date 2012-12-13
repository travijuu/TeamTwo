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

#include <vector>

#include "GraphicSettingsMenu.h"

GraphicSettingsMenu::GraphicSettingsMenu(std::string name, MenuControllerInterface* menuControllerInterface, SettingsCallbackInterface* settingsCallbackInterface)
{
    this->name = name;
    this->menuControllerInterface = menuControllerInterface;
    this->settingsCallbackInterface = settingsCallbackInterface;
    wManager = &CEGUI::WindowManager::getSingleton();
    sheet = wManager->createWindow("DefaultWindow",this->name+"Sheet");

    shadow = light = spotLight = true;
   
    createMenu();
}

GraphicSettingsMenu::~GraphicSettingsMenu()
{
}


bool GraphicSettingsMenu::getShadow()
{
    return shadow;
}
bool GraphicSettingsMenu::getLight()
{
    return light;
}
bool GraphicSettingsMenu::getSpotLight()
{
    return spotLight;
}
void GraphicSettingsMenu::setShadow(bool value)
{
    shadow = value;
    ((CEGUI::Checkbox*) wManager->getWindow(name+"/shadowCheckbox"))->setSelected(value);
}
void GraphicSettingsMenu::setLight(bool value)
{
    light = value;
    ((CEGUI::Checkbox*) wManager->getWindow(name+"/lightCheckbox"))->setSelected(value);
}
void GraphicSettingsMenu::setSpotLight(bool value)
{
    spotLight = value;
    ((CEGUI::Checkbox*) wManager->getWindow(name+"/spotLightCheckbox"))->setSelected(value);
}

bool GraphicSettingsMenu::buttonClickEvent(const CEGUI::EventArgs& evt)
{
     CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
     std::string actionName = mouseEventArgs.window->getName().c_str(); 
     if(actionName == name+"/backButton")
     ;
     else if (actionName == name+"/saveButton")
     {
         //antiAlising = ( (CEGUI::Combobox*) wManager->getWindow(name+"/antiAlisingCombobox"))->getSelectedItem()->getText().c_str();
         shadow = ((CEGUI::Checkbox*) wManager->getWindow(name+"/shadowCheckbox"))->isSelected();
         light = ((CEGUI::Checkbox*) wManager->getWindow(name+"/lightCheckbox"))->isSelected();
         spotLight = ((CEGUI::Checkbox*) wManager->getWindow(name+"/spotLightCheckbox"))->isSelected();
         settingsCallbackInterface->graphicSettingsChangedEvent();
     }
     menuControllerInterface->back();
     resetFields();
     return true;
}
bool GraphicSettingsMenu::checkboxEvent(const CEGUI::EventArgs& evt)
{
    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
    std::string checkbox = mouseEventArgs.window->getName().c_str();

    if( ((CEGUI::Checkbox*)wManager->getWindow(checkbox))->isSelected() )
         ((CEGUI::Checkbox*)wManager->getWindow(checkbox))->setText(" Enabled");
    else
         ((CEGUI::Checkbox*)wManager->getWindow(checkbox))->setText(" Disabled");
    return true;
}
void GraphicSettingsMenu::closeMenu()
{
    wManager->getWindow(name)->setVisible(false);
}

void GraphicSettingsMenu::createMenu()
{
    CEGUI::Window* menuLayout = wManager->loadWindowLayout("layouts/GraphicSettings.layout");
    wManager->getWindow(name)->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.3f, 0)));
    wManager->getWindow(name+"/backButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GraphicSettingsMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/saveButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GraphicSettingsMenu::buttonClickEvent,this));
    wManager->getWindow(name+"/shadowCheckbox")->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&GraphicSettingsMenu::checkboxEvent,this));
    wManager->getWindow(name+"/spotLightCheckbox")->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&GraphicSettingsMenu::checkboxEvent,this));
    wManager->getWindow(name+"/lightCheckbox")->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&GraphicSettingsMenu::checkboxEvent,this));

    resetFields();
    sheet->addChildWindow(menuLayout);
}
void GraphicSettingsMenu::showMenu()
{
    wManager->getWindow(name)->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);
}

void GraphicSettingsMenu::resetFields()
{
    ((CEGUI::Checkbox*)wManager->getWindow(name+"/shadowCheckbox"))->setSelected(shadow);
    ((CEGUI::Checkbox*)wManager->getWindow(name+"/lightCheckbox"))->setSelected(light);
    ((CEGUI::Checkbox*)wManager->getWindow(name+"/spotLightCheckbox"))->setSelected(spotLight);
}
CEGUI::Window* GraphicSettingsMenu::getSheet()
{
    return sheet;
}