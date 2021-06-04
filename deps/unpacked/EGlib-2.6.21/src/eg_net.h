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
#ifndef __EGNET_H__
#define __EGNET_H__
#include "eg_config.h"
#include "eg_mem.h"
#define EGNET_DBGL 0

/* ========================================================================= */
/** @defgroup EGnet EGnet
 *
 * This header contain the definitions to make basic client/server
 * communications. The protocol used to comunicate is TCP/IP. and the type of
 * communication is permanent (within each cycle connect/disconnect
 * start_read/stop_read).
 *
 * @version 0.9.1
 * @par History:
 * - 2012-12-27
 * 						- Udate error handling and system error handling.
 * - 2005-08-30
 * 						- A different aproach is to always send back and forth the
 * 						information (so that if debugging is enabled, we get confirmation
 * 						that the data was sent right), the drawback is that the
 * 						performance fall by a factor of two, but on the other hand, the
 * 						previous fix had a degradation factor of 8 (in bytes per
 * 						second)... so it seems that this approach is betteer and more
 * 						stable, still waiting for further tests.
 * - 2005-08-29
 * 						- When sending lot's of information during a single connection,
 * 						the internal send/receive data queue may overflow, to avoid this,
 * 						we add a low level syncronization step every
 * 						EG_NET_DATA_QUEUE_SIZE bits.
 * 						- Add EGnetClearSocket and EGnetInitSocket functions.
 * - 2005-06-14
 * 						- Take out the non-blocking versions (with a define) because it
 * 						won't run on some machines, but according to the ISO definitions
 * 						it should.... it's just the way it goes.
 * 						- Add flag to disable non-blocking calls.
 * - 2005-02-09
 * 						- Fix EGnetSendDouble, error comprises values < 0.5
 * - 2004-09-10
 * 						- Renan added some compatibility modifications to run this
 * 						library on windows.
 * - 2004-01-14
 * 						- first implementation/
 * */
/** @file
 * @ingroup EGnet */
/** @addtogroup EGnet */
/** @{ */
/** @example eg_net.ex.c */

/* ========================================================================= */
/** @brief If set to one, check that the information send was properly
 * received, this may slow down the code by a significant factor, and also may
 * fix synchronization issues without enabling EG_NET_SYNCHRONIZE */
#define EG_NET_CONFIRM 0
/* ========================================================================= */
/** @brief If set to one, use synchronization among sender and receiver. */
#define EG_NET_SYNCHRONIZE 0
/* ========================================================================= */
/** @brief Overhead of a MAC/TCP/IP package over the data part of the packet, 
 * note that the MAC header is about 14 bytes long, that IP has an overhead 
 * of 20 bytes plus 4 bytes for options and padding. TCP other 20 bytes plus 
 * up to 44 bytes for options and padding, thus the overhead is between 58 
 * and 102 bytes. Then to send x bytes across a TCP/IP connection,
 * we actually send x bytes + [58-102] bytes. This overhead is the constant
 * that we define bellow. For details on the structure of the MAC/IP/TCP
 * headers, see <A HREF=http://www.networksorcery.com/enp/protocol/tcp.htm>the
  TCP/IP description</A>. Note also that we shouldn't send packets of length 
 * above 572 bytes, thus the actual data length should be less than 470 bytes.
 * */
#define EG_NET_TCPIP_OVERHEAD 128
/* ========================================================================= */
/** @brief minimum length of the data queue in any socket connection. By
 * default it is set to 64Kb
 * */
#define EG_NET_DATA_QUEUE_SIZE 131072U
/* ========================================================================= */
/** @brief Minimum length of the incomming connection queue at the listening
 * socket. */
#define EG_NET_LISTEN_QUEUE_SIZE 16384U
/* ========================================================================= */
/** @brief structure for socket */
typedef struct
{
	int s_fd;						/**< store the file descriptor asociated to the socket */
	int f_fd;						/**< store the file descriptor used to read/write, note 
													 that if the socket is on the server side and is 
													 active, these two fd will be differents, but if it 
													 is on the client side both fd will be the same. */
	unsigned port;			/**< store the port of the connection. */
	unsigned int out_sz;/**< sended bytes without syncronization. */
	unsigned int in_sz;	/**< received bytes without syncronization. */
}
EGsocket_t;

/* ========================================================================= */
/** @brief Set to one to enable non-blocking calls, zero to disable them. What
 * happen in that case is that non-blocking calls behave as regular blocking
 * calls.
 * */
#define EG_NET_ALLOW_NON_BLOCKING 0

/* ========================================================================= */
/** @brief Initialize a socket structure */
#define EGnetInitSocket(skt) memset(skt,0,sizeof(EGsocket_t))

/* ========================================================================= */
/** @brief clear a socket structure and leave it ready to be freed */
#define EGnetClearSocket(__lskt) ({\
	EGsocket_t*const _EGskt = (EGsocket_t*)(__lskt);\
	if(_EGskt->s_fd>0) close(_EGskt->s_fd);\
	if(_EGskt->f_fd>0 && _EGskt->s_fd != _EGskt->f_fd) close(_EGskt->f_fd);\
	memset(_EGskt,0,sizeof(EGsocket_t));\
	0;})

/* ========================================================================= */
/** @brief set a socket to wait for connections.
 * @return zero on success, non-zero otherwise.
 * */
int EGnetListen (EGsocket_t * const skt, unsigned p);
/* ========================================================================= */
/** @brief set a socket to wait for connections.
 * @return zero on success, non-zero otherwise.
 * @note if a fatal system error ocours
 * during the call, the function will return non-zero, and syserrnop (if
 * provided) will return the system error number.
 * */
int EGnetListenE(EGsocket_t*const skt,unsigned p,int*const syserrnop);
/* ========================================================================= */
/** @brief try to establish a remote connection to host_name:port.
 * @return zero on success, non-zero otherwise.
 * @note if a fatal system error ocours
 * during the call, the function will return non-zero, and syserrnop (if
 * provided) will return the system error number.
 * */
int EGnetConnectE (EGsocket_t * const skt, const char *host_name, unsigned port,int*const syserrnop);
/* ========================================================================= */
/** @brief try to establish a remote connection to host_name:port.
 * @return zero on success, non-zero otherwise.
 * */
int EGnetConnect (EGsocket_t * const skt, const char *host_name, unsigned port);
/* ========================================================================= */
/** @brief close an established connection on the client side 
 * @return zero on success, non-zero otherwise.
 * @note if a fatal system error ocours
 * during the call, the function will return non-zero, and syserrnop (if
 * provided) will return the system error number.
 * */
int EGnetDisconnectE(EGsocket_t * const skt,int*const syserrnop);
/* ========================================================================= */
/** @brief close an established connection on the client side 
 * @return zero on success, non-zero otherwise.
 * */
int EGnetDisconnect(EGsocket_t * const skt);
/* ========================================================================= */
/** @brief accept an incomming connection from another program/host. If 
 * no connection is on the queue (of incomming connections), the program will 
 * return EAGAIN, otherwise it return zero on success and non zero (1) in 
 * error. 
 * @return zero on success, EAGAIN if no connections are awaiting in the
 * incomming queue, one otherwise.
 * @note if a fatal system error ocours
 * during the call, the function will return non-zero, and syserrnop (if
 * provided) will return the system error number.
 * */
int EGnetStartReadNBE (EGsocket_t * const skt,int*const syserrnop);
/* ========================================================================= */
/** @brief accept an incomming connection from another program/host. If 
 * no connection is on the queue (of incomming connections), the program will 
 * return EAGAIN, otherwise it return zero on success and non zero (1) in 
 * error. 
 * @return zero on success, EAGAIN if no connections are awaiting in the
 * incomming queue, one otherwise.
 * */
int EGnetStartReadNB (EGsocket_t * const skt);
/* ========================================================================= */
/** @brief accept an incomming connection from another program/host, if no 
 * connection is on the queue, the program will block until one is found. 
 * @return zero on success, non-zero otherwise.
 * @note if a fatal system error ocours
 * during the call, the function will return non-zero, and syserrnop (if
 * provided) will return the system error number.
 * */
int EGnetStartReadE (EGsocket_t * const skt,int*const syserrnop);
/* ========================================================================= */
/** @brief accept an incomming connection from another program/host, if no 
 * connection is on the queue, the program will block until one is found. 
 * @return zero on success, non-zero otherwise.
 * */
int EGnetStartRead(EGsocket_t * const skt);
/* ========================================================================= */
/** @brief close an established connection with another program/host 
 * @return zero on success, non-zero otherwise.
 * @note if a fatal system error ocours
 * during the call, the function will return non-zero, and syserrnop (if
 * provided) will return the system error number.
 * */
int EGnetStopReadE (EGsocket_t * const skt,int*const syserrnop);
/* ========================================================================= */
/** @brief close an established connection with another program/host 
 * @return zero on success, non-zero otherwise.
 * */
int EGnetStopRead (EGsocket_t * const skt);
/* ========================================================================= */
/** @brief send a char through a connected socket and keep system error
 * information */
int EGnetSendCharE (EGsocket_t * const skt, const int c,int*const syserrnop);
/* ========================================================================= */
/** @brief send a char through a connected socket */
int EGnetSendChar (EGsocket_t * const skt, const int c);
/* ========================================================================= */
/** @brief recieve a char through a connected socket  and keep system error
 * information */
int EGnetRecvCharE (EGsocket_t * const skt, char *const c,int*const syserrnop);
/* ========================================================================= */
/** @brief recieve a char through a connected socket */
int EGnetRecvChar (EGsocket_t * const skt, char *const c);
/* ========================================================================= */
/** @brief send a unsigned short through a connected socket and keep system
 * error information */
int EGnetSendUshortE (EGsocket_t * const skt, const unsigned n,int*const syserrnop);
/* ========================================================================= */
/** @brief send a unsigned short through a connected socket */
int EGnetSendUshort (EGsocket_t * const skt, const unsigned n);
/* ========================================================================= */
/** @brief recieve a unsigned short through a connected socket and keep system
 * error information */
int EGnetRecvUshortE (EGsocket_t * const skt, unsigned*const n,int*const syserrnop);
/* ========================================================================= */
/** @brief recieve a unsigned short through a connected socket */
int EGnetRecvUshort (EGsocket_t * const skt, unsigned*const n);
/* ========================================================================= */
/** @brief send a short through a connected socket */
#define EGnetSendShort(skt,n) EGnetSendUshort(skt,(unsigned)n)
#define EGnetSendShortE(skt,n,syserrnop) EGnetSendUshortE(skt,(unsigned)n,syserrnop)
/* ========================================================================= */
/** @brief recieve a short through a connected socket and keep system
 * error information */
int EGnetRecvShortE (EGsocket_t * const skt, int*const n,int*const syserrnop);
/* ========================================================================= */
/** @brief recieve a short through a connected socket */
int EGnetRecvShort (EGsocket_t * const skt, int*const n);
/* ========================================================================= */
/** @brief send an unsigned int through a connected socket and keep system
 * error information*/
int EGnetSendUintE (EGsocket_t * const skt, const unsigned int n,int*const syserrnop);
/* ========================================================================= */
/** @brief send an unsigned int through a connected socket */
int EGnetSendUint (EGsocket_t * const skt, const unsigned int n);
/* ========================================================================= */
/** @brief recieve an unsigned int through a connected socket and keep system
 * error information*/
int EGnetRecvUintE (EGsocket_t * const skt, unsigned int *const n,int*const syserrnop);
/* ========================================================================= */
/** @brief recieve an unsigned int through a connected socket */
int EGnetRecvUint (EGsocket_t * const skt, unsigned int *const n);
/* ========================================================================= */
/** @brief send an int through a connected socket */
#define EGnetSendInt(skt,n) EGnetSendUint(skt,(unsigned) n)
/* ========================================================================= */
/** @brief recieve an int through a connected socket and keep system
 * error information*/
int EGnetRecvIntE (EGsocket_t * const skt, int *const n,int*const syserrnop);
/* ========================================================================= */
/** @brief recieve an int through a connected socket */
int EGnetRecvInt (EGsocket_t * const skt, int *const n);
/* ========================================================================= */
/** @brief send a double through a connected socket. The current implementation
 * has some limits, it tries to represent the number \f$ d = 2^ex \f$ where
 * \f$0.5\leq|x|<1\f$, unfortunatelly, \f$ |e|\leq 128 \f$, and numbers that
 * can't be represented in this form won't be transmitted in a right way 
 * and keep system error information*/
int EGnetSendDoubleE (EGsocket_t * const skt, const double d,int*const syserrnop);
/* ========================================================================= */
/** @brief recieve a double through a connected socket. The current 
 * implementation
 * has some limits, it tries to represent the number \f$ d = 2^ex \f$ where
 * \f$0.5\leq|x|<1\f$, unfortunatelly, \f$ |e|\leq 128 \f$, and numbers that
 * can't be represented in this form won't be transmitted in a right way 
 * and keep system error information*/
int EGnetRecvDoubleE (EGsocket_t * const skt, double *const d,int*const syserrnop);
/* ========================================================================= */
/** @brief send a double through a connected socket. The current implementation
 * has some limits, it tries to represent the number \f$ d = 2^ex \f$ where
 * \f$0.5\leq|x|<1\f$, unfortunatelly, \f$ |e|\leq 128 \f$, and numbers that
 * can't be represented in this form won't be transmitted in a right way. */
int EGnetSendDouble (EGsocket_t * const skt, const double d);
/* ========================================================================= */
/** @brief recieve a double through a connected socket. The current 
 * implementation
 * has some limits, it tries to represent the number \f$ d = 2^ex \f$ where
 * \f$0.5\leq|x|<1\f$, unfortunatelly, \f$ |e|\leq 128 \f$, and numbers that
 * can't be represented in this form won't be transmitted in a right way. */
int EGnetRecvDouble (EGsocket_t * const skt, double *const d);
/* ========================================================================= */
/** @brief send a string through a connected socket, it will send up to the 
 * '\\0' char at the end of the string (including it) and keep system error information */
int EGnetSendStringE (EGsocket_t * const skt, const char *const str,int*const syserrnop);
/* ========================================================================= */
/** @brief recieve a string through a connected socket, the string must 
 * have allocated memory and its maximum size be max_size (including the '\\0' 
 * char)  and keep system error information */
int EGnetRecvStringE (EGsocket_t * const skt, char *const str, size_t max_size,int*const syserrnop);
/* ========================================================================= */
/** @brief send a string through a connected socket, it will send up to the 
 * '\\0' char at the end of the string (including it). */
int EGnetSendString (EGsocket_t * const skt, const char *const str);
/* ========================================================================= */
/** @brief recieve a string through a connected socket, the string must 
 * have allocated memory and its maximum size be max_size (including the '\\0' 
 * char) */
int EGnetRecvString (EGsocket_t * const skt, char *const str, size_t max_size);
/* ========================================================================= */
/** @} */
#endif
