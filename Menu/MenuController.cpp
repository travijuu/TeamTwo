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

#include <map>

#include "MenuController.h"
#include "LobbyMenu.h"
//#include "SoundManager.h"


#include <iostream>

MenuController::MenuController( std::string menuName)
{
    wManager = &CEGUI::WindowManager::getSingleton();
    sheet = wManager->createWindow("DefaultWindow",menuName);
    //CEGUI::System::getSingleton().setGUISheet(sheet);
    optionNum = 0;
    this->menuName = menuName;
    this->callbackObject=callbackObject;

    CEGUI::FontManager::getSingleton().create( "Nimbus-9.font" );
    CEGUI::FontManager::getSingleton().create( "Nimbus-10.font" );
    CEGUI::FontManager::getSingleton().create( "Nimbus-11.font" );
    CEGUI::FontManager::getSingleton().create( "Nimbus-12.font" );
    

    item = wManager->createWindow("TaharezLook/Button", menuName+"/BackButton");
    item->setText("Back");
    item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    item->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuController::backMenu, this));
    item->setVisible(false);
    item->setFont("Nimbus-10");
    sheet->addChildWindow(item);

    item = wManager->createWindow("TaharezLook/StaticText","SubMenuLabel");


    informationPanel = wManager->loadWindowLayout("layouts/Information.layout");
    wManager->getWindow("Information")->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0.3f, 0)));
    wManager->getWindow("Information/okButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuController::buttonClicked,this));
    wManager->getWindow("Information/infoText")->setText("");

    
    
}
MenuController::~MenuController()
{
    delete wManager;
    delete sheet;
    
    menuObjectMap.empty();
    optionVector.empty();
    subOptionMap.empty();
}

void MenuController::addOption( std::string optionName )
{
    
    bool flag = false;
    for(int i=0;i<optionVector.size();i++)
    {
        if(optionName == optionVector.at(i))
            flag = true;
    }

    if(!flag)
    {
        optionNum++;
        wManager->destroyWindow(optionName);
        item = wManager->createWindow("TaharezLook/Button", optionName+"Button");
        item->setText(optionName);
        item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
        item->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.05*optionNum+0.35,0)));
        item->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuController::buttonClicked, this));
        item->setFont("Nimbus-10");
        sheet->addChildWindow(item);

        optionVector.push_back(optionName+"Button");
    }
}

void MenuController::addOption( std::string optionName, Menu *menu )
{
    optionNum++;

    item = wManager->createWindow("TaharezLook/Button", optionName+"Button");
    item->setText(optionName);
    item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    item->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.05*optionNum+0.35,0)));
    item->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuController::buttonClicked,this));
    item->setFont("Nimbus-10");
    sheet->addChildWindow(item);
    
    optionVector.push_back(optionName+"Button");
    menuObjectMap[optionName+"Button"] = menu;

}


void MenuController::addSubOption( std::string subOptionName, std::string parentOptionName, Menu* menu )
{

    item = wManager->createWindow("TaharezLook/Button", subOptionName+"Button");
    item->setText(subOptionName);
    item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    item->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuController::buttonClicked,this));
    item->setVisible(false);
    item->setFont("Nimbus-10");
    sheet->addChildWindow(item);

    subOptionMap[subOptionName+"Button"] = parentOptionName+"Button";
    menuObjectMap[subOptionName+"Button"] = menu;
}
void MenuController::addSubOption( std::string subOptionName, std::string parentOptionName )
{

    item = wManager->createWindow("TaharezLook/Button", subOptionName+"Button");
    item->setText(subOptionName);
    item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    item->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuController::buttonClicked, this));
    item->setVisible(false);
    item->setFont("Nimbus-10");
    sheet->addChildWindow(item);

    subOptionMap[subOptionName+"Button"] = parentOptionName+"Button";
}

void MenuController::showMain()
{  
    clearMenu();
    for(int i =0;i<optionVector.size();i++)
    {
        item = wManager->getWindow(optionVector.at(i));
        item->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.05*i+0.35,0)));
        item->setVisible(true);
    }

    CEGUI::System::getSingleton().setGUISheet(sheet);


}
bool MenuController::backMenu(const CEGUI::EventArgs &e)
{
      showMain();
      return true;
}
void MenuController::clearMenu()
{   
    std::map<std::string, std::string>::const_iterator iterator;
    for(iterator = subOptionMap.begin();iterator != subOptionMap.end();iterator++)
    {
        wManager->getWindow(iterator->first)->setVisible(false);
        //std::cout<<iterator->second<<std::endl;
        
    }
    for(int i =0;i < optionVector.size();i++)
    {
        wManager->getWindow(optionVector.at(i))->setVisible(false);
        if(optionVector.at(i)!="SettingsButton")
            menuObjectMap[optionVector.at(i)]->closeMenu();
       
    }
    wManager->getWindow(menuName+"/BackButton")->setVisible(false);
    wManager->getWindow("SubMenuLabel")->setVisible(false);

    //menuObjectMap["LobbyButton"].closeMenu();
}
void MenuController::showMenu( std::string menu )
{
    clearMenu();
    std::map<std::string, std::string>::const_iterator itr;
    int buttoncnt = 1;

    item = wManager->getWindow(menu);
    
    item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    item->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.4,0)));

    CEGUI::Window *label = wManager->getWindow("SubMenuLabel");
    label->setSize(CEGUI::UVector2(CEGUI::UDim(0.19, 0), CEGUI::UDim(0.05, 0)));
    label->setPosition(CEGUI::UVector2(CEGUI::UDim(0.38,0), CEGUI::UDim(0.38,0)));
    label->setText("   "+item->getText());
    label->setVisible(true);
    label->setFont("Nimbus-12"); 
    sheet->addChildWindow(label);

    for(itr = subOptionMap.begin();itr != subOptionMap.end();itr++)
    {
        if(itr->second == menu )
        {
            buttoncnt++;
            item = wManager->getWindow(itr->first);
            item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
            item->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.05*buttoncnt+.35,0)));
            item->setVisible(true);
        }
    }
    buttoncnt++;

    item = wManager->getWindow(menuName+"/BackButton");
    item->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    item->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0), CEGUI::UDim(0.05*buttoncnt+0.35,0)));
    item->setVisible(true);
    CEGUI::System::getSingleton().setGUISheet(sheet);

}

bool MenuController::buttonClicked(const CEGUI::EventArgs& evt)
{
    CEGUI::MouseEventArgs& mouseEventArgs = (CEGUI::MouseEventArgs&) evt;
    std::string name = mouseEventArgs.window->getName().c_str();

    if(name == "Information/okButton")
    {
        informationPanel->setVisible(false);
        return true;
    }

    currentMenu = name;
    
    if(menuObjectMap[name] == NULL )
        showMenu(name);
    else
        menuObjectMap[name]->showMenu();


    return true;
}


void MenuController::back()
{
    if( subOptionMap[currentMenu] != "" )
        this->showMenu( subOptionMap[currentMenu] );
    else
        this->showMain();

}
void MenuController::showMessage(std::string message)
{
    wManager->getWindow("Information/infoText")->setText(message);
    informationPanel->setVisible(true);
    CEGUI::System::getSingleton().getGUISheet()->addChildWindow(informationPanel);   
}

void MenuController::showLoading()
{
    CEGUI::Window* windowSheet = wManager->createWindow("DefaultWindow","Loading");
    CEGUI::Window *img = wManager->createWindow("TaharezLook/StaticImage","LoadingImage");
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("LoadingImg","images/team2.png");
    img->setProperty("FrameEnabled","False");
    img->setProperty("Image", "set:LoadingImg image:full_image");
    img->setVisible(true);
    img->show();
    windowSheet->addChildWindow(img);
    CEGUI::System::getSingleton().setGUISheet(windowSheet);
}

void MenuController::hideLoading()
{
    wManager->destroyWindow("Loading");
    CEGUI::System::getSingleton().setGUISheet(sheet);

}