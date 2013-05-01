/*
 Copyright (C) 2012 by Ivan Safrin

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */


#include "PolyUILoader.h"

using namespace Polycode;

UILoader::UILoader() {
	rootDocument = NULL;
}

Number UILoader::readNumberOrError(ObjectEntry *from, String key, String errorWhere) {
	Number rval = -1;
	bool result = from->readNumber(key, &rval);
	if(result) {
		return rval;
	} else {
		throw UILoaderError(errorWhere + ": "+from->name+"["+key+"] must be a number.");
	}
}

Number UILoader::readNumberNonNegativeOrError(ObjectEntry *from, String key, String errorWhere) {
	Number rval = readNumberOrError(from, key, errorWhere);
	if(rval < 0) {
		throw UILoaderError(errorWhere + ": "+from->name+"["+key+"] must be non-negative.");
	}
	return rval;
}

String UILoader::readStringOrError(ObjectEntry *from, String key, String errorWhere) {
	String rval = "";
	bool result = from->readString(key, &rval);
	if(result) {
		return rval;
	} else {
		throw UILoaderError(errorWhere + ": "+from->name+"["+key+"] must be a String.");
	}
}

UIButton* UILoader::buildButton(ObjectEntry *data) {
    Number width, height = -1;
    String label = "";
	String errorWhere = "Building Button";

	width = readNumberNonNegativeOrError(data, "width", errorWhere);

	// If "label" doesn't exist or is of wrong type, &label
	// will keep pointing to an empty string.
    data->readString("label", &label);

    bool hasHeight = data->readNumber("height", &height);
    if(hasHeight && height < 0) {
        throw UILoaderError("Button XML node has negative height.");
    }

    UIButton* rval;
    if(hasHeight) {
        rval = new UIButton(label, width, height);
    } else {
        rval = new UIButton(label, width);
    }

    return rval;
}

UIBox* UILoader::buildBox(ObjectEntry *data) {
    Number width, height, borderTop, borderBottom, borderLeft, borderRight = -1;
    String imageFile;
	String errorWhere = "Building Box";

	width = readNumberNonNegativeOrError(data, "width", errorWhere);
	height = readNumberNonNegativeOrError(data, "height", errorWhere);
	borderTop = readNumberNonNegativeOrError(data, "borderTop", errorWhere);
	borderBottom = readNumberNonNegativeOrError(data, "borderBottom", errorWhere);
	borderLeft = readNumberNonNegativeOrError(data, "borderLeft", errorWhere);
	borderRight = readNumberNonNegativeOrError(data, "borderRight", errorWhere);

	imageFile = readStringOrError(data, "image", errorWhere);

    UIBox* rval = new UIBox(imageFile, borderTop, borderRight, borderBottom, borderLeft, width, height);
    return rval;
}

vector<ObjectEntry> UILoader::getTypeAttributes(String type) {
	std::vector<ObjectEntry> rval;
	if(type == "Box") {
		ObjectEntry obj;
		obj.name = "xPos"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "yPos"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "width"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "height"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "borderTop"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "borderBottom"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "borderLeft"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "borderRight"; obj.type = ObjectEntry::INT_ENTRY; obj.intVal = 10; rval.push_back(obj);
		obj.name = "image"; obj.type = ObjectEntry::STRING_ENTRY; obj.stringVal = ""; rval.push_back(obj);
		obj.name = "id"; obj.type = ObjectEntry::STRING_ENTRY; obj.stringVal = ""; rval.push_back(obj);
	}
	return rval;
}

UIElement* UILoader::loadObject(Object *object) {
	rootDocument = object; // TODO: deepcopy object into rootDocument to avoid
						   // memory management issues
	if(rootDocument->root.name != "Elements") {
		throw UILoaderError("XML Document contains no UI elements.");
	}

	entriesByEntities.clear();
	entitiesByEntries.clear();

	// An empty root container to store all children in.
	UIElement *rootContainer = new UIElement();

	loadObjectEntries(&rootDocument->root, rootContainer);
	entriesByEntities[rootContainer] = &rootDocument->root;
	entitiesByEntries[&rootDocument->root] = rootContainer;

	return rootContainer;
}

ScreenEntity* UILoader::loadObjectEntry(ObjectEntry *node, Entity *parent) {
	String elementType = node->name;
	ScreenEntity* child;
		
	if(elementType == "Button") {
		child = buildButton(node);
	} else if(elementType == "Box") {
		child = buildBox(node);
	} else {
		throw UILoaderError("Element of unknown type.");
	}

	// Do some generic UIElement updates from data
	String elementTag;
	bool result = node->readString("id", &elementTag);
	if(result) {
		child->id = String(elementTag);
	}

	Number xPos, yPos = -1;
	result = node->readNumber("xPos", &xPos);
	if(!result) {
		throw UILoaderError("Button XML node has no xPos attribute of type Number.");
	}

	result = node->readNumber("yPos", &yPos);
	if(!result) {
		throw UILoaderError("Button XML node has no yPos attribute of type Number.");
	}
	child->setPosition(xPos, yPos);

	// Recursively search the child element for grandchildren,
	// and add them as children of the child.
	loadObjectEntries(node, child);
	parent->addChild(child);

	entriesByEntities[child] = node;
	entitiesByEntries[node] = child;

	return child;
}

void UILoader::loadObjectEntries(ObjectEntry *parentNode, Entity *parent) {
	for(int i=0; i < parentNode->children.size(); i++)
	{
		ObjectEntry* nextElement = parentNode->children[i];
		if( // If the child is an attribute, rather than a tag, skip it.
			nextElement->type != ObjectEntry::CONTAINER_ENTRY &&
			nextElement->type != ObjectEntry::ARRAY_ENTRY
		) {
			continue;
		}

		loadObjectEntry(nextElement, parent);
	}
}

void UILoader::deleteAllLoadedChildren(Entity* entity) {
	for(int i=0; i<entity->getNumChildren(); i++) {
		Entity* child = entity->getChildAtIndex(i);
		if(entriesByEntities[child]) {
			deleteAllLoadedChildren(child);
			delete child;
			entity->removeChild(child);
			i--; // Hack to avoid breaking the iterator
		}
	}
}

ScreenEntity* UILoader::reloadObjectEntry(ObjectEntry* node) {
	Entity* toReplace = this->getLoadedEntity(node);
	if(!toReplace) return NULL;
	
	// TODO: cleanly remove all children of toReplace
	// from entriesByEntities
	this->deleteAllLoadedChildren(toReplace);
	Entity* parent = toReplace->getParentEntity();
	parent->removeChild(toReplace);
	delete toReplace;

	return loadObjectEntry(node, parent);
}

Entity* UILoader::getLoadedEntity(ObjectEntry *entry) {
	return this->entitiesByEntries[entry];
}

ObjectEntry* UILoader::getLoadedFrom(Entity* entity) {
	return this->entriesByEntities[entity];
}

std::vector<Entity*> *UILoader::getAllLoadedEntities() {
	std::vector<Entity*> *rval = new std::vector<Entity*>();
	for(std::map<Entity*, ObjectEntry*>::iterator i = entriesByEntities.begin(); i != entriesByEntities.end(); i++) {
		rval->push_back(i->first);
	}
	return rval;
}
