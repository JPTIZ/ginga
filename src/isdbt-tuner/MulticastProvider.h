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

#ifndef MULTICASTPROVIDER_H_
#define MULTICASTPROVIDER_H_

#include "system/SystemCompat.h"
#include "system/PracticalSocket.h"
using namespace ::ginga::system;

#include "IDataProvider.h"

GINGA_TUNER_BEGIN

class MulticastProvider : public IDataProvider
{
protected:
  string addr;
  int portNumber;
  short capabilities;
  UDPSocket *udpSocket;

public:
  MulticastProvider (string groupAddr, int port);
  ~MulticastProvider ();
  virtual void setListener (arg_unused (ITProviderListener *listener)){};
  virtual void attachFilter (arg_unused (IFrontendFilter *filter)){};
  virtual void removeFilter (arg_unused (IFrontendFilter *filter)){};
  virtual short
  getCaps ()
  {
    return capabilities;
  };

  virtual bool
  tune ()
  {
    if (callServer () > 0)
      {
        return true;
      }

    return false;
  };
  virtual Channel *
  getCurrentChannel ()
  {
    return NULL;
  }
  virtual bool
  getSTCValue (arg_unused (uint64_t *stc), arg_unused (int *valueType))
  {
    return false;
  }
  virtual bool
  changeChannel (arg_unused (int factor))
  {
    return false;
  }
  bool
  setChannel (arg_unused (string channelValue))
  {
    return false;
  }

  virtual int
  createPesFilter (arg_unused (int pid), arg_unused (int pesType),
                   arg_unused (bool compositeFiler))
  {
    return -1;
  }

  virtual string
  getPesFilterOutput ()
  {
    return "";
  }

  virtual void close (){};

  virtual int callServer ();
  virtual char *receiveData (int *len);
};

GINGA_TUNER_END

#endif /*MULTICASTPROVIDER_H_*/
