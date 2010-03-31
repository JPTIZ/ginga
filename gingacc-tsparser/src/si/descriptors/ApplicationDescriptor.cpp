/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "../../../include/ApplicationDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	ApplicationDescriptor::ApplicationDescriptor() {
		descriptorTag                 = 0x00;
		descriptorLength              = 0;
		applicationPriority           = 0;
		applicationProfilesLength     = 0;
		profiles                      = new vector<struct Profile*>;
		serviceBoundFlag              = false;
		transportProtocolLabelsLength = 0;
		transportProtocolLabels       = NULL;
	}

	ApplicationDescriptor::~ApplicationDescriptor() {
		vector<struct Profile*>::iterator i;
		if (profiles != NULL) {
			i = profiles->begin();
			while (i != profiles->end()) {
				delete (*i);
				++i;
			}
			delete profiles;
			profiles = NULL;
		}

		if (transportProtocolLabels != NULL) {
			delete transportProtocolLabels;
			transportProtocolLabels = NULL;
		}
	}

	unsigned char ApplicationDescriptor::getApplicationPriority() {
		return applicationPriority;
	}

	vector<struct Profile*>* ApplicationDescriptor::getProfiles() {
		return profiles;
	}

	unsigned char ApplicationDescriptor::getVisibility() {
		return visibility;
	}

	unsigned char ApplicationDescriptor::getTransportProtocolLabelsLength() {
		return transportProtocolLabelsLength;
	}

	char* ApplicationDescriptor::getTransportProtocolLabels() {
		return transportProtocolLabels;
	}
	unsigned int ApplicationDescriptor::getDescriptorLength() {
		return descriptorLength;
	}

	unsigned char ApplicationDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	void ApplicationDescriptor::print(){
		vector<struct Profile*>::iterator i;
		for (i = profiles->begin(); i != profiles->end(); ++i) {

		}
	}

	size_t ApplicationDescriptor::process(char* data, size_t pos) {
		struct Profile* profile;
		size_t profpos;

		cout << "ApplicationDescriptor::process with pos = '" << pos;

		descriptorLength = data[pos+1];
		cout << "' and length = '" << (unsigned int)descriptorLength;
		cout << endl;

		pos += 2;
		applicationProfilesLength = data[pos];

		cout << "ApplicationDescriptor::process appProfilesLength = '";
		cout << (unsigned int)applicationProfilesLength << "'" << endl;

		profpos = pos;
		cout << "ApplicationDescriptor::process profiles pos = " << pos << endl;
		for (int i = 0; i < applicationProfilesLength; ++i) {
			pos++;
			profile = new struct Profile;
			profile->applicationProfile = ((((data[pos] & 0xFF) << 8) & 0xFF00)|
					(data[pos+1] & 0xFF));

			pos += 2;
			profile->versionMajor = data[pos];

			pos++;
			profile->versionMinor = data[pos];

			pos++;
			profile->versionMicro = data[pos];
			profiles->push_back(profile);
			cout << "ApplicationDescriptor::process with prefetch ";
			cout << "applicationProfile = '" << profile->applicationProfile;
			cout << "' versionMajor = '" << (profile->versionMajor & 0xFF);
			cout << "' versionMinor = '" << (profile->versionMinor & 0xFF);
			cout << "' versionMicro = '" << (profile->versionMicro & 0xFF);
			cout << "'" << endl;
		}

		pos = profpos + applicationProfilesLength;

		cout << "ApplicationDescriptor::process pos after length = ";
		cout << pos << endl;

		serviceBoundFlag = ((data[pos] & 0x80) << 7);
		visibility       = ((data[pos] & 0x60) << 6);
		pos++;

		applicationPriority = data[pos];
		transportProtocolLabelsLength =
				descriptorLength - applicationProfilesLength - 2;

		cout << "ApplicationDescriptor::process priority = ";
		cout << (unsigned int) applicationPriority;
		cout << " and transpPLabelsLenght = " ;
		cout << (unsigned int) transportProtocolLabelsLength << endl;

		transportProtocolLabels = new char[transportProtocolLabelsLength];
		for (int i = 0; i < transportProtocolLabelsLength; ++i) {
			pos++;
			transportProtocolLabels[i] = data[pos];
		}
		cout << "ApplicationDescriptor::process finish with pos = " << pos << endl;
		return pos;
	}
}
}
}
}
}
}
}
