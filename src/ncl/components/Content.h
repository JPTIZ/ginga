/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef _CONTENT_H_
#define _CONTENT_H_

#include <string>
#include <iostream>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	class Content {
		protected:
			string type; // tipo do conteudo (recomendavel uso do MIME)
			long size;   // tamanho do conteudo (recomendavel em octetos)
			set<string> typeSet;

		public:
			Content(string someType, long someSize);
			Content();
			virtual ~Content();
			bool instanceOf(string s);
			virtual long getSize(void);
			virtual string getType(void);
			virtual void setSize(long someSize);
			virtual void setType(string someType);
	};
}
}
}
}
}

#endif //_CONTENT_H_