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

UIButton* UILoader::buildButton(TiXmlElement *data) {
    Number width, height = -1;
    const char* label;

	int result = data->QueryFloatAttribute("width", &width);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Button XML node has no width attribute of type Number.");
    } else if(width < 0) {
        throw UILoaderError("Button XML node has negative width.");
    }

    label = data->Attribute("label");
    if(label == NULL) {
        label = "";
    }

    bool hasHeight = data->QueryFloatAttribute("height", &height) == TIXML_SUCCESS;
    if(hasHeight && height < 0) {
        throw UILoaderError("Button XML node has negative height.");
    }

    UIButton* rval;
    if(hasHeight) {
        rval = new UIButton(String(label), width, height);
    } else {
        rval = new UIButton(String(label), width);
    }

    return rval;
}

UIBox* UILoader::buildBox(TiXmlElement *data) {
    Number width, height, borderTop, borderBottom, borderLeft, borderRight = -1;
    const char* imageFile;

	int result = data->QueryFloatAttribute("width", &width);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Box XML node has no width attribute of type Number.");
    } else if(width < 0) {
        throw UILoaderError("Box XML node has negative width.");
    }

	result = data->QueryFloatAttribute("height", &height);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Box XML node has no height attribute of type Number.");
    } else if(height < 0) {
        throw UILoaderError("Box XML node has negative height.");
    }

	result = data->QueryFloatAttribute("borderTop", &borderTop);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Box XML node has no borderTop attribute of type Number.");
    } else if(borderTop < 0) {
        throw UILoaderError("Box XML node has negative borderTop.");
    }

	result = data->QueryFloatAttribute("borderBottom", &borderBottom);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Box XML node has no borderBottom attribute of type Number.");
    } else if(borderBottom < 0) {
        throw UILoaderError("Box XML node has negative borderBottom.");
    }

	result = data->QueryFloatAttribute("borderLeft", &borderLeft);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Box XML node has no borderLeft attribute of type Number.");
    } else if(borderLeft < 0) {
        throw UILoaderError("Box XML node has negative borderLeft.");
    }

	result = data->QueryFloatAttribute("borderRight", &borderRight);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Box XML node has no borderRight attribute of type Number.");
    } else if(borderRight < 0) {
        throw UILoaderError("Box XML node has negative borderRight.");
    }

    imageFile = data->Attribute("image");
    if(imageFile == NULL) {
        throw UILoaderError("Box XML node has no image file specified.");
    }

    UIBox* rval = new UIBox(String(imageFile), borderTop, borderRight, borderBottom, borderLeft, width, height);
    return rval;
}

UIElement* UILoader::loadXML(TiXmlDocument *doc) {
	TiXmlHandle docHandle(doc); TiXmlHandle root = docHandle.FirstChildElement();
	if(!root.ToElement() || String(root.ToElement()->Value()) != "Elements") {
		throw UILoaderError("XML Document contains no UI elements.");
	}

	// An empty root container to store all children in.
	UIElement *rootContainer = new UIElement();

	loadXMLElement(root.ToElement(), rootContainer);

	return rootContainer;
}

UIElement* UILoader::loadXMLElement(TiXmlElement *node, Entity* parent) {
	TiXmlHandle elementHandle(node);http://i.imgur.com/zFiJj.jpg
	TiXmlElement *nextElement = elementHandle.FirstChild().ToElement();
	for( nextElement; nextElement!= NULL; nextElement=nextElement->NextSiblingElement())
	{
		String elementType(nextElement->Value());
		ScreenEntity *child = NULL;

		if(elementType == "Button") {
			child = buildButton(nextElement);
		} else if(elementType == "Box") {
			child = buildBox(nextElement);
		} else {
			throw UILoaderError("Element of unknown type.");
		}

		// Do some generic UIElement updates from data
		const char* elementTag = nextElement->Attribute("id");
		if(elementTag) {
			child->id = String(elementTag);
		}

		int result;
		Number xPos, yPos = -1;
		result = nextElement->QueryFloatAttribute("xPos", &xPos);
		if(result != TIXML_SUCCESS) {
			throw UILoaderError("Button XML node has no xPos attribute of type Number.");
		}

		result = nextElement->QueryFloatAttribute("yPos", &yPos);
		if(result != TIXML_SUCCESS) {
			throw UILoaderError("Button XML node has no yPos attribute of type Number.");
		}
		child->setPosition(xPos, yPos);

		// Recursively search the child element for grandchildren,
		// and add them as children of the child.
		loadXMLElement(nextElement, child);
		parent->addChild(child);
	}
}
