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

#ifndef NTPDATAGRAM_H_
#define NTPDATAGRAM_H_

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>

#if defined _WIN32 || defined __CYGWIN__
  #include <winsock2.h>
#else
  #include <netinet/in.h>
#endif

using namespace std;

#ifndef ALT_ASM_SRC
typedef signed char  alt_8;
typedef unsigned char  alt_u8;
typedef signed short alt_16;
typedef unsigned short alt_u16;
typedef signed long alt_32;
typedef unsigned long alt_u32;
typedef long long alt_64;
typedef unsigned long long alt_u64;
#endif

#define ALT_INLINE        __inline__
#define ALT_ALWAYS_INLINE __attribute__ ((always_inline))
#define ALT_WEAK          __attribute__((weak))

class NTPDatagram {

#define NTP_BUFFER_SIZE 48

#define NTP_DATA_GRAM_CONTROL_HEADER(base_addr) ((alt_u32*) (base_addr  + 0x00 ))
#define NTP_CONT_HEAD_PRECISION_MSK (0xFF)
#define NTP_CONT_HEAD_PRECISION_OFST (0)
#define NTP_CONT_HEAD_POLL_MSK  (0xFF)
#define NTP_CONT_HEAD_POLL_OFST (8)
#define NTP_CONT_HEAD_STRATUM_MSK (0xFF)
#define NTP_CONT_HEAD_STRATUM_OFST (16)
#define NTP_CONT_HEAD_MODE_MSK  (0x7)
#define NTP_CONT_HEAD_MODE_OFST (24)
#define NTP_CONT_HEAD_VN_MSK (0x7)
#define NTP_CONT_HEAD_VN_OFST (27)
#define NTP_CONT_HEAD_LI_MSK (0x3)
#define NTP_CONT_HEAD_LI_OFST (30)

#define NTP_DATA_GRAM_ROOT_DELAY(base_addr)       ((alt_u32*) (base_addr  + 0x04 ))
#define NTP_DATA_GRAM_ROOT_DISP(base_addr)        ((alt_u32*) (base_addr  + 0x08 ))
#define NTP_DATA_GRAM_REF_ID(base_addr)           ((alt_u32*) (base_addr  + 0x0C ))
#define NTP_DATA_GRAM_REF_TSTMP_INT(base_addr)    ((alt_u32*) (base_addr  + 0x10 ))
#define NTP_DATA_GRAM_REF_TSTMP_FRAC(base_addr)   ((alt_u32*) (base_addr  + 0x14 ))
#define NTP_DATA_GRAM_ORIG_TSTMP_INT(base_addr)   ((alt_u32*) (base_addr  + 0x18 ))
#define NTP_DATA_GRAM_ORIG_TSTMP_FRAC(base_addr)  ((alt_u32*) (base_addr  + 0x1C ))
#define NTP_DATA_GRAM_RECV_TSTMP_INT(base_addr)   ((alt_u32*) (base_addr  + 0x20 ))
#define NTP_DATA_GRAM_RECV_TSTMP_FRAC(base_addr)  ((alt_u32*) (base_addr  + 0x24 ))
#define NTP_DATA_GRAM_TRANS_TSTMP_INT(base_addr)  ((alt_u32*) (base_addr  + 0x28 ))
#define NTP_DATA_GRAM_TRANS_TSTMP_FRAC(base_addr) ((alt_u32*) (base_addr  + 0x2C ))
#define NTP_DATA_GRAM_KEY_ID(base_addr)           ((alt_u32*) (base_addr  + 0x30 ))
#define NTP_DATA_GRAM_MSG_DIG1(base_addr)         ((alt_u32*) (base_addr  + 0x34 ))
#define NTP_DATA_GRAM_MSG_DIG2(base_addr)         ((alt_u32*) (base_addr  + 0x38 ))
#define NTP_DATA_GRAM_MSG_DIG3(base_addr)         ((alt_u32*) (base_addr  + 0x3C ))
#define NTP_DATA_GRAM_MSG_DIG4(base_addr)         ((alt_u32*) (base_addr  + 0x40 ))

#define MAX_EPOCH_NR 1000
#define JAN_1970 0x83aa7e80 /* 2208988800 1970 - 1900 in seconds */

private:

protected:
	alt_u8 stream[NTP_BUFFER_SIZE];

	alt_u8  leap_indicator;
	alt_u8  version_number;
	alt_u8  mode;
	alt_u8  stratum;
	alt_8   poll_interval;
	alt_8   precision;
	alt_32  root_delay;
	alt_u32 root_dispersion;
	alt_u32 ref_id;
	alt_u32 ref_timestamp1;
	alt_u32 ref_timestamp2;
	alt_u32 orig_timestamp1;
	alt_u32 orig_timestamp2;
	alt_u32 recv_timestamp1;
	alt_u32 recv_timestamp2;
	alt_u32 trans_timestamp1;
	alt_u32 trans_timestamp2;
	alt_u32 key_id;
	alt_u32 msg_digest1;
	alt_u32 msg_digest2;
	alt_u32 msg_digest3;
	alt_u32 msg_digest4;

public:
	NTPDatagram();
	NTPDatagram(char* ntpStream);
	~NTPDatagram();

	void setLeapIndicator(alt_u8 li);
	void setVersionNumber(alt_u8 vn);
	void setMode(alt_u8 m);
	void setStratum(alt_u8 s);
	void setPoll_interval(alt_8 pi);
	void setPrecision(alt_8 p);
	void setRootDelay(alt_32 rd);
	void setRootDispersion(alt_u32 rd);
	void setRefId(alt_u32 id);
	void setRefTimestamp1(alt_u32 rt1);
	void setRefTimestamp2(alt_u32 rt2);
	void setOrigTimestamp1(alt_u32 ot1);
	void setOrigTimestamp2(alt_u32 ot2);
	void setRecvTimestamp1(alt_u32 rt1);
	void setRecvTimestamp2(alt_u32 rt2);
	void setTransTimestamp1(alt_u32 tt1);
	void setTransTimestamp2(alt_u32 tt2);
	void setKeyId(alt_u32 id);
	void setMsgDigest1(alt_u32 ms1);
	void setMsgDigest2(alt_u32 md2);
	void setMsgDigest3(alt_u32 md3);
	void setMsgDigest4(alt_u32 md4);

	alt_u8 getLeapIndicator();
	alt_u8 getVersionNumber();
	alt_u8 getMode();
	alt_u8 getStratum();
	alt_8 getPollInterval();
	alt_8 getPrecision();
	alt_32 getRootDelay();
	alt_u32 getRootDispersion();
	alt_u32 getRefId();
	alt_u32 getRefTimestamp1();
	alt_u32 getRefTimestamp2();
	alt_u32 getOrigTimestamp1();
	alt_u32 getOrigTimestamp2();
	alt_u32 getRecvTimestamp1();
	alt_u32 getRecvTimestamp2();
	alt_u32 getTransTimestamp1();
	alt_u32 getTransTimestamp2();
	alt_u32 getKeyId();
	alt_u32 getMsgDigest1();
	alt_u32 getMsgDigest2();
	alt_u32 getMsgDigest3();
	alt_u32 getMsgDigest4();

	int updateStream();
	int getStream(char** buffer);

	static int convertNTPDataToNet(alt_u8 *buffer);
	static int convertNTPDataToHost(alt_u8 *buffer);
	static struct tm * ntp2unix(u_long ntp);
};


#endif /* NTPDATAGRAM_H_ */