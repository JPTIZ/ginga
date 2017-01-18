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

#ifndef _LINKSIMPLEACTION_H_
#define _LINKSIMPLEACTION_H_

#include "ncl/SimpleAction.h"
using namespace ::ginga::ncl;

#include "NclFormatterEvent.h"
using namespace ::ginga::formatter;

#include "INclLinkActionListener.h"
#include "NclLinkAction.h"

GINGA_FORMATTER_BEGIN

class NclLinkSimpleAction : public NclLinkAction
{
protected:
  NclFormatterEvent *event;
  short actionType;

private:
  INclLinkActionListener *listener;

protected:
  virtual void run ();

public:
  NclLinkSimpleAction (NclFormatterEvent *event, short type);
  virtual ~NclLinkSimpleAction ();
  NclFormatterEvent *getEvent ();
  short getType ();
  void setSimpleActionListener (INclLinkActionListener *listener);
  virtual vector<NclFormatterEvent *> *getEvents ();
  virtual vector<NclLinkAction *> *getImplicitRefRoleActions ();
};

GINGA_FORMATTER_END
#endif //_LINKSIMPLEACTION_H_
