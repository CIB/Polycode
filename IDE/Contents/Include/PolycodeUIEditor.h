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

#pragma once

#include "PolycodeEditor.h"
#include <PolycodeUI.h>
#include <Polycode.h>
#include <PolyUILoader.h>

using namespace Polycode;

class PolycodeUIEditor : public PolycodeEditor {
public:
	PolycodeUIEditor();
	virtual ~PolycodeUIEditor();
	
	bool openFile(OSFileEntry filePath);
	void Resize(int x, int y);
	void saveFile();
	
	void handleEvent(Event *event);
protected:
	void clearSelection();
	void updateSelection(ScreenEntity *entity);
	void updateSelection(Entity *entity);
	void updateTree(UILoader* loader, UITree *treeNode, Entity *entity);

	UIElement selection;

	// Container for all children of the UI
	UIElement *root;

	// Tree container displaying the UI structure
	UITreeContainer *uiTree;

	// Definition of the UI as Object
	Object uiDef;

	bool isLoading;

};

class PolycodeUIEditorFactory : public PolycodeEditorFactory {
public:
	PolycodeUIEditorFactory() : PolycodeEditorFactory() {
		extensions.push_back("uidef");				
	}
	PolycodeEditor *createEditor() { return new PolycodeUIEditor(); }
};
