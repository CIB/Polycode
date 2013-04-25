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

UIButton* UILoader::buildButton(ObjectEntry *data) {
    Number width, height = -1;
    String label = "";

	bool result = data->readNumber("width", &width);
    if(!result) {
        throw UILoaderError("Button XML node has no width attribute of type Number.");
    } else if(width < 0) {
        throw UILoaderError("Button XML node has negative width.");
    }

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

	bool result = data->readNumber("width", &width);
    if(!result) {
        throw UILoaderError("Box XML node has no width attribute of type Number.");
    } else if(width < 0) {
        throw UILoaderError("Box XML node has negative width.");
    }

	result = data->readNumber("height", &height);
    if(!result) {
        throw UILoaderError("Box XML node has no height attribute of type Number.");
    } else if(height < 0) {
        throw UILoaderError("Box XML node has negative height.");
    }

	result = data->readNumber("borderTop", &borderTop);
    if(!result) {
        throw UILoaderError("Box XML node has no borderTop attribute of type Number.");
    } else if(borderTop < 0) {
        throw UILoaderError("Box XML node has negative borderTop.");
    }

	result = data->readNumber("borderBottom", &borderBottom);
    if(!result) {
        throw UILoaderError("Box XML node has no borderBottom attribute of type Number.");
    } else if(borderBottom < 0) {
        throw UILoaderError("Box XML node has negative borderBottom.");
    }

	result = data->readNumber("borderLeft", &borderLeft);
    if(!result) {
        throw UILoaderError("Box XML node has no borderLeft attribute of type Number.");
    } else if(borderLeft < 0) {
        throw UILoaderError("Box XML node has negative borderLeft.");
    }

	result = data->readNumber("borderRight", &borderRight);
    if(!result) {
        throw UILoaderError("Box XML node has no borderRight attribute of type Number.");
    } else if(borderRight < 0) {
        throw UILoaderError("Box XML node has negative borderRight.");
    }

    result = data->readString("image", &imageFile);
    if(!result) {
        throw UILoaderError("Box XML node has no image file specified.");
    }

    UIBox* rval = new UIBox(imageFile, borderTop, borderRight, borderBottom, borderLeft, width, height);
    return rval;
}

UIElement* UILoader::loadObject(Object *object) {
	if(object->root.name != "Elements") {
		throw UILoaderError("XML Document contains no UI elements.");
	}

	// An empty root container to store all children in.
	UIElement *rootContainer = new UIElement();

	loadObjectEntry(&object->root, rootContainer);

	return rootContainer;
}

UIElement* UILoader::loadObjectEntry(ObjectEntry *node, Entity* parent) {
	for(int i=0; i < node->children.size(); i++)
	{
		ObjectEntry* nextElement = node->children[i];
		if(nextElement->type != ObjectEntry::CONTAINER_ENTRY) {
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
	}
}
