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

#ifndef AwesomiumHandler_h_
#define AwesomiumHandler_h_

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "mb/ILocalScreenManager.h"
#include "mb/IInputManager.h"
#include "mb/interface/IWindow.h"
#include "mb/interface/IInputEventListener.h"
#include "mb/interface/IMotionEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include <stdio.h>
#include <string>
#include <sstream>
#include <map>
#include <iostream>
using namespace std;

extern "C" {
#include <Awesomium/awesomium_capi.h>
}

#include <Awesomium/KeyboardCodes.h>
using namespace Awesomium;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	typedef int AwesomiumHDR;

	class AwesomiumInfo :
			public IInputEventListener,
			public IMotionEventListener {

		public:
			static const short ET_NONE   = 0;
			static const short ET_KEY    = 1;
			static const short ET_BUTTON = 2;
			static const short ET_MOTION = 3;

			GingaScreenID myScreen;
			AwesomiumHDR id;
			string mURL;
			ISurface* surface;
			int mouseX, mouseY;
			int x, y;
			int w, h;
			bool hasFocus;
			bool setFocus;
			string rFile;
			bool update;
			int eventCode;
			short eventType;

			bool _eMVarW;
			pthread_cond_t _eMVar;
			pthread_mutex_t _eM;

			AwesomiumInfo(GingaScreenID screenId, AwesomiumHDR id);
			virtual ~AwesomiumInfo();

			void waitEvent();
			bool eventArrived();

			bool userEventReceived(IInputEvent* ev);
			bool motionEventReceived(int x, int y, int z);
	};

	class AwesomiumHandler {
		private:
			static map<int, int> fromGingaToAwesomium;
			static pthread_mutex_t s_lMutex;
			static map<AwesomiumHDR, AwesomiumInfo*> s_infos;
			static AwesomiumHDR s_id;
			static map<AwesomiumHDR, IInputManager*> s_ims;

			/* static since we have to respect browser isolation */
			static bool initialized;
			static awe_webview* webView;

			static ILocalScreenManager* dm;

			static void initCodeMap();

			static bool getAwesomeInfo(
					AwesomiumHDR id,
					AwesomiumInfo** aInfo,
					bool removeInfo=false);

			static bool getAwesomeIM(
					AwesomiumHDR id,
					IInputManager** im,
					bool removeInfo=false);

		public:
			static AwesomiumHDR createAwesomium(GingaScreenID screenId);
			static void destroyAwesomium(AwesomiumHDR id);
			static void getSize(AwesomiumHDR id, int* w, int* h);

			static void setAwesomiumBounds(
					AwesomiumHDR id, int x, int y, int w, int h);

			static void loadUrl(AwesomiumHDR id, string url);
			static string getUrl(AwesomiumHDR id);
			static ISurface* getSurface(AwesomiumHDR id);
			static void setFocus(AwesomiumHDR id, bool focus);

		private:
			static void setFocus(AwesomiumInfo* aInfo);
			static void focus(AwesomiumInfo* aInfo);
			static void unfocus(AwesomiumInfo* aInfo);

		public:
			static void eventHandler(AwesomiumInfo* aInfo);

		private:
			static void injectKey(AwesomiumInfo* aInfo, int keyCode);
			static void refresh(AwesomiumHDR id);
			static void update(AwesomiumInfo* aInfo, double value);

		public:
			static void stopUpdate(AwesomiumHDR id);
	};
}
}
}
}
}
}

#endif /*AwesomiumHandler_h_*/
