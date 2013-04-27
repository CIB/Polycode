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
	uiTree = new UITreeContainer("boxIcon.png", "Structure", 400, 400);
	uiTree->setPosition(this->getWidth() - uiTree->getWidth() - 5, 10);
	addChild(uiTree);

	selection.setPosition(0, 0);
	addChild(&selection);

	uiTree->addEventListener(this, InputEvent::EVENT_MOUSEUP);
}

PolycodeUIEditor::~PolycodeUIEditor() {
	delete uiTree;
}

void PolycodeUIEditor::updateTree(UILoader *loader, UITree *treeNode, Entity *entity) {
	String label = entity->id;
	if(label == "") {
		label = "Unknown";
	}
	treeNode->setLabelText(entity->id);
	treeNode->setUserData(entity);
	treeNode->clearTree();
	
	for(int i=0; entity->getChildAtIndex(i); i++) {
		Entity *child = entity->getChildAtIndex(i);
		if(loader->getLoadedFrom(child)) {
			// Only display nodes that were loaded from an UI definition.
			updateTree(loader, treeNode->addTreeChild("box_icon.png", "", NULL), child);
		}
	}
}

bool PolycodeUIEditor::openFile(OSFileEntry filePath) {
	isLoading = true;
	
	if(!uiDef.loadFromXML(filePath.fullPath)) {
		return false;
	}
	UILoader loader; 
	root = loader.loadObject(&uiDef);
	if(root->id == "") {
		root->id = "Root";
	}

	UITree *rootNode = uiTree->getRootNode();
	updateTree(&loader, rootNode, root);

	int border = 5;
	root->setPosition(border, border);
	root->setWidth(this->getWidth() - 4*border - uiTree->getWidth());
	root->setHeight(this->getHeight() - 2*border);
	addChild(root);

	// Make sure selection is still rendered on top.
	moveChildTop(&selection);
	
	PolycodeEditor::openFile(filePath);
	
	isLoading = false;	
	return true;
}

void PolycodeUIEditor::saveFile() {
	uiDef.saveToXML(filePath);
	setHasChanges(false);
}

void PolycodeUIEditor::Resize(int x, int y) {
	// TODO: for some reason the position of the child elements of root isn't updated
	int border = 5;
	this->setWidth(x); this->setHeight(y);
	uiTree->setPosition(this->getWidth() - uiTree->getWidth() - 5, 10);
	root->Resize(this->getWidth() - 4*border - uiTree->getWidth(), this->getHeight() - 2*border);
	PolycodeEditor::Resize(x,y);
}

void PolycodeUIEditor::clearSelection() {
	selection.deleteChildren();
}

void PolycodeUIEditor::updateSelection(ScreenEntity *entity) {
	clearSelection();
	int border = 5;
	UIBox *leftBox = new UIBox("Images/boxIcon.png", 0, 0, 0, 0, border, entity->getHeight());	
	leftBox->setPosition(entity->getScreenPosition().x - selection.getScreenPosition().x, entity->getScreenPosition().y - selection.getScreenPosition().y);
	
	UIBox *rightBox = new UIBox("Images/boxIcon.png", 0, 0, 0, 0, 5, entity->getHeight());
	rightBox->setPosition(entity->getScreenPosition().x - selection.getScreenPosition().x + entity->getWidth() - border, entity->getScreenPosition().y - selection.getScreenPosition().y);
		
	UIBox *topBox = new UIBox("Images/boxIcon.png", 0, 0, 0, 0, entity->getWidth(), 5);
	topBox->setPosition(entity->getScreenPosition().x - selection.getScreenPosition().x, entity->getScreenPosition().y - selection.getScreenPosition().y );
	
	UIBox *bottomBox = new UIBox("Images/boxIcon.png", 0, 0, 0, 0, entity->getWidth(), 5);
	bottomBox->setPosition(entity->getScreenPosition().x - selection.getScreenPosition().x, entity->getScreenPosition().y - selection.getScreenPosition().y + entity->getHeight() - border);
	
	selection.addChild(leftBox);
	selection.addChild(rightBox);
	selection.addChild(topBox);
	selection.addChild(bottomBox);

}

void PolycodeUIEditor::updateSelection(Entity *entity) {
	clearSelection();
}

void PolycodeUIEditor::handleEvent(Event *event) {
	if(event->getDispatcher() == uiTree) {
		if(event->getEventCode() == InputEvent::EVENT_MOUSEUP) {	
			UITree* selected = uiTree->getRootNode()->getSelectedNode();
			if(!selected) return;
			Entity* selectedEntity = (Entity*) selected->getUserData();
			if(!selectedEntity) return;
			
			ScreenEntity* selectedScreenEntity = dynamic_cast<ScreenEntity*>(selectedEntity);
			if(selectedScreenEntity) {
				updateSelection(selectedScreenEntity);
			} else {
				updateSelection(selectedEntity);
			}
		}
	}

	PolycodeEditor::handleEvent(event);
}