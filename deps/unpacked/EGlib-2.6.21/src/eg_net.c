/* EGlib "Efficient General Library" provides some basic structures and
 * algorithms commons in many optimization algorithms.
 *
 * Copyright (C) 2005 Daniel Espinoza and Marcos Goycoolea.
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public 
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 
 * */
/** @file
 * @ingroup EGnet */
/** @addtogroup EGnet */
/** @{ */
/* ========================================================================= */
#ifndef __EGNET_C__
#define __EGNET_C__
#include "eg_net.h"
/* ========================================================================= */
int EGnetListenE (EGsocket_t * const skt, unsigned p,int*const syserrnop)
{
	/* local variables */
	struct sockaddr_in addr;
	int rval=0,errnum=0;
	if(syserrnop) *syserrnop=0;
	/* test if the socket exist */
	TESTG((rval=!skt),CLEANUP,"socket pointer is null");
	/* if the socket is already open, we close it and reset it */
	if (skt->s_fd) EGnetClearSocket(skt);

	/* store the port number (in machine byte order) */
	skt->port = p;

	/* open the socket, AF_INET indicate that the connection is IP, the
	 * SOCK_STREAM */
	skt->s_fd = socket (AF_INET, SOCK_STREAM, 0);
	if (skt->s_fd < 0)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"socket(AF_INET,SOCK_STREAM,0) failed, strerror %s",strerror(errnum));
	}

	/* bind it to an address, according to the ip(7) manual page, this operation
	 * links the socket to a phisical interface, the INADDR_ANY link the socket 
	 * to all phisical (and logical) interefaces. the family AF_INET is to 
	 * specify that the connection is through TCP/IP protocol. p is the port of
	 * connection, but it must be stored in network byte order (thus the call to
	 * htons). */
	memset (&addr, 0, sizeof (addr));
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons ((uint16_t)p);
	rval = bind (skt->s_fd, (struct sockaddr *) &addr, (socklen_t)(sizeof (addr)));
	if (rval)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"bind(%d,%p(%d),%zd) failed, strerror %s",skt->s_fd,&addr,p,sizeof(addr),strerror(errnum));
	}

	/* now we set the socket to be listening for incomming connections. here we
	 * set the size of the queue of incomming connections (second parameter). */
	rval = listen (skt->s_fd, EG_NET_LISTEN_QUEUE_SIZE);
	if (rval)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"listen(%d,%d) failed, strerror %s",skt->s_fd,EG_NET_LISTEN_QUEUE_SIZE,strerror(errnum));
	}
	/* ending */
	rval=0;
	CLEANUP:
	if(rval) EGnetClearSocket(skt);
	return rval;
}
/* ========================================================================= */
int EGnetListen (EGsocket_t * const skt, unsigned p)
{return EGnetListenE(skt,p,0); }
/* ========================================================================= */
int EGnetConnect (EGsocket_t * const skt, const char *host_name, unsigned port)
{return EGnetConnectE(skt,host_name,port,0);}
/* ========================================================================= */
int EGnetConnectE (EGsocket_t * const skt, const char *host_name, unsigned port, int*const syserrnop)
{
	struct hostent *host;
	struct sockaddr_in addr;
	int rval = 0,errnum;
	size_t max_buff_size = EG_NET_DATA_QUEUE_SIZE;
	if(syserrnop) *syserrnop=0;
	/* check the socket and the FD */
	TESTG((rval=!host_name),CLEANUP,"no host name given");
	TESTG((rval=!skt),CLEANUP, "socket is NULL");
	TESTG((rval= skt->s_fd),CLEANUP, "socket is open, can't reconnect");
	TESTG((rval= skt->f_fd),CLEANUP, "socket is open, can't reconnect");

	/* open the socket, AF_INET indicate that the connection is IP, the
	 * SOCK_STREAM */
	skt->s_fd = socket (AF_INET, SOCK_STREAM, 0);
	if (skt->s_fd < 0)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"socket(AF_INET,SOCK_STREAM,0) failed, strerror %s",strerror(errnum));
	}

	/* first we get the host information */
	host = gethostbyname (host_name);
	TESTG((rval=!host),CLEANUP, "Can't get host information for %s", host_name);

	/* now we set the addr structure to connect to the remote host */
	memcpy (&addr.sin_addr, host->h_addr_list[0], (size_t)(host->h_length));
	addr.sin_family = AF_INET;
	addr.sin_port = htons ((uint16_t)port);

	/* now we connect to the remote host */
	rval = connect (skt->s_fd, (struct sockaddr *) &addr, ((socklen_t)(sizeof(addr))));
	if (rval)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"connect(%d,%p(%s),%zd) failed, strerror %s",skt->s_fd,&addr,host_name,sizeof(addr),strerror(errnum));
	}

	/* set the send and receive buffer to max_buff_size */
	rval = setsockopt (skt->s_fd, SOL_SOCKET, SO_SNDBUF, &max_buff_size, ((socklen_t)(sizeof (size_t))));
	if (rval)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"setsockopt(%d,%d,%d,%zd) failed, strerror %s",skt->s_fd,SOL_SOCKET,SO_SNDBUF,max_buff_size,strerror(errnum));
	}
	rval = setsockopt (skt->s_fd, SOL_SOCKET, SO_RCVBUF, &max_buff_size, ((socklen_t)(sizeof (size_t))));
	if (rval)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"setsockopt(%d,%d,%d,%zd) failed, strerror %s",skt->s_fd,SOL_SOCKET,SO_RCVBUF,max_buff_size,strerror(errnum));
	}

	/* now the F_FD is set to S_FD */
	skt->f_fd = skt->s_fd;
	/* ending */
	rval=0;
	CLEANUP:
	if(rval) EGnetClearSocket(skt);
	return rval;
}
/* ========================================================================= */
int EGnetDisconnect(EGsocket_t*const skt){return EGnetDisconnectE(skt,0);}
/* ========================================================================= */
int EGnetDisconnectE (EGsocket_t * const skt,int*const syserrnop)
{
	int rval=0,errnum;
	if(syserrnop)*syserrnop=0;
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval=(skt->s_fd<=0)),CLEANUP, "socket not ready");
	TESTG((rval=(skt->f_fd!=skt->s_fd)),CLEANUP, "socket not connected");
	if((rval=close(skt->s_fd)))
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"close(%d) failed, strerror %s",skt->s_fd,strerror(errnum));
	}
	rval=0;
	CLEANUP:
	memset(skt,0,sizeof(EGsocket_t));
	return rval;
}
/* ========================================================================= */
static int EGnetStartReadEE(EGsocket_t*const skt,const int nb,int*const syserrnop)
{
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof (addr);
	size_t max_buff_size = EG_NET_DATA_QUEUE_SIZE;
	#if EG_NET_ALLOW_NON_BLOCKING
	long fd_mode;
	#endif
	int rval = 0,errnum,fd;
	if(syserrnop)*syserrnop=0;
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval= skt->f_fd),CLEANUP, "socket already ready to read/write");
	TESTG((rval= !skt->s_fd),CLEANUP, "socket not initialized by listen");
	/* set the state of the listening socket to blocking mode, i.e. it block the
	 * execution of the program until we get an incomming connection */
	#if EG_NET_ALLOW_NON_BLOCKING
	if(nb)
	{
		fcntl (skt->s_fd, F_GETFL, &fd_mode);
		fd_mode = fd_mode | O_NONBLOCK;
		fcntl (skt->s_fd, F_SETFL, &fd_mode);
	}
	else
	{
		fcntl (skt->s_fd, F_GETFL, &fd_mode);
		fd_mode = fd_mode & (~O_NONBLOCK);
		fcntl (skt->s_fd, F_SETFL, &fd_mode);
	}
	#endif
	/* init the addr structure, AF_INET indicate that isa IP connection,
	 * INADDR_ANY indicate that all interfaces are good (or allowed to recive 
	 * connections) */
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;
	fd = skt->f_fd = accept (skt->s_fd, ((ACCEPT_TYPE_ARG2) (&addr)), &addr_size);
	/* if accept return with EAGAIN, it means that the incomming connections 
	 * queue was empty, and thus there is no message to read. In this case we 
	 * return EAGAIN to the calling function and let the socket in listening 
	 * state */
	if (fd == EAGAIN && nb)
	{
		skt->f_fd = 0,skt->out_sz=skt->in_sz=0;
		TESTG((rval=EAGAIN),CLEANUP,"no incomming connection to accept");
	}
	else if (fd < 0)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		skt->f_fd = 0,skt->out_sz=skt->in_sz=0;
		TESTG((rval= 1),CLEANUP, "can't stablish connection with incomming request");
	}
	/* set the send and receive buffer to max_buff_size */
	rval = setsockopt (skt->f_fd, SOL_SOCKET, SO_SNDBUF, &max_buff_size, ((socklen_t)(sizeof (size_t))));
	if (rval)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		close(skt->f_fd);
		skt->f_fd=0,skt->out_sz=skt->in_sz=0;
		TESTG((rval=1),CLEANUP, "setsockopt(%d,%d,%d,%zd) failed, strerror %s",fd,SOL_SOCKET,SO_SNDBUF,max_buff_size,strerror(errnum));
	}
	rval = setsockopt (skt->f_fd, SOL_SOCKET, SO_RCVBUF, &max_buff_size, ((socklen_t)(sizeof (size_t))));
	if (rval)
	{
		errnum=errno;
		if(syserrnop) *syserrnop=errnum;
		close(skt->f_fd);
		skt->f_fd=0,skt->out_sz=skt->in_sz=0;
		TESTG((rval=1),CLEANUP, "setsockopt(%d,%d,%d,%zd) failed, strerror %s",fd,SOL_SOCKET,SO_RCVBUF,max_buff_size,strerror(errnum));
	}
	IFMESSAGE (EGNET_DBGL, "connected (%d)", addr_size);
	/* ending */
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetStartReadNB(EGsocket_t*const skt){return EGnetStartReadEE(skt,1,0);}
/* ========================================================================= */
int EGnetStartReadNBE(EGsocket_t * const skt,int*const syserrnop){return EGnetStartReadEE(skt,1,syserrnop);}
/* ========================================================================= */
int EGnetStartRead(EGsocket_t * const skt){return EGnetStartReadEE(skt,0,0);}
/* ========================================================================= */
int EGnetStartReadE(EGsocket_t * const skt,int*const syserrnop){return EGnetStartReadEE(skt,0,syserrnop);}
/* ========================================================================= */
int EGnetStopRead(EGsocket_t * const skt){return EGnetStopReadE(skt,0);}
/* ========================================================================= */
int EGnetStopReadE(EGsocket_t * const skt,int*const syserrnop)
{
	int rval=0,errnum,fd;
	if(syserrnop) *syserrnop=0;
	TESTG((rval=!skt),CLEANUP, "Null socket");
	TESTG((rval=(skt->s_fd<=0)),CLEANUP, "socket is not open");
	TESTG((rval=(skt->f_fd<=0)),CLEANUP, "socket have not been put to read before");
	TESTG((rval=(skt->s_fd == skt->f_fd)),CLEANUP, "socket is not a lisening socket");
	rval=close(skt->f_fd),fd=skt->f_fd;
	skt->f_fd=0,skt->in_sz=skt->out_sz=0;
	if(rval)
	{
		errnum=errno,fd=skt->f_fd;
		if(syserrnop) *syserrnop=errnum;
		TESTG((rval=1),CLEANUP,"close(%d) failed, strerror %s",fd,strerror(errnum));
	}
	/* ending */
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
#define EG_NET_SEND 0
#define EG_NET_RECEIVE 1
#define EG_NET_MARKER '5'
/* ========================================================================= */
/** @brief error control for receive function.
 * This function assumes that the caller has defined:
 * 		- syserrnop, as a pointer to integer; if not null, it will store the 
 * 			last system error number.
 * 		- rval return value of actual recv call
 * 		- errnum local integer where to store the local error number
 * 		- CLEANUP where to jump in case of error, with non-zero rval
 * */
#define __EGnetRecv(__fd,__dt,__sz,__fl) do{\
	const int __Nsz=(__sz);\
	rval=recv((__fd),(__dt),((size_t)(__Nsz)),__fl);\
	if(rval==0){\
		TESTG((rval=1),CLEANUP,"Peer closed connection");\
	}\
	else if(rval!=(__Nsz)){\
		errnum=errno;\
		if(syserrnop)*syserrnop=errnum;\
		TESTG((rval=1),CLEANUP,"recv(%d,%p,%d,%d) failed, strerror %s ",(__fd),(__dt),__Nsz,(__fl),strerror(errnum));}}while(0)
/* ========================================================================= */
/** @brief error control for send function.
 * This function assumes that the caller has defined:
 * 		- syserrnop, as a pointer to integer; if not null, it will store the 
 * 			last system error number.
 * 		- rval return value of actual send call
 * 		- errnum local integer where to store the local error number
 * 		- CLEANUP where to jump in case of error, with non-zero rval
 * */
#define __EGnetSend(__fd,__dt,__sz,__fl) do{\
	const int __Nsz=(__sz);\
	rval=send((__fd),(__dt),((size_t)(__Nsz)),__fl);\
	if(rval!=(__Nsz)){\
		errnum=errno;\
		if(syserrnop)*syserrnop=errnum;\
		TESTG((rval=1),CLEANUP,"send(%d,%p,%d,%d) failed, strerror %s ",(__fd),(__dt),__Nsz,(__fl),strerror(errnum));}}while(0)
/* ========================================================================= */
/** @brief Assure that the data queue don't overflow by using syncronizations
 * calls between both ends of the connections.
 * @param skt socket where we are working.
 * @param type either EG_NET_SEND or EG_NET RECEIVE.
 * @param length bytes being transfered.
 * @aram syserrnop pointer where to return system-error codes if any
 * */
#if EG_NET_SYNCHRONIZE
static int EGnetSynchronizeE (EGsocket_t * const skt,
																		 int const type,
																		 const size_t length,
																		 int*const syserrnop)
{
	int rval=0,errnum=0;
	unsigned char marker = EG_NET_MARKER;
	if(syserrnop)*syserrnop=0;
	switch (type)
	{
		case EG_NET_SEND:
			skt->in_sz = EG_NET_TCPIP_OVERHEAD;
			if (skt->out_sz + EG_NET_TCPIP_OVERHEAD + length >= EG_NET_DATA_QUEUE_SIZE)
			{
				IFMESSAGE(EGNET_DBGL-10,"Syncronizing %u %u", skt->in_sz, skt->out_sz);
				__EGnetRecv(skt->f_fd,&marker,1,0);
				TESTG((rval=(marker!=EG_NET_MARKER)),CLEANUP, "Error while synchronizing"
						", expecting to receive %c(%d) and received %c(%d)", EG_NET_MARKER,
						(int) EG_NET_MARKER, marker, (int) marker);
				skt->out_sz = EG_NET_TCPIP_OVERHEAD;
			}
			else
				skt->out_sz += EG_NET_TCPIP_OVERHEAD + length;
			break;
		case EG_NET_RECEIVE:
			skt->out_sz = EG_NET_TCPIP_OVERHEAD;
			if (skt->in_sz + EG_NET_TCPIP_OVERHEAD + length >= EG_NET_DATA_QUEUE_SIZE)
			{
				IFMESSAGE(EGNET_DBGL-10, "Syncronizing %u %u", skt->in_sz, skt->out_sz);
				__EGnetSend(skt->f_fd,&marker,1,0);
				skt->in_sz = EG_NET_TCPIP_OVERHEAD;
			}
			else
				skt->in_sz += EG_NET_TCPIP_OVERHEAD + length;
			break;
		default:
			EXIT (1, "Unknown communication type %d", type);
			break;
	}
	rval=0;
	CLEANUP:
	return rval;
}
#else
#define EGnetSynchronizeE(skt,type,length,syserrp) 0
#endif
/* ========================================================================= */
/** @brief define confirmation protocols. They assume that send functions have
 * defined a buffcp variable where to receive answer and check answer, they
 * also assume that buff is an array of char containing the desaired info to
 * transmit. moreover, it assumes rval syserrnop and errnum are defined */
#if EG_NET_CONFIRM
#define EGnetRConf(__Cfd,__Csz,__Cfl) __EGnetSend((__Cfd),buff,(__Csz),(__Cfl))
#define EGnetSConf(__Cfd,__Csz,__Cfl) do{__EGnetRecv((__Cfd),buffcp,(__Csz),(__Cfl));TESTG((rval=(memcmp(buff,buffcp,(size_t)(__Csz)))),CLEANUP,"wrong comunication");}while(0)
#else
#define EGnetRConf(__Cfd,__Csz,__Cfl) 
#define EGnetSConf(__Cfd,__Csz,__Cfl) 
#endif
/* ========================================================================= */
/** @brief define high level send/receive functions. It assumes that data is
 * stored in buff, and that for checking purposes it has buffcp defined and
 * sck */
#define EGnetSend(__Hfd,__Hsz,__Hfl) do{\
	EGcallD(EGnetSynchronizeE(skt,EG_NET_SEND,(__Hsz),syserrnop));\
	__EGnetSend((__Hfd),buff,(__Hsz),(__Hfl));\
	EGnetSConf((__Hfd),(__Hsz),(__Hfl));}while(0)

#define EGnetRecv(__Hfd,__Hsz,__Hfl) do{\
	EGcallD(EGnetSynchronizeE(skt,EG_NET_RECEIVE,(__Hsz),syserrnop));\
	__EGnetRecv((__Hfd),buff,(__Hsz),(__Hfl));\
	EGnetRConf((__Hfd),(__Hsz),(__Hfl));}while(0)
/* ========================================================================= */
int EGnetSendCharE(EGsocket_t * const skt, const int c,int*const syserrnop)
{
	int rval=0,errnum=0;
	unsigned char buff[1]={((unsigned char)c)&0xff};
	#if EG_NET_CONFIRM
	unsigned char buffcp[1];
	#endif
	if(syserrnop)*syserrnop=0;
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval= !skt->f_fd),CLEANUP, "socket not ready to I/O operations");
	EGnetSend(skt->f_fd,1,0);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetSendChar(EGsocket_t * const skt, const int c){return EGnetSendCharE(skt,c,0);}
/* ========================================================================= */
int EGnetRecvCharE (EGsocket_t * const skt, char *const buff,int*const syserrnop)
{
	int rval=0,errnum=0;
	if(syserrnop)*syserrnop=0;
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval= !skt->f_fd),CLEANUP, "socket not ready to I/O operations");
	EGnetRecv(skt->f_fd,1,0);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetRecvChar(EGsocket_t * const skt, char *const c){return EGnetRecvCharE(skt,c,0);}
/* ========================================================================= */
int EGnetSendUshortE (EGsocket_t * const skt, const unsigned n,int*const syserrnop)
{
	int rval=0,errnum=0;
	unsigned char buff[2];
	#if EG_NET_CONFIRM
	unsigned char buffcp[2];
	#endif
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval= !skt->f_fd),CLEANUP, "socket not ready to I/O operations");
	buff[0] = (unsigned char)((n >> 8) & 0xff);
	buff[1] = (unsigned char)((n) & 0xff);
	EGnetSend(skt->f_fd,2,0);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetSendUshort (EGsocket_t * const skt,const unsigned n){return EGnetSendUshortE(skt,n,0);}
/* ========================================================================= */
int EGnetRecvUshortE (EGsocket_t * const skt, unsigned *const n,int*const syserrnop)
{
	unsigned char buff[2];
	int rval=0,errnum=0;
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval= !skt->f_fd),CLEANUP, "socket not ready to I/O operations");
	EGnetRecv(skt->f_fd,2,0);
	*n = ((unsigned) buff[0]) << 8;
	*n |= ((unsigned) buff[1]);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetRecvUshort(EGsocket_t * const skt, unsigned*const n){return EGnetRecvUshortE(skt,n,0);}
/* ========================================================================= */
int EGnetRecvShortE (EGsocket_t * const skt, int*const n,int*const syserrnop)
{
	unsigned un=0;
	int rval = EGnetRecvUshortE (skt, &un,syserrnop);
	*n = ((short)un);
	return rval;
}
/* ========================================================================= */
int EGnetRecvShort (EGsocket_t * const skt, int*const n) {return EGnetRecvShortE(skt,n,0);}
/* ========================================================================= */
int EGnetSendUintE(
		EGsocket_t*const skt,
		const unsigned int n,
		int*const syserrnop)
{
	int rval=0,errnum=0;
	unsigned char buff[4];
#if EG_NET_CONFIRM
	unsigned char buffcp[4];
#endif
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval= !skt->f_fd),CLEANUP, "socket not ready to I/O operations");
	buff[0] = (unsigned char)((n >> 24) & 0xff);
	buff[1] = (unsigned char)((n >> 16) & 0xff);
	buff[2] = (unsigned char)((n >> 8) & 0xff);
	buff[3] = (unsigned char)((n) & 0xff);
	EGnetSend(skt->f_fd,4,0);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetSendUint (EGsocket_t * const skt, unsigned int n){return EGnetSendUintE(skt,n,0);}
/* ========================================================================= */
int EGnetRecvUintE (EGsocket_t * const skt, unsigned int *const n,int*const syserrnop)
{
	unsigned char buff[4];
	int rval=0,errnum=0;
	TESTG((rval= !skt),CLEANUP, "Null socket");
	TESTG((rval= !skt->f_fd),CLEANUP, "socket not ready to I/O operations");
	EGnetRecv(skt->f_fd,4,0);
	*n = ((unsigned) buff[0]) << 24;
	*n |= ((unsigned) buff[1]) << 16;
	*n |= ((unsigned) buff[2]) << 8;
	*n |= ((unsigned) buff[3]);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetRecvUint(EGsocket_t * const skt, unsigned int *const n){return EGnetRecvUintE(skt,n,0);}
/* ========================================================================= */
int EGnetRecvIntE (EGsocket_t * const skt, int *const n,int*const syserrnop)
{
	unsigned un=0;
	int rval=EGnetRecvUintE (skt, &un,syserrnop);
	*n=(int)un;
	return rval;
}
/* ========================================================================= */
int EGnetRecvInt(EGsocket_t * const skt, int *const n){return EGnetRecvIntE(skt,n,0);}
/* ========================================================================= */
int EGnetSendDoubleE(EGsocket_t * const skt, const double exd,int*const syserrnop)
{
	/* the idea is that we store in buff[9] the sign, in buff[8] we store the
	 * exponent, and in the first 8 bytes we store the value. the only drawback 
	 * is that this only allow us to send/recieve values whose exponents are 
	 * at most 128 and at least -128 */
	double d = exd;
	int rval=0,errnum=0;
	unsigned char buff[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 128, 0 };
#if EG_NET_CONFIRM
	unsigned char buffcp[10];
#endif
	if (d < 0)
	{
		buff[9] = 1;
		d = -d;
	}
	if (d >= 1)
	{
#define __HI_EXP(x,e,v,lv) {if( x >=v ){ e = (unsigned char)(e + (unsigned char)(lv)); x /= v;}}
		__HI_EXP (d, buff[8], 18446744073709551616.0, 64);
		__HI_EXP (d, buff[8], 4294967296.0, 32);
		__HI_EXP (d, buff[8], 65536.0, 16);
		__HI_EXP (d, buff[8], 256.0, 8);
		__HI_EXP (d, buff[8], 16.0, 4);
		__HI_EXP (d, buff[8], 4.0, 2);
		__HI_EXP (d, buff[8], 2.0, 1);
#undef __HI_EXP
		d /= 2;
		buff[8]++;/* += ((unsigned char)(1));*/
	}
	else if (d < 0.5)
	{
#define __LO_EXP(x,e,v,lv) {if( x < 1/v ) { e = (unsigned char)(e - ((unsigned char)(lv))); x *= v;}}
		__LO_EXP (d, buff[8], 18446744073709551616.0, 64);
		__LO_EXP (d, buff[8], 4294967296.0, 32);
		__LO_EXP (d, buff[8], 65536.0, 16);
		__LO_EXP (d, buff[8], 256.0, 8);
		__LO_EXP (d, buff[8], 16.0, 4);
		__LO_EXP (d, buff[8], 4.0, 2);
		__LO_EXP (d, buff[8], 2.0, 1);
#undef __LO_EXP
	}
	d *= 256;
	buff[0] = (unsigned char) d;
	d -= buff[0];
	d *= 256;
	buff[1] = (unsigned char) d;
	d -= buff[1];
	d *= 256;
	buff[2] = (unsigned char) d;
	d -= buff[2];
	d *= 256;
	buff[3] = (unsigned char) d;
	d -= buff[3];
	d *= 256;
	buff[4] = (unsigned char) d;
	d -= buff[4];
	d *= 256;
	buff[5] = (unsigned char) d;
	d -= buff[5];
	d *= 256;
	buff[6] = (unsigned char) d;
	d -= buff[6];
	d *= 256;
	buff[7] = (unsigned char) d;
	EGnetSend(skt->f_fd,10,0);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetSendDouble(EGsocket_t * const skt, const double exd){return EGnetSendDoubleE(skt,exd,0);}
/* ========================================================================= */
int EGnetRecvDoubleE (EGsocket_t * const skt, double *const d,int*const syserrnop)
{
	unsigned char buff[10];
	int rval=0,errnum=0;
	unsigned int ui1, ui2;
	TESTG((rval= !skt->f_fd),CLEANUP, "Uninitialized socket");
	EGnetRecv(skt->f_fd,10,0);
	ui1 = buff[0];
	ui1 = ui1 << 8;
	ui1 |= buff[1];
	ui1 = ui1 << 8;
	ui1 |= buff[2];
	ui1 = ui1 << 8;
	ui1 |= buff[3];
	ui2 = buff[4];
	ui2 = ui2 << 8;
	ui2 |= buff[5];
	ui2 = ui2 << 8;
	ui2 |= buff[6];
	ui2 = ui2 << 8;
	ui2 |= buff[7];
	//ui2 = (buff[4] << 24) | (buff[5] << 16) | (buff[6] << 8) | buff[7];
	*d = ((ui2 / 4294967296.0) + ui1) / 4294967296.0;
	if (buff[8] > 128)
	{
#define __HI_EXP(x,e,v,lv) {if( e >= lv + 128){ e = (unsigned char)(e - ((unsigned char)lv)); x *= v;}}
		__HI_EXP (*d, buff[8], 18446744073709551616.0, 64);
		__HI_EXP (*d, buff[8], 4294967296.0, 32);
		__HI_EXP (*d, buff[8], 65536.0, 16);
		__HI_EXP (*d, buff[8], 256.0, 8);
		__HI_EXP (*d, buff[8], 16.0, 4);
		__HI_EXP (*d, buff[8], 4.0, 2);
		__HI_EXP (*d, buff[8], 2.0, 1);
#undef __HI_EXP
	}
	else if (buff[8] < 128)
	{
#define __LO_EXP(x,e,v,lv) {if( e <= 128 - lv){ e = (unsigned char)(e + ((unsigned char)lv)); x /= v;}}
		__LO_EXP (*d, buff[8], 18446744073709551616.0, 64);
		__LO_EXP (*d, buff[8], 4294967296.0, 32);
		__LO_EXP (*d, buff[8], 65536.0, 16);
		__LO_EXP (*d, buff[8], 256.0, 8);
		__LO_EXP (*d, buff[8], 16.0, 4);
		__LO_EXP (*d, buff[8], 4.0, 2);
		__LO_EXP (*d, buff[8], 2.0, 1);
#undef __LO_EXP
	}
	if (buff[9])
		*d = -(*d);
	rval=0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetRecvDouble(EGsocket_t * const skt, double *const d){return EGnetRecvDoubleE(skt,d,0);}
/* ========================================================================= */
int EGnetSendStringE (EGsocket_t * const skt, const char *const const exstr,int*const syserrnop)
{
	int rval=0;
	const char *str = exstr;
	while (*str)
	{
		EGcallD(EGnetSendCharE(skt,*str,syserrnop));
		str++;
	}
	EGcallD(EGnetSendCharE(skt,*str,syserrnop));
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetSendString (EGsocket_t * const skt, const char *const const exstr){return EGnetSendStringE(skt,exstr,0);}
/* ========================================================================= */
int EGnetRecvStringE (EGsocket_t * const skt, char *const const exstr, size_t max_size,int*const syserrnop)
{
	int rval;
	char *str = exstr;
	size_t count = max_size - 1;
	do
	{
		EGcallD(EGnetRecvCharE(skt,str,syserrnop));
	} while ((*str) && (str++, --count));
	*str = 0;
	CLEANUP:
	return rval;
}
/* ========================================================================= */
int EGnetRecvString(EGsocket_t * const skt, char *const const exstr, size_t max_size){return EGnetRecvStringE(skt,exstr,max_size,0);}
/* ========================================================================= */
/** @} */
#endif
