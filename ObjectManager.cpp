#include "ObjectManager.h"

#include "LOG.h"

std::map<std::string, std::map<std::string, Object*> > ObjectManager::mObjList;
std::string ObjectManager::mGroup = "default";

void ObjectManager::createNew(std::string tag)
{
	if (mObjList[mGroup][tag] != nullptr)
		LOG::ERR("Error: Object with tag: %s, already exist!\n", tag.c_str());
	else {
		Object* tempObj = new Object();
		mObjList[mGroup][tag] = tempObj;
	}

}

void ObjectManager::createNewAndInitialize(std::string tag, SMesh* mesh, GDX_OBJECT_TYPE type, std::string texturePath)
{
	if (mObjList[mGroup][tag] != nullptr)
		LOG::ERR("Error: Object with tag: %s, already exist!\n", tag.c_str());
	else {
		Object* tempObj = new Object();
		tempObj->init(mesh, type, texturePath);
		mObjList[mGroup][tag] = tempObj;
	}

}

void ObjectManager::deleteObj(std::string tag)
{
	if (mObjList[mGroup][tag] != nullptr) {
		delete mObjList[mGroup][tag];
		mObjList[mGroup][tag] = nullptr;
	}
}

Object* ObjectManager::getObj(std::string tag)
{
	if (mObjList[mGroup][tag] == nullptr)
		LOG::ERR("Error: There is no object with tag: %s in this group!\n", tag.c_str());
	else {
		return mObjList[mGroup][tag];
	}
	return nullptr;
}

Object* ObjectManager::releaseObjFromList(std::string tag)
{
	return nullptr;
}

void ObjectManager::addObjToList(Object** objPtr)
{
}

void ObjectManager::useGroup(std::string groupName)
{
	mGroup = groupName;
}

void ObjectManager::useDefaultGroup()
{
	mGroup = "default";
}

void ObjectManager::deleteGroup(const std::string& groupName)
{
}

void ObjectManager::updateAll()
{
	for (std::map<std::string, std::map<std::string, Object*> >::iterator it =
		mObjList.begin(); it != mObjList.end(); it++) {

		for (std::map<std::string, Object*>::iterator it2 =
			it->second.begin(); it2 != it->second.end(); it2++) {
			it2->second->update();

		}

	}
}

void ObjectManager::renderAll()
{
	for (std::map<std::string, std::map<std::string, Object*> >::iterator it =
		mObjList.begin(); it != mObjList.end(); it++) {

		for (std::map<std::string, Object*>::iterator it2 =
			it->second.begin(); it2 != it->second.end(); it2++) {
			it2->second->draw();
		}

	}
}


void ObjectManager::updateGroup(std::string group)
{
	for (std::map<std::string, Object*>::iterator it =
		mObjList[group].begin(); it != mObjList[group].end(); it++) {
		it->second->update();

	}
}

void ObjectManager::renderGroup(std::string group)
{
	for (std::map<std::string, Object*>::iterator it =
		mObjList[group].begin(); it != mObjList[group].end(); it++) {
		it->second->draw();

	}
}


void ObjectManager::updateGroup()
{
	updateGroup(mGroup);
}

void ObjectManager::renderGroup()
{
	renderGroup(mGroup);
}