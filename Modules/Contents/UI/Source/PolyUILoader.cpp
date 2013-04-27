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

	loadObjectEntry(&rootDocument->root, rootContainer);
	entriesByEntities[rootContainer] = &rootDocument->root;
	entitiesByEntries[&rootDocument->root] = rootContainer;

	return rootContainer;
}

UIElement* UILoader::loadObjectEntry(ObjectEntry *node, Entity* parent) {
	for(int i=0; i < node->children.size(); i++)
	{
		ObjectEntry* nextElement = node->children[i];
		if( // If the child is an attribute, rather than a tag, skip it.
			nextElement->type != ObjectEntry::CONTAINER_ENTRY &&
			nextElement->type != ObjectEntry::ARRAY_ENTRY
		) {
			continue;
		}
		
		String elementType = nextElement->name;
		ScreenEntity *child = NULL;
		
		if(elementType == "Button") {
			child = buildButton(nextElement);
		} else if(elementType == "Box") {
			child = buildBox(nextElement);
		} else {
			throw UILoaderError("Element of unknown type.");
		}

		// Do some generic UIElement updates from data
		String elementTag;
		bool result = nextElement->readString("id", &elementTag);
		if(result) {
			child->id = String(elementTag);
		}

		Number xPos, yPos = -1;
		result = nextElement->readNumber("xPos", &xPos);
		if(!result) {
			throw UILoaderError("Button XML node has no xPos attribute of type Number.");
		}

		result = nextElement->readNumber("yPos", &yPos);
		if(!result) {
			throw UILoaderError("Button XML node has no yPos attribute of type Number.");
		}
		child->setPosition(xPos, yPos);

		// Recursively search the child element for grandchildren,
		// and add them as children of the child.
		loadObjectEntry(nextElement, child);
		parent->addChild(child);

		entriesByEntities[child] = nextElement;
		entitiesByEntries[nextElement] = child;
	}
}

Entity* UILoader::getLoadedEntity(ObjectEntry *entry) {
	return this->entitiesByEntries[entry];
}

ObjectEntry* UILoader::getLoadedFrom(Entity* entity) {
	return this->entriesByEntities[entity];
}