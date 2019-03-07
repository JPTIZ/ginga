/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef GINGA_DOCUMENT_H
#define GINGA_DOCUMENT_H

#include "Object.h"

GINGA_NAMESPACE_BEGIN

class Context;
class Media;
class Switch;

/// Run-time representation of an NCL document.
///
/// The Document maintains the NCL object tree and acts a top-level interface
/// to this tree.
///
class Document
{
public:

  /// Creates a new document.
  ///
  /// The newly created document has a root Context, called `__root__`,
  /// which contains a single MediaSettings object, called `__settings__`.
  ///
  /// @param L The Lua state to associate this document to.
  ///
  Document (lua_State *L);

  /// Destroys document and all its objects.
  virtual ~Document ();

  /// Gets the Lua state associated with document.
  lua_State *getLuaState ();

  /// Gets the set of objects in document whose type match \p mask.
  ///
  /// @param[out] objects The set of matched objects.
  /// @param mask A bitmask of or-ed Object::Type values.
  ///
  void getObjects (set<Object *> *objects,
                   unsigned int mask=(unsigned int) -1);

  /// Gets the object in document with the given id.
  ///
  /// @param id The id to match.
  /// @return The matched object or \c NULL (no such object).
  ///
  Object *getObject (const string &id);

  /// Gets the event in document with the given qualified id.
  ///
  /// @param id The qualified id to match.
  /// @return The matched event or \c NULL (no such event).
  ///
  Event *getEvent (const string &id);

  /// Gets the root Context of document.
  Context *getRoot ();

  /// Gets the MediaSettings object of document.
  MediaSettings *getSettings ();

  /// Creates a new object and adds it to document.
  ///
  /// The newly created object has a single presentation event: the lambda
  /// event, called `@lambda`.
  ///
  /// @param type The type of the new object.
  /// @param parent The parent object to add the new object to.
  /// @param id The id of the new object (must not occur in document).
  /// @return The newly created object if successful, or \c NULL otherwise.
  ///
  Object *createObject (Object::Type type, Composition *parent,
                        const string &id);

  /// Creates a new event and adds it to an object in document.
  ///
  /// @param type The type of the new event.
  /// @param objId The id of the container object (must be in document).
  /// @param evtId the id of the new event (must no occur in \p object).
  /// @return The newly created event if successful, or \c NULL otherwise.
  ///
  /// @see Object::createEvent().
  ///
  Event *createEvent (Event::Type type, const string &objId,
                      const string &evtId);

  /// Creates a new event as specified by an event qualified id.
  ///
  /// @param qualId The qualified id of the event to be created.
  /// @return The newly created event if successful, or \c NULL otherwise.
  Event *createEvent (const string &qualId);

  /// Adds object to composition.
  ///
  /// @param obj The object to add to \p comp.
  /// @param comp The composition to add \p obj to.
  /// @return \c true if successful, or \c false otherwise.
  ///
  bool addObjectToComposition (Object *obj, Composition *comp);

private:

  /// The Lua state associate with this document.
  lua_State *_L;

  // TODO ------------------------------------------------------------------

public:
  int evalAction (Event *, Event::Transition, const string &value = "");

  int evalAction (Action);

  bool evalPredicate (Predicate *);

  bool evalPropertyRef (const string &, string *);

  bool getData (const string &, void **);

  bool setData (const string &, void *, UserDataCleanFunc fn = nullptr);


private:

  /// User data attached to this document.
  UserData _udata;

  // TODO ------------------------------------------------------------------
  list<Action> evalActionInContext (Action, Context *);
};

GINGA_NAMESPACE_END

#endif // GINGA_DOCUMENT_H
