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
#include "PolyUITextInput.h"
#include "PolyObject.h"
#include "PolyConfig.h"

PolycodeUIEditor::PolycodeUIEditor() : PolycodeEditor(true) {
	editorType = "PolycodeUIEditor";
	selectedEntity = NULL;
	
	// Property inspector
	Config *conf = CoreServices::getInstance()->getConfig();
	int fontSize = conf->getNumericValue("Polycode", "uiDefaultFontSize");	
	String fontName = conf->getStringValue("Polycode", "uiDefaultFontName");

	Number st = conf->getNumericValue("Polycode", "uiTreeContainerSkinT");
	Number sr = conf->getNumericValue("Polycode", "uiTreeContainerSkinR");
	Number sb = conf->getNumericValue("Polycode", "uiTreeContainerSkinB");
	Number sl = conf->getNumericValue("Polycode", "uiTreeContainerSkinL");	
	Number padding = conf->getNumericValue("Polycode", "uiTreeContainerSkinPadding");	

	propertyBgBox = new UIBox(conf->getStringValue("Polycode", "uiTreeContainerSkin"),
						  st,sr,sb,sl, 300, 200);
	propertyBgBox->processInputEvents = true;

	addChild(propertyBgBox);

	UIButton *button = new UIButton("Apply", 50);
	button->setPosition(propertyBgBox->getWidth() - button->getWidth() - 5, propertyBgBox->getHeight() - button->getHeight() - 5);
	button->addEventListener(this, UIEvent::CLICK_EVENT);
	propertyBgBox->addChild(button);

	// UI Structure Tree
	uiTree = new UITreeContainer("boxIcon.png", "Structure", 300, 190);
	uiTree->setPosition(this->getWidth() - uiTree->getWidth() - 5, 10);
	addChild(uiTree);

	// Selection indicator
	selection.setPosition(0, 0);
	addChild(&selection);

	uiTree->addEventListener(this, InputEvent::EVENT_MOUSEUP);
	CoreServices::getInstance()->getCore()->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
}

PolycodeUIEditor::~PolycodeUIEditor() {
	delete uiTree;
}

void PolycodeUIEditor::updateTree(UITree *treeNode, Entity *entity) {
	String label = entity->id;
	if(label == "") {
		label = "Unknown";
	}
	treeNode->setLabelText(entity->id);
	treeNode->setUserData(entity);
	treeNode->clearTree();
	
	for(int i=0; entity->getChildAtIndex(i); i++) {
		Entity *child = entity->getChildAtIndex(i);
		if(loader.getLoadedFrom(child)) {
			// Only display nodes that were loaded from an UI definition.
			updateTree(treeNode->addTreeChild("box_icon.png", "", NULL), child);
		}
	}
}

bool PolycodeUIEditor::openFile(OSFileEntry filePath) {
	isLoading = true;
	
	if(!uiDef.loadFromXML(filePath.fullPath)) {
		return false;
	}
	root = loader.loadObject(&uiDef);
	if(root->id == "") {
		root->id = "Root";
	}

	UITree *rootNode = uiTree->getRootNode();
	updateTree(rootNode, root);

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
	propertyBgBox->setPosition(this->getWidth() - propertyBgBox->getWidth() - 5, 210);
	root->Resize(this->getWidth() - 4*border - uiTree->getWidth(), this->getHeight() - 2*border);
	PolycodeEditor::Resize(x,y);
}

void PolycodeUIEditor::clearSelection() {
	selection.deleteChildren();
}

void PolycodeUIEditor::updateSelection(ScreenEntity *entity) {
	clearSelection();
	selectedEntity = entity;
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

	// Update the property box
	Config *conf = CoreServices::getInstance()->getConfig();
	int fontSize = conf->getNumericValue("Polycode", "uiDefaultFontSize");	
	String fontName = conf->getStringValue("Polycode", "uiDefaultFontName");

	ObjectEntry *entry = loader.getLoadedFrom(entity);
	for(int i=0; i<propertyBgBox->getNumChildren(); i++) {
		Entity* child = propertyBgBox->getChildAtIndex(i);
		if(child->id == "property_input") {
			propertyBgBox->removeChild(child);
			i--; // Hack to avoid breaking the iterator
		}
	}
	int row = 0;
	int column = 0;
	for(int i = 0; i < entry->children.size(); i++) {
		ObjectEntry *childEntry = entry->children[i];

		if(childEntry->type == ObjectEntry::INT_ENTRY) {
			ScreenLabel *label = new ScreenLabel(childEntry->name, fontSize, fontName, Label::ANTIALIAS_FULL);
			label->setColor(1.0, 1.0, 1.0, 0.5);
			label->setPosition(5 + 80 * column, 4 + 40*row);
			label->id = "property_input";
			propertyBgBox->addChild(label);

			UITextInput* input = new UITextInput(false, 60, 12);
			input->setNumberOnly(true);
			input->setPosition(5 + 80 * column, 20+40*row);
			input->setText(String::IntToString(childEntry->intVal));
			input->setUserData(childEntry);
			input->id = "property_input";
			propertyBgBox->addChild(input);

			row++;
			if(row * 40 + 20 > propertyBgBox->getHeight()) {
				row = 0;
				column++;
			}
		}
	}
}

void PolycodeUIEditor::updateSelection(Entity *entity) {
	clearSelection();
}

ScreenEntity* PolycodeUIEditor::selectAt(ScreenEntity* parent, Vector2 mousePosition) {
	ScreenEntity* rval = NULL;
	Vector2 pos = parent->getScreenPosition();
	// Only allow objects that were loaded from XML as result.
	if(loader.getLoadedFrom(parent)) {
		Number width = parent->getWidth();
		Number height = parent->getHeight();
		if(pos.x <= mousePosition.x && pos.y <= mousePosition.y && (pos.x + width) >= mousePosition.x && (pos.y + height) >= mousePosition.y) {
			rval = parent;
		}
	}
	for(int i=parent->getNumChildren()-1; i >= 0; i--) {
		ScreenEntity *next = dynamic_cast<ScreenEntity*>(parent->getChildAtIndex(i));
		if(next) {
			ScreenEntity *child = this->selectAt(next, mousePosition);
			if(child) rval = child;
		}
	}

	return rval;
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
	// TODO: check if dispatcher is core input handler
	} else if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
		InputEvent *inputEvent = (InputEvent*) event;
		ScreenEntity* hit = selectAt(root, inputEvent->mousePosition);
		if(hit) {
			this->updateSelection(hit);
		}
	} else if(event->getEventCode() == UIEvent::CLICK_EVENT) {
		bool somethingChanged = false;
		for(int i = 0; i < propertyBgBox->getNumChildren(); i++) {
			UITextInput *input = dynamic_cast<UITextInput*>(propertyBgBox->getChildAtIndex(i));
			if(!input) continue;

			ObjectEntry *attribute = (ObjectEntry*) input->getUserData();

			if(attribute->type == ObjectEntry::INT_ENTRY) {
				int newValue = atoi(input->getText().c_str());
				if(attribute->intVal != newValue) {
					somethingChanged = true;
					attribute->intVal = newValue;
				}
			}
		}

		if(somethingChanged) {
			ObjectEntry *parent = loader.getLoadedFrom(selectedEntity);
			selectedEntity = loader.reloadObjectEntry(parent);
			// TODO: only update the part that actually changed
			updateTree(uiTree->getRootNode(), root);

			ScreenEntity* screenEntity = dynamic_cast<ScreenEntity*>(selectedEntity);
			if(screenEntity) {
				updateSelection(screenEntity);
			}
		}
	}

	PolycodeEditor::handleEvent(event);
}