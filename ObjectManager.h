#pragma once
#include "Object.h"
#include <vector>
#include <string>
#include <map>
#include <stdio.h>

#define OB(a) ObjectManager::getObj(a) 

class ObjectManager
{
public:
	static void createNew(std::string tag);
	static void createNewAndInitialize(std::string tag, SMesh* mesh, GDX_OBJECT_TYPE type);
	static void deleteObj(std::string tag);
	static Object* getObj(std::string tag);
	static Object* releaseObjFromList(std::string tag);
	static void addObjToList(Object** objPtr);

	static void updateAll();
	static void renderAll();

	static void updateGroup();
	static void renderGroup();

	static void updateGroup(std::string group);
	static void renderGroup(std::string group);

	static void useGroup(std::string groupName);
	static void useDefaultGroup();
	static void deleteGroup(const std::string &groupName);


private:
	static std::map<std::string, std::map<std::string, Object*> > mObjList;
	static std::string mGroup;
};
