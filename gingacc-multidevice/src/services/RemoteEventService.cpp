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

#include "config.h"

#include "multidevice/services/RemoteEventService.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "contextmanager/ContextManager.h"
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {

#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif
	IContextManager* RemoteEventService::contextManager = NULL;
	RemoteEventService* RemoteEventService::_instance = NULL;

	RemoteEventService::RemoteEventService() {
#if HAVE_COMPSUPPORT
		RemoteEventService::contextManager = ((ContextManagerCreator*)(cm->getObject(
						"ContextManager")))();
#else
		RemoteEventService::contextManager = ContextManager::getInstance();
#endif
		clog << "RemoteEventService::new RemoteEventService()" << endl;

		groups.clear();
		Thread::mutexInit(&groupsMutex, false);
	}
	
	void RemoteEventService::setBaseDeviceNCLPath(string base_device_ncl_path){
		this->base_device_ncl_path = base_device_ncl_path;
	}

	RemoteEventService::~RemoteEventService() {
		map<int, TcpSocketService*>::iterator i;

		Thread::mutexLock(&groupsMutex);
		i = groups.begin();
		while (i != groups.end()) {
			delete i->second;
			++i;
		}
		groups.clear();

		Thread::mutexUnlock(&groupsMutex);
		Thread::mutexDestroy(&groupsMutex);
	}

	RemoteEventService* RemoteEventService::getInstance() {
		if (_instance == NULL) {
			_instance = new RemoteEventService();
		}
		return _instance;
	}

	void RemoteEventService::addDeviceClass(unsigned int id) {
		Thread::mutexLock(&groupsMutex);
		if (groups.count(id) == 0) {
			groups[id] = new TcpSocketService(
							RemoteEventService::DEFAULT_PORT,
							this);
		} else {
			clog << "RemoteEventService::addDeviceClass Warning! Trying to ";
			clog << "add the same device class '" << id << "' twice!" << endl;
		}
		Thread::mutexUnlock(&groupsMutex);
	}

	void RemoteEventService::addDevice(
			unsigned int device_class, unsigned int device_id,
			char* addr, int srvPort, bool isLocalConnection) {

		map<int, TcpSocketService*>::iterator i;
		TcpSocketService* tss;

		Thread::mutexLock(&groupsMutex);
		i = groups.find(device_class);
		if (i == groups.end()) {
			Thread::mutexUnlock(&groupsMutex);
			return;
		}

		tss = i->second;
		tss->addConnection(device_id, addr, srvPort, isLocalConnection);
		clog << "RemoteEventService :: TcpSocketService->addConnection";
		clog << "device_id=" << device_id << endl;

		Thread::mutexUnlock(&groupsMutex);
	}

	void RemoteEventService::addDocument(
			unsigned int device_class, char* name, char* body) {

		TcpSocketService* tss;

		Thread::mutexLock(&groupsMutex);
		if (groups.count(device_class) == 0) {
			Thread::mutexUnlock(&groupsMutex);
			return;
		}

		tss = groups[device_class];
		tss->postTcpCommand((char*)"ADD", 0, name, body);
		Thread::mutexUnlock(&groupsMutex);
		//ADD without start will be used by prefetch mechanisms
	}

	void RemoteEventService::startDocument(
			unsigned int device_class, char* name) {

		TcpSocketService* tss;


        string str_name = string(name);

		Thread::mutexLock(&groupsMutex);
		if (groups.count(device_class) == 0) {
			Thread::mutexUnlock(&groupsMutex);
			return;
		}

		tss = groups[device_class];

		clog << "RemoteEventService::startDocument " << name << endl; 
        
        size_t pos_fname = str_name.find_last_of("/\\");
		string doc_name;
		
		if (pos_fname != std::string::npos) {
			doc_name = string(str_name.substr(pos_fname + 1));
		} else {
			doc_name = str_name;
		}
        
        string str_path = string(str_name.substr(0,pos_fname));

        int pos_fdir = str_path.find_last_of("/\\");

        string last_dir_name = string(str_path.substr(pos_fdir+1));

        string doc_rel_path = doc_name;

        //clog << "RES::start " << doc_rel_path << endl;
        
		//TODO: start individual media objects
		//TODO: MANIFEST

		string zipDumpStr = SystemCompat::getTemporaryDir() + "basetmp.zip";
		//char *zip_dump = (char*)"/tmp/basetmp.zip";

		/* string dir_app = SystemCompat::getUserCurrentPath() +
				SystemCompat::getPath(string(name));
*/
		string dir_app = SystemCompat::getPath(base_device_ncl_path) + SystemCompat::getIUriD() +
				SystemCompat::getPath(SystemCompat::updatePath(string(name)));


		clog << "RemoteEventService::startDocument dir app="<<dir_app<<endl;
		//clog << "RemoteEventService::tmp dir="<<string(zip_dump)<<endl;

		zip_directory(zipDumpStr, dir_app, SystemCompat::getIUriD());
		clog << "RemoteEventService::startDocument zip_directory all done!" << endl;

		string zip_base64 = getBase64FromFile(zipDumpStr);
		clog << "RemoteEventService::startDocument getBase64 all done!" << endl;
		remove(zipDumpStr.c_str());
		//TODO: prefetch. add w/o start

		if (zip_base64 != "") {
			clog << "RemoteEventService::zipb64.len = ";
			clog << zip_base64.length() << endl;

	//		tss->postTcpCommand((char*)"ADD", 0, name, (char*)zip_base64.c_str());
	//		clog << "RemoteEventService:: ADD name="<<name<<endl;

	//		tss->postTcpCommand((char*)"START", 0, name, (char*)"");
	//
			tss->postTcpCommand((char*)"START", 0,(char*) doc_rel_path.c_str(), (char*)zip_base64.c_str());
		
			clog << "RemoteEventService:: START name=" << doc_rel_path << endl;
		}

		Thread::mutexUnlock(&groupsMutex);
	}

	void RemoteEventService::stopDocument(
			unsigned int device_class, char* name) {

		TcpSocketService* tss;

		Thread::mutexLock(&groupsMutex);
		if (groups.count(device_class) == 0) {
			Thread::mutexUnlock(&groupsMutex);
			return;
		}

		tss = groups[device_class];
		clog << "RemoteEventService::stopDocument "<< name << endl;
		tss->postTcpCommand((char*)"STOP", 0, name, (char*)"");
		Thread::mutexUnlock(&groupsMutex);
	}

	/***/

	bool RemoteEventService::newDeviceConnected(int newDevClass, int w, int h) {
		return false;
	}

	void RemoteEventService::connectedToBaseDevice(unsigned int domainAddr) {

	}

	bool RemoteEventService::receiveRemoteContent(
			int remoteDevClass,
			string contentUri) {
		return false;
	}

	bool RemoteEventService::receiveRemoteContent(
			int remoteDevClass,
			char *stream, int streamSize) {
		return false;
	}

	bool RemoteEventService::receiveRemoteContentInfo(
			string contentId, string contentUri) {
		return false;
	}


	bool RemoteEventService::receiveRemoteEvent(
						int remoteDevClass,
						int eventType,
						string eventContent) {

		if (eventType == IDeviceDomain::FT_ATTRIBUTIONEVENT) {
			//clog << "RemoteEventService::receiveRemoteEvent ATTR";
			//clog << eventContent << endl;

			string name, value;
			size_t pos;
			pos = eventContent.find("=");
			name = eventContent.substr(0,pos-1);
			value = eventContent.substr(pos+2);
			RemoteEventService::contextManager->setGlobalVar(name,value);

		}
		return true;
	}


}
}
}
}
}
}
