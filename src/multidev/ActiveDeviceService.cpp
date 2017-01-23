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
#include "ActiveDeviceService.h"
#include "DeviceDomain.h"

GINGA_MULTIDEV_BEGIN

ActiveDeviceService::ActiveDeviceService () : BaseDeviceService ()
{
  serviceClass = DeviceDomain::CT_ACTIVE;
}

ActiveDeviceService::~ActiveDeviceService () {}

void
ActiveDeviceService::connectedToBaseDevice (unsigned int domainAddr)
{
  set<IRemoteDeviceListener *>::iterator i;

  addDevice (domainAddr, DeviceDomain::CT_BASE, 0, 0);

  Thread::mutexLock (&lMutex);
  i = listeners->begin ();
  while (i != listeners->end ())
    {
      (*i)->connectedToBaseDevice (domainAddr);
      ++i;
    }
  Thread::mutexUnlock (&lMutex);
}

bool
ActiveDeviceService::receiveMediaContent (arg_unused (unsigned int devAddr),
                                          arg_unused (char *stream), arg_unused (int streamSize))
{
  return false;
}

GINGA_MULTIDEV_END
