/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "ginga.h"
#include "NodeEntity.h"
#include "CompositeNode.h"

GINGA_NCL_BEGIN

NodeEntity::NodeEntity (const string &uid, Content *someContent) : Node (uid)
{
  _descriptor = NULL;
  _anchorList.push_back (new LambdaAnchor (uid));
  this->_content = someContent;
}

NodeEntity::~NodeEntity ()
{
  vector<Anchor *>::iterator i;
  set<ReferNode *>::iterator j;
  Anchor *anchor;

  if (_descriptor != NULL)
    {
      // descriptor is deleted in descriptor base
      _descriptor = NULL;
    }

  if (_content != NULL)
    {
      delete _content;
      _content = NULL;
    }

  for (j = _instSameInstances.begin (); j != _instSameInstances.end (); ++j)
    {
      if ((Node *)(*j) != (Node *)this && Entity::hasInstance ((*j), true))
        {
          delete (*j);
        }
    }
  _instSameInstances.clear ();

  for (j = _gradSameInstances.begin (); j != _gradSameInstances.end (); ++j)
    {
      delete (*j);
    }
  _gradSameInstances.clear ();

  i = _anchorList.begin ();
  while (i != _anchorList.end ())
    {
      anchor = (*i);
      if (Entity::hasInstance (anchor, true))
        {
          delete (*i);
        }
      ++i;
    }
  _anchorList.clear ();
}

bool
NodeEntity::addAnchor (int index, Anchor *anchor)
{
  if (index == 0)
    {
      return false;
    }
  return Node::addAnchor (index, anchor);
}

LambdaAnchor *
NodeEntity::getLambdaAnchor ()
{
  LambdaAnchor *lambda;
  lambda = static_cast<LambdaAnchor *> (*(_anchorList.begin ()));
  return lambda;
}

void
NodeEntity::setId (const string &id)
{
  LambdaAnchor *anchor;

  Entity::setId (id);
  anchor = getLambdaAnchor ();
  anchor->setId (id);
}

bool
NodeEntity::removeAnchor (int index)
{
  if (index == 0)
    {
      return false;
    }
  return Node::removeAnchor (index);
}

GenericDescriptor *
NodeEntity::getDescriptor ()
{
  return _descriptor;
}

void
NodeEntity::setDescriptor (GenericDescriptor *someDescriptor)
{
  _descriptor = someDescriptor;
}

Content *
NodeEntity::getContent ()
{
  return _content;
}

void
NodeEntity::setContent (Content *someContent)
{
  _content = someContent;
}

bool
NodeEntity::addAnchor (Anchor *anchor)
{
  return Node::addAnchor (anchor);
}

bool
NodeEntity::removeAnchor (Anchor *anchor)
{
  return Node::removeAnchor (anchor);
}

set<ReferNode *> *
NodeEntity::getInstSameInstances ()
{
  return &_instSameInstances;
}

set<ReferNode *> *
NodeEntity::getGradSameInstances ()
{
  if (_gradSameInstances.empty ())
    {
      return NULL;
    }

  return &_gradSameInstances;
}

bool
NodeEntity::addSameInstance (ReferNode *node)
{
  if (node->getInstanceType () == "instSame")
    {
      if (_instSameInstances.count (node) != 0)
        {
          return false;
        }

      _instSameInstances.insert (node);
    }
  else if (node->getInstanceType () == "gradSame")
    {
      if (_gradSameInstances.count (node) != 0)
        {
          return false;
        }

      _gradSameInstances.insert (node);
    }
  else
    {
      return false;
    }

  return true;
}

void
NodeEntity::removeSameInstance (ReferNode *node)
{
  set<ReferNode *>::iterator i;

  i = _gradSameInstances.find (node);
  if (i != _gradSameInstances.end ())
    {
      _gradSameInstances.erase (i);
    }

  i = _instSameInstances.find (node);
  if (i != _instSameInstances.end ())
    {
      _instSameInstances.erase (i);
    }
}

GINGA_NCL_END
