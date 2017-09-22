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

#ifndef EVENT_TRANSITION_MANAGER_H
#define EVENT_TRANSITION_MANAGER_H

#include "NclEvents.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

class NclEventTransitionManager
{
public:
  NclEventTransitionManager () {}
  virtual ~NclEventTransitionManager ();

  void addPresentationEvent (PresentationEvent *evt);
  void removeEventTransition (PresentationEvent *evt);

  void resetTimeIndex ();
  void prepare (bool isWholeContent, GingaTime startTime);
  void start (GingaTime offsetTime);
  void stop (GingaTime endTime);
  void abort (GingaTime endTime);

  void updateTransitionTable (GingaTime timeValue, Player *player,
                              NclEvent *mainEvt);

  EventTransition *nextTransition (NclEvent *mainEvt);

private:
  size_t _currentTransitionIdx;
  size_t _startTransitionIdx;
  vector <EventTransition *> _transTbl;

  void addTransition (EventTransition *trans);
};

GINGA_FORMATTER_END

#endif // EVENT_TRANSITION_MANAGER_H