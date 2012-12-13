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

#include <Shapes/OgreBulletCollisionsCompoundShape.h>

#include "MazeBuilder.h"
#include "PhysicsShapeHandler.h"
#include "Util/Logger.h"
#include "Util/ConfigReader.h"
#include "NetworkUtil/MessageStructure.h"

MazeBuilder::MazeBuilder(std::string filename,Ogre::SceneManager* sceneManager,OgreBulletDynamics::DynamicsWorld* dynamics)
{
    this->filename=filename;
    this->sceneManager=sceneManager;
    this->dynamics=dynamics;
    mapReader=new MapReader(filename);
}

MazeBuilder::~MazeBuilder()
{
	delete mapReader;
}

void MazeBuilder::readMap()
{
    Logger::getSingleton()->addLine("Reading map...");
    mapReader->readMap();
}

Entity* MazeBuilder::createHorizontalPlane(std::string name,std::string materialName,Ogre::Vector3 position,float xRotation,float xLen,float zLen)
{
    OgreBulletDynamics::RigidBody* planeRigid = new OgreBulletDynamics::RigidBody(name + "_rigid", dynamics);
    Plane topPlane(Ogre::Vector3::UNIT_Y,0);
	float div=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsDouble("Texture","floor_ceil-divider");
    MeshManager::getSingleton().createPlane(name,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,topPlane,xLen,zLen,xLen/30.,zLen/30,true,1,xLen/div*1.0,zLen/div*1.0,Ogre::Vector3(0,0,1));
    Entity* entity=sceneManager->createEntity(name);
    entity->setMaterialName(materialName);
    SceneNode* node=sceneManager->getRootSceneNode()->createChildSceneNode(name+"_node");
    node->attachObject(entity);
    node->setPosition(position);
    if(xRotation==0)
    {
        OgreBulletCollisions::CollisionShape* staticShape= new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,.5,0), 0);
        planeRigid->setStaticShape(staticShape, .1, 5);
        planeRigid->getBulletObject()->setUserPointer(new CollisionObjectInfo(node,planeRigid,CollisionObjectTypes::GROUND,NULL));
    }
    else
    {
        Quaternion q;
        q.FromAngleAxis(Degree(xRotation),Ogre::Vector3::UNIT_X);
        OgreBulletCollisions::BoxCollisionShape* boxShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(xLen/2., 5, zLen/2.));
        OgreBulletCollisions::CompoundCollisionShape* compound=new  OgreBulletCollisions::CompoundCollisionShape();
        compound->addChildShape(boxShape,Ogre::Vector3(0,-5,0));
        planeRigid->setStaticShape(node, compound, .6, .6, position,q);
        planeRigid->getBulletRigidBody()->setHitFraction(0);
        planeRigid->getBulletObject()->setUserPointer(new CollisionObjectInfo(node, planeRigid, CollisionObjectTypes::WALL, NULL));
    }
    
    nodeVector.push_back(node);
    rigidVector.push_back(planeRigid);
    return entity;
}

void MazeBuilder::generateMap(float wallWidth,float wallHeight,float mapScaleFactor,bool enablePhysics)
{
	this->wallHeight=wallHeight;
    double minX=999999;
    double maxX=-9999999;
    double minZ=999999;
    double maxZ=-9999999;
    Entity4Position_t* position;
    Entity* entity;
    //StaticGeometry* sg=sceneManager->createStaticGeometry("Walls");
    while((position=mapReader->getWallEntity())!=NULL)
    {
        position->startX*=mapScaleFactor;
        position->startZ*=mapScaleFactor;
        position->endX*=mapScaleFactor;
        position->endZ*=mapScaleFactor;

        double _x=position->startX;
        double _z=position->startZ;
        
        for(int i=0;i<2;i++)
        {
            if(_x<minX)
                minX=_x;
            if(_x>maxX)
                maxX=_x;
            if(_z<minZ)
                minZ=_z;
            if(_z>maxZ)
                maxZ=_z;
            _x=position->endX;
            _z=position->endZ;
        }


        float xDistance=position->startX-position->endX;
        float zDistance=position->startZ-position->endZ;
        if(!xDistance && !zDistance)
            continue;

        float hypotenuse=sqrt(pow((float)xDistance,2.0f)+pow((float)zDistance,2.0f));
        float aSinRes=Ogre::Math::ASin((Real)(zDistance/(double)hypotenuse)).valueDegrees();
		if(position->startX<position->endX)
			aSinRes*=-1;

        float xPos=(fabs(xDistance)/2.0)+(position->startX<position->endX?position->startX:position->endX);
        float zPos=(fabs(zDistance)/2.0)+(position->startZ<position->endZ?position->startZ:position->endZ);
        
        std::cout<<"xlen: "<<xDistance<<std::endl;
        std::cout<<"zlen: "<<zDistance<<std::endl;
        std::cout<<"hypotenuse: "<<hypotenuse<<std::endl;
        std::cout<<"arcsin: "<<aSinRes<<" degrees"<<std::endl;
        std::cout<<"midX: "<<xPos<<std::endl;
        std::cout<<"midZ: "<<zPos<<std::endl;

        std::cout<<"wall entity start x: "<<position->startX<<std::endl;
        std::cout<<"wall entity start z: "<<position->startZ<<std::endl;
        std::cout<<"wall entity end x: "<<position->endX<<std::endl;
        std::cout<<"wall entity end z: "<<position->endZ<<std::endl;
        std::cout<<"\n--------------------------------------\n";
        entity=createWall(-xPos,zPos,aSinRes,Ogre::Vector3(hypotenuse/2.0+wallWidth,wallHeight,wallWidth),enablePhysics);
        entityVector.push_back(entity);
    }
    float xPos=(minX+maxX)/2.;
    float zPos=(minZ+maxZ)/2.;
    float xLen=Ogre::Math::Abs(maxX-minX);
    float zLen=Ogre::Math::Abs(maxZ-minZ);
    entity=createHorizontalPlane("Ceil","Environment/Ceil",Ogre::Vector3(-xPos,wallHeight,zPos),180,xLen+xLen*.01,zLen+xLen*.01);
    
    float xArr[]={minX, minX,maxX,maxX};
    float zArr[]={minZ,maxZ,minZ,maxZ};
    //entity=createTop(xArr,zArr,wallHeight-1,enablePhysics);
    entityVector.push_back(entity);
    entity=createHorizontalPlane("Floor","Environment/Floor1",Ogre::Vector3(-xPos,0,zPos),0,xLen+xLen*.01,zLen+xLen*.01);
    entityVector.push_back(entity);
    generateObjects();
    //sg->addEntity(entity,Ogre::Vector3(0,wallHeight*5,0));
    //sg->setCastShadows(true);
    //sg->build();
}

Ogre::Entity* MazeBuilder::createWall(float xPos, float zPos, float verticalRotation,Ogre::Vector3 size,bool enablePhysics)
{
    float xSize = fabs(size.x);
    float ySize = fabs(size.y);
    float zSize = fabs(size.z);
    float xTex = xSize / 15.;
    float yTex = ySize / 15.;
    static int wallNumber = 0;
    static int bodyNumber=0;
    
    Entity* _entity;
    std::string _entityName;
    _entityName = "MazeWall" + Converter::intToString(wallNumber);
    SceneNode* rootNode=sceneManager->getRootSceneNode()->createChildSceneNode("MazeWall_node_"+Converter::intToString(wallNumber));
    rootNode->setPosition(xPos,ySize/2.,zPos);
    Quaternion orientationx((Ogre::Radian)verticalRotation * 3.14 / 180, Ogre::Vector3(0, 1, 0));
    rootNode->rotate(orientationx);
    
    Plane plane(Ogre::Vector3::UNIT_Z, 0);
			float divX=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsDouble("Texture","wall-divider-x");
			float divY=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsDouble("Texture","wall-divider-y");

    for(int i=0;i<4;i++)
    {
        float ySegment = (ySize) / 30.;
        if(i==0 || i==1)
        {
            float xSegment = (xSize * 2) / 30.;
            MeshManager::getSingleton().createPlane(_entityName,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,plane, xSize*2, ySize, xSegment, ySegment, true, 1, xSegment/divX*1.0, ySegment/divY*1.0,Ogre::Vector3::UNIT_Y);
            _entity=sceneManager->createEntity(_entityName);
            _entity->setMaterialName("Environment/Wall1");
            SceneNode* _nodex=rootNode->createChildSceneNode();
            _nodex->attachObject(_entity);
            if(i==1)
            {
                _nodex->translate(0,0,-zSize,Node::TS_LOCAL);
                _nodex->rotate(Ogre::Vector3::UNIT_Y,Degree(180));
            }
            else
                _nodex->translate(0,0,zSize,Node::TS_LOCAL);
        }
        else
        {
            float zSegment = zSize / 30;
            MeshManager::getSingleton().createPlane(_entityName,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,plane,zSize*2,ySize, 1, 1, true, 1, (xSize * 2) / 30., ySegment/divY,Ogre::Vector3::UNIT_Y);
            _entity=sceneManager->createEntity(_entityName);
            SceneNode* _nodex=rootNode->createChildSceneNode();
            _nodex->attachObject(_entity);
            _entity->setMaterialName("Environment/Wall1");
            if(i==2)
            {
                _nodex->translate(-xSize,0,0,Node::TS_LOCAL);
                _nodex->rotate(Ogre::Vector3::UNIT_Y,Degree(-90));
            }
            else
            {
                _nodex->translate(xSize,0,0,Node::TS_LOCAL);
                _nodex->rotate(Ogre::Vector3::UNIT_Y,Degree(90));
            }
        }
        _entityName = "MazeWall" + Converter::intToString(wallNumber);
        _entity->setRenderQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_1);
        _entity->setCastShadows(true);
        wallNumber++;
    }
    
    if (enablePhysics)
    {
        std::string bodyName = "MazeRigidbody" + Converter::intToString(bodyNumber++);
        OgreBulletCollisions::BoxCollisionShape* boxShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(xSize,ySize/2.,zSize));
        OgreBulletDynamics::RigidBody* rigidBody = new OgreBulletDynamics::RigidBody(bodyName, dynamics);
        rigidBody->setStaticShape(rootNode,boxShape, .6, .6, Ogre::Vector3(xPos, ySize/2., zPos), orientationx);
        rigidBody->getBulletRigidBody()->setHitFraction(0);
        rigidBody->getBulletObject()->setUserPointer(new CollisionObjectInfo(rootNode, rigidBody, CollisionObjectTypes::WALL, NULL));
        rigidVector.push_back(rigidBody);
    }
    

    /*


     Ogre::ManualObject* wallObject = sceneManager->createManualObject();
     int index=0;
     wallObject->begin("Examples/brickwall", Ogre::RenderOperation::OT_TRIANGLE_LIST);
     for(int i=1;i>=-1;i-=2)
     {
         //build front & back
         
         wallObject->position(  xSize*i,    0,         zSize*i  );
         wallObject->textureCoord( xTex,0);
         wallObject->normal(0,0,i);

         wallObject->position(  xSize*i,    ySize,     zSize*i  );
         wallObject->textureCoord( xTex, yTex);
         wallObject->normal(0,0,i);

         wallObject->position( -xSize*i,    0,         zSize*i );
         wallObject->textureCoord(0,0);
         wallObject->normal(0,0,i);

         wallObject->position( -xSize*i,    ySize,     zSize*i );
         wallObject->textureCoord(0, yTex);
         wallObject->normal(0,0,i);
             
         wallObject->triangle(index,index+3,index+2);
         wallObject->triangle(index,index+1,index+3);
          
         index+=4;

         //build right & left

         wallObject->position(  -xSize*i,    0,              zSize*i  );
         wallObject->textureCoord(xTex, 0);
         wallObject->normal(-i,0,0);

         wallObject->position(  -xSize*i,    ySize,              zSize*i  );
         wallObject->textureCoord(xTex, yTex);
         wallObject->normal(-i,0,0);


         wallObject->position(  -xSize*i,    0,              -zSize*i  );
         wallObject->textureCoord(0, 0);
         wallObject->normal(-i,0,0);

         wallObject->position(  -xSize*i,    ySize,         -zSize*i  );
         wallObject->textureCoord(0,yTex);
         wallObject->normal(-i,0,0);

         wallObject->triangle(index,index+3,index+2);
         wallObject->triangle(index,index+1,index+3);
         index+=4;
     }


    wallObject->setCastShadows(true);    
    wallObject->end();
*/
	/*
    SceneNode* _node=sceneManager->getRootSceneNode()->createChildSceneNode();
    _node->setPosition(xPos,0,zPos);
    for(float _x=xSize-50;-xSize+50<_x;_x-=50)
    {
        for(int i=0;i<2;i++)
        {
            SceneNode* _nod=_node->createChildSceneNode();
            Entity* _ent=sceneManager->createEntity("Block.mesh");
            float _modelZ=_ent->getBoundingBox().getSize().z/2.*3;
            float _y=_ent->getBoundingBox().getSize().y/2*6;
            float _wallZ=zSize;
            if(i==1)
                _nod->rotate(Ogre::Vector3(0,1,0),Degree(180));
            //_nod->attachObject(_ent);
            _nod->scale(3,6,3);
            _nod->setPosition(_x,0,0);
            _nod->translate(0,_y,(_modelZ+_wallZ)*(i==0?-1:1));
        }
    }

    Quaternion orientation2((Ogre::Radian)verticalRotation * 3.14 / 180, Ogre::Vector3(0, 1, 0));
    _node->rotate(orientation2);*/
     return _entity;
     /*


    wallObject->setCastShadows(true);
    std::string meshName = "wallmesh" + Converter::intToString(wallNumber);
    std::string entityName = "MazeWall" + Converter::intToString(wallNumber);
    MeshPtr mesh = wallObject->convertToMesh(meshName);
    //mesh.getPointer()->freeEdgeList();
    //mesh.getPointer()->buildEdgeList();
    Ogre::SceneNode* sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode(entityName);
    Ogre::Entity* entity = sceneManager->createEntity(entityName, meshName);
    //Pass* pass=entity->getSubEntity(0)->getTechnique(0)->createPass();
    //pass->setDepthBias(1);
    entity->setCastShadows(true);
    entity->setRenderQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_1);
    sceneNode->attachObject(entity);
    sceneNode->setPosition(xPos, 0, zPos);
    Quaternion orientation((Ogre::Radian)verticalRotation * 3.14 / 180, Ogre::Vector3(0, 1, 0));
    sceneNode->rotate(orientation);

    if (!enablePhysics)
    {
        std::string bodyName = "MazeRigidbody" + Converter::intToString(wallNumber);
        OgreBulletCollisions::BoxCollisionShape* boxShape = PhysicsHandler::createBoxShape(entity);
        OgreBulletCollisions::CompoundCollisionShape* shape = PhysicsHandler::createCompoundShapeAndAddChild(boxShape, Ogre::Vector3(0, ySize / 2., 0));
        OgreBulletDynamics::RigidBody* rigidBody = new OgreBulletDynamics::RigidBody(bodyName,dynamics);
        rigidBody->setStaticShape(sceneNode, shape, .6, .6, Ogre::Vector3(xPos, 0, zPos), orientation);
        
        rigidBody->getBulletRigidBody()->setHitFraction(0);
        rigidBody->getBulletObject()->setUserPointer(new CollisionObjectInfo(sceneNode, rigidBody, CollisionObjectTypes::WALL, NULL));
        rigidVector.push_back(rigidBody);
    }

    wallNumber++;
    nodeVector.push_back(sceneNode);
    return entity;
     * */
}

void MazeBuilder::generateLights(bool lightsEnabled)
{
	float scaleFactor=mapReader->getScaleFactor();
    Entity2Position_t* pos;
    while((pos=mapReader->getLightEntity())!=NULL)
    {
        pos->startX*=scaleFactor;
        pos->startZ*=scaleFactor;
        createLight(-pos->startX,wallHeight,pos->startZ, lightsEnabled);
    }
}

void MazeBuilder::createLight(float x, float y, float z, bool lightsEnabled)
{
	Ogre::Vector3 lampPos(x,y,z);
	Ogre::Vector3 lightPos=lampPos;
	Ogre::Vector3 scale(15,15,15);
	
    SceneNode* lampNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    Entity* lampEntity = sceneManager->createEntity("Lamp.mesh");
    lampEntity->setCastShadows(true);
    lampNode->attachObject(lampEntity);
    lampNode->scale(scale);

	float lampHalf=lampEntity->getBoundingBox().getSize().y/2.*scale.y;
	lightPos.y-=(3*lampHalf);

    lampNode->setPosition(lampPos);

    Ogre::Light* spotLight = sceneManager->createLight();
    spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
    spotLight->setDiffuseColour(1, 1, 1);
    spotLight->setDirection(0, -1, 0);
    spotLight->setSpotlightRange(Degree(35), Degree(70));
    spotLight->setPosition(lightPos);

    Light* pointLight = sceneManager->createLight();
    pointLight->setType(Light::LT_POINT);
    pointLight->setDiffuseColour(1, 1, 1);
    pointLight->setPosition(lightPos);

    
    float lightDistance=1000;
    float _dist=ConfigReader::getConfigFile("GameSettings")->getFieldValueAsDouble("MapSettings","light-distance");
    if(_dist!=-1)
        lightDistance=_dist;
    pointLight->setAttenuation(lightDistance, 1.0, 4.5/lightDistance, 75./Ogre::Math::Pow(lightDistance,2.));

    if(!lightsEnabled)
    {
        spotLight->setVisible(false);
        pointLight->setVisible(false);
    }
    else
    {
        if(!ConfigReader::getConfigFile("GameSettings")->getFieldValueAsBool("GraphicsSettings","spot-lights"))
            spotLight->setVisible(false);
        if(!ConfigReader::getConfigFile("GameSettings")->getFieldValueAsBool("GraphicsSettings","point-lights"))
            pointLight->setVisible(false);
    }
    

    nodeVector.push_back(lampNode);
    entityVector.push_back(lampEntity);
    spotLightVector.push_back(spotLight);
    pointLightVector.push_back(pointLight);
}

void MazeBuilder::destroyAll()
{
    destroyLights();
    destroyWalls();
}

void MazeBuilder::destroyLights()
{
    for(int i=0;i<pointLightVector.size();i++)
        sceneManager->destroyLight(pointLightVector.at(i));

    for(int i=0;i<spotLightVector.size();i++)
        sceneManager->destroyLight(spotLightVector.at(i));
    
    pointLightVector.clear();
    spotLightVector.clear();
}

void MazeBuilder::destroyWalls()
{
    for(int i=0;i<rigidVector.size();i++)
        delete rigidVector.at(i);

    for(int i=0;i<nodeVector.size();i++)
    {
        nodeVector.at(i)->detachAllObjects();
        sceneManager->destroySceneNode(nodeVector.at(i));
    }

    for(int i=0;i<entityVector.size();i++)
        sceneManager->destroyEntity(entityVector.at(i));

    rigidVector.clear();
    nodeVector.clear();
    entityVector.clear();
}

MapReader* MazeBuilder::getMapReader()
{
    return mapReader;
}

void MazeBuilder::disableSpotLights()
{
    for(int i=0;i<spotLightVector.size();i++)
        spotLightVector.at(i)->setVisible(false);
}

void MazeBuilder::enableSpotLights()
{
    for(int i=0;i<spotLightVector.size();i++)
        spotLightVector.at(i)->setVisible(true);
}

void MazeBuilder::generateObjects()
{
    int modelCount=0;
    ModelEntity_t* ent;
    float mapScale=mapReader->getScaleFactor();
    while((ent=mapReader->getModelEntity())!=NULL)
    {
        Entity* entity;
        Ogre::Vector3 scale(1,1,1);
        Ogre::Vector3 position(-ent->entityPosition.startX*mapScale, 0, ent->entityPosition.startZ*mapScale);
        float defaultRotation=-90;
		bool physicsEnabled=true;
		bool autoAlign=false;
        if(std::string(ent->modelName)=="OgreHead")
        {
			defaultRotation+=270;
            entity=sceneManager->createEntity("Computer.mesh");
            scale=Ogre::Vector3(7,7,7);
			//position.y=0;
			autoAlign=true;
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;



            //position.y+=entity->getBoundingBox().getSize().y/2.*scale.y;
            /*
            SceneNode* boxNode=sceneManager->getRootSceneNode()->createChildSceneNode("boxNode"+Converter::intToString(modelCount));
            boxNode->setPosition(position);
            boxNode->attachObject(entity);
            boxNode->scale(scale);
            boxNode->rotate(Ogre::Vector3::UNIT_Y,Degree(defaultRotation+ent->direction));
            std::string boxBody=std::string(ent->modelName)+"_box_"+Converter::intToString(modelCount++);
            OgreBulletCollisions::BoxCollisionShape* boxShape=PhysicsHandler::createBoxShape(entity,scale);
            OgreBulletDynamics::RigidBody* boxRigid=new OgreBulletDynamics::RigidBody(boxBody,dynamics);
            boxRigid->setShape(boxNode,boxShape,1,.7,2,position,boxNode->getOrientation());
            boxRigid->getBulletRigidBody()->setGravity(btVector3(0,-200,0));
            boxRigid->getBulletObject()->setUserPointer(new CollisionObjectInfo(boxNode, boxRigid, CollisionObjectTypes::DYNAMIC_OBJECT, NULL));

            entityVector.push_back(entity);
            rigidVector.push_back(boxRigid);
            nodeVector.push_back(boxNode);
            
            continue;
             */
        }
        else if(std::string(ent->modelName)=="Servers")
        {
            entity=sceneManager->createEntity("Servers.mesh");
            scale=Ogre::Vector3(7,7,7);
			defaultRotation+=90;
			position.y=0;
        }
        else if(std::string(ent->modelName)=="Statue")
        {
            entity=sceneManager->createEntity("Statue.mesh");
            scale=Ogre::Vector3(10,10,10);
			position.y=0;
        }
        else if(std::string(ent->modelName)=="Camera")
        {
            entity=sceneManager->createEntity("Camera.mesh");
            defaultRotation+=90;
            scale=Ogre::Vector3(15,15,15);
			position.y=getWallHeight();
			physicsEnabled=false;
        }
        else if(std::string(ent->modelName)=="Lab")
        {
            entity=sceneManager->createEntity("Lab.mesh");
            defaultRotation+=180;
            scale=Ogre::Vector3(7,7,7);
            position.y=0;
        }
        else if(std::string(ent->modelName)=="ComputerDesk")
        {
            entity=sceneManager->createEntity("ComputerDesk.mesh");
            defaultRotation+=180;
            scale=Ogre::Vector3(7,7,7);
            position.y=0;
        }
        else if(std::string(ent->modelName)=="Robot")
        {
            entity=sceneManager->createEntity("Box3.mesh");
            defaultRotation+=270;
            scale=Ogre::Vector3(15,15,15);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="FireEx")
        {
            entity=sceneManager->createEntity("FireEx.mesh");
			scale=Ogre::Vector3(5,5,5);
			position.y=0;
        }
        else if(std::string(ent->modelName)=="AirTunnel")
        {
            entity=sceneManager->createEntity("AirTunnel.mesh");
			scale=Ogre::Vector3(10,10,10);
			position.y=getWallHeight();
			physicsEnabled=false;
        }
        else if(std::string(ent->modelName)=="CeilMonitors")
        {
            entity=sceneManager->createEntity("CeilMonitors.mesh");
			scale=Ogre::Vector3(15,15,15);
			position.y=getWallHeight();
			physicsEnabled=false;
        }
        else if(std::string(ent->modelName)=="FuelTank")
        {
            entity=sceneManager->createEntity("FuelTank.mesh");
            defaultRotation+=270;
            scale=Ogre::Vector3(8,8,8);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Stairs")
        {
            entity=sceneManager->createEntity("Stairs.mesh");
            defaultRotation+=270;
            scale=Ogre::Vector3(12,12,12);
            position.y=0;
        }
        else if(std::string(ent->modelName)=="ShieldStairs")
        {
            entity=sceneManager->createEntity("ShieldStairs.mesh");
            scale=Ogre::Vector3(15,15,15);
            position.y=0;
        }
        else if(std::string(ent->modelName)=="InfoComputer")
        {
            entity=sceneManager->createEntity("InfoComputer.mesh");
            defaultRotation+=270;
            scale=Ogre::Vector3(7,7,7);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Computer")
        {
            entity=sceneManager->createEntity("Computer.mesh");
            defaultRotation+=270;
            scale=Ogre::Vector3(7,7,7);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
		else if(std::string(ent->modelName)=="Container")
        {
            entity=sceneManager->createEntity("Container.mesh");
            scale=Ogre::Vector3(15,15,15);
            position.y+=entity->getBoundingBox().getSize().y/2.*scale.y;
		}
        else if(std::string(ent->modelName)=="Shield")
        {
            entity=sceneManager->createEntity("Shield.mesh");
            scale=Ogre::Vector3(15,15,15);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Box1")
        {
            entity=sceneManager->createEntity("Box1.mesh");
            scale=Ogre::Vector3(12,12,12);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Box2")
        {
            entity=sceneManager->createEntity("Box2.mesh");
            scale=Ogre::Vector3(12,12,12);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Box3")
        {
            entity=sceneManager->createEntity("Box3.mesh");
            scale=Ogre::Vector3(12,12,12);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Box4")
        {
            entity=sceneManager->createEntity("Box4.mesh");
            scale=Ogre::Vector3(12,12,12);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Box5")
        {
            entity=sceneManager->createEntity("Box5.mesh");
            scale=Ogre::Vector3(7,7,7);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Box6")
        {
            entity=sceneManager->createEntity("Box6.mesh");
            scale=Ogre::Vector3(15,15,15);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
        else if(std::string(ent->modelName)=="Box7")
        {
            entity=sceneManager->createEntity("Box7.mesh");
            scale=Ogre::Vector3(13,13,13);
            position.y=entity->getBoundingBox().getSize().y/2.*scale.y;
        }
		else
		{
			Logger::getSingleton()->addLine("MazeBuilder: unknown object read from map file!",true);
			continue;
		}
		/*
		if(autoAlign)
		{
			if((((int)ent->direction)%90)<5)
			{
				Ogre::Vector3 startPos=position;
				Ogre::Vector3 endPos=Quaternion(Degree(defaultRotation-90+ent->direction),Ogre::Vector3::UNIT_Y)*Ogre::Vector3(0,0,1)*500;
				Ogre::Vector3 hitPos;
				if(PhysicsHandler::performRaycastTest(startPos,endPos,dynamics,NULL,&hitPos))
				{
					float _y=position.y;
					position=hitPos;
					float halfZ=entity->getBoundingBox().getSize().z/2.*scale.z;
					position+=((Quaternion(Degree(defaultRotation+ent->direction),Ogre::Vector3::UNIT_Y)*Ogre::Vector3(1,0,1))*halfZ);
					position.y=_y;
				}
			}
		}
		*/

        SceneNode* modelNode=sceneManager->getRootSceneNode()->createChildSceneNode();
        modelNode->setPosition(position);
        modelNode->attachObject(entity);
        modelNode->scale(scale);
        modelNode->rotate(Ogre::Vector3::UNIT_Y,Degree(defaultRotation+ent->direction));

		//DEBUG
		node=modelNode;

		if(physicsEnabled)
		{
			std::string bodyName=std::string(ent->modelName)+"_"+Converter::intToString(modelCount);
			btScaledBvhTriangleMeshShape* _btShape=PhysicsHandler::createScaledTriangleMeshShape(entity,scale);
			OgreBulletDynamics::RigidBody* _rigid=PhysicsHandler::addStaticMeshRigidBody(bodyName,_btShape,dynamics,position,modelNode->getOrientation());
			_rigid->setKinematicObject(true);
			_rigid->getBulletRigidBody()->setFriction(.1);
			_rigid->getBulletRigidBody()->setRestitution(0);
			_rigid->getBulletObject()->setUserPointer(new CollisionObjectInfo(modelNode, _rigid, CollisionObjectTypes::STATIC_OBJECT, NULL));
			rigidVector.push_back(_rigid);
		}


        entityVector.push_back(entity);
        nodeVector.push_back(modelNode);
    }
}

float MazeBuilder::getWallHeight()
{
	return wallHeight;
}