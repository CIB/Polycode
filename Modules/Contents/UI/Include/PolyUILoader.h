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
#include "PolyGlobals.h"
#include "PolyScreenEntity.h"
#include "tinyxml.h"
#include "PolycodeUI.h"
#include <stdexcept>
#include <map>

namespace Polycode {

    /**
     * Thrown when anything goes wrong with loading an UI definition.
     */
    class UILoaderError : public std::runtime_error {
        // TODO: Find out whether tinyxml supports error line numbers
        //       and attach those to the error.
	public:
        UILoaderError() : runtime_error("Error while loading UI Definition.") {};
        UILoaderError(String message) : runtime_error(message.c_str()) {};
    };

    class _PolyExport UILoader {
	public:
		UILoader();

		UIElement* loadObject(Object *doc);

		/**
		 * Get the Entity that was loaded from the entry. NULL if none.
		 *
		 * @param entry The entry that the returned Entity was loaded from.
		 */
		Entity* getLoadedEntity(ObjectEntry *entry);

		/**
		 * Get the Object that the entity was loaded from, or NULL if none.
		 *
		 * @param entity The entity that we want to know the entry of.
		 */
		ObjectEntry *getLoadedFrom(Entity *entity);

		/**
		 * Get a vector containing all the entities loaded by this loader.
		 */
		std::vector<Entity*> *getAllLoadedEntities();

		/**
		 * Destroy the screen entity currently associated with
		 * the given object entry, and reload that entry.
		 */
		ScreenEntity* reloadObjectEntry(ObjectEntry* node);

		/**
		 * Delete all child entities of an entity that were loaded
		 * using this loader.
		 */
		void deleteAllLoadedChildren(Entity* entity);

		/**
		 * Get a list of valid attributes for a specific Element type.
		 *
		 * Returned ObjectEntry's will represent the name of the attribute,
		 * the valid type, and the default value.
		 */
		vector<ObjectEntry> getTypeAttributes(String type);

		Object *rootDocument;
	protected:
		Number readNumberNonNegativeOrError(ObjectEntry *from, String key, String errorWhere);
		Number readNumberOrError(ObjectEntry *from, String key, String errorWhere);
		String readStringOrError(ObjectEntry *from, String key, String errorWhere);

        UIButton* buildButton(ObjectEntry *data);
        UIBox* buildBox(ObjectEntry *data);
		void loadObjectEntries(ObjectEntry *parentNode, Entity* parent);
		ScreenEntity* loadObjectEntry(ObjectEntry *node, Entity* parent);

		std::map<Entity*, ObjectEntry*> entriesByEntities;
		std::map<ObjectEntry*, Entity*> entitiesByEntries;
    };
}
