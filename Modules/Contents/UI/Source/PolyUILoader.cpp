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

UIDefinition::UIDefinition(TiXmlDocument* xmlDescription) : Entity() {
    this->xmlDescription = new TiXmlDocument(*xmlDescription);
}

UIDefinition::~UIDefinition() {
    delete xmlDescription;
}

UIButton* UILoader::buildButton(TiXmlElement *data) {
    Number xPos, yPos, width, height = -1;
    const char* label;

    int result;
    result = data->QueryFloatAttribute("xPos", &xPos);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Button XML node has no xPos attribute of type Number.");
    }

    result = data->QueryFloatAttribute("yPos", &yPos);
    if(result != TIXML_SUCCESS) {
        throw UILoaderError("Button XML node has no yPos attribute of type Number.");
    }

    result = data->QueryFloatAttribute("width", &width);
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

    rval->setPosition(xPos, yPos);

    return rval;
}
