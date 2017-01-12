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

#ifndef APPLICATIONNAMEDESCRIPTOR_H_
#define APPLICATIONNAMEDESCRIPTOR_H_

#include <string.h>

#include <vector>
using namespace std;

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {

	struct AppName {
		char languageCode[3];
		unsigned char applicationNameLength;
		char* applicationNameChar;
	};

	class ApplicationNameDescriptor : public IMpegDescriptor {
		private:
			vector<struct AppName*> appNames;

		public:
			ApplicationNameDescriptor();
			virtual ~ApplicationNameDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			size_t process(char* data, size_t pos);
			void print();
		};
}
}
}
}
}
}
}

#endif /* APPLICATIONNAMEDESCRIPTOR_H_ */