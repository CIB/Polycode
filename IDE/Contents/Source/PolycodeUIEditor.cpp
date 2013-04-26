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

#include "PolycodeUIEditor.h"
#include "PolyUILoader.h"
#include "PolyObject.h"

PolycodeUIEditor::PolycodeUIEditor() : PolycodeEditor(true) {
	editorType = "PolycodeUIEditor";
}

PolycodeUIEditor::~PolycodeUIEditor() {
}

bool PolycodeUIEditor::openFile(OSFileEntry filePath) {
	isLoading = true;
	
	if(uiDef.loadFromXML(filePath.fullPath)) {
		UILoader loader; 
		root = loader.loadObject(&uiDef);
	}

	root->setPosition(100, 100);
	addChild(root);
	
	PolycodeEditor::openFile(filePath);
	
	isLoading = false;	
	return true;
}

void PolycodeUIEditor::saveFile() {
	uiDef.saveToXML(filePath);
	setHasChanges(false);
}

void PolycodeUIEditor::Resize(int x, int y) {
	//root->Resize(x, y);
	PolycodeEditor::Resize(x,y);
}

void PolycodeUIEditor::handleEvent(Event *event) {
	PolycodeEditor::handleEvent(event);
}