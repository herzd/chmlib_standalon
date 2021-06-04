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
#include "EGlib.h"

static void net_usage (char *program)
{
	fprintf (stdout, "Usage: %s [options]\n", program);
	fprintf (stdout, "Options:\n");
	fprintf (stdout, "    -b     boss mode\n");
	fprintf (stdout, "    -c     client mode\n");
	fprintf (stdout, "    -h c   host name of boss\n");
	fprintf (stdout, "    -k     send kill signal to boss\n");
	fprintf (stdout, "    -m n   send 'n' chars to boss and exit\n");
	fprintf (stdout, "    -p n   port number\n");
}

#define BOSS 1
#define CLIENT 2
#define KILL 3
static unsigned int nchars = 0;
static unsigned port = 0;
static char *hname = 0;
static int mode = 0;
static int parseargs (int argc,
											char **argv)
{
	int c;
	while ((c = getopt (argc, argv, "bch:km:p:")) != EOF)
	{
		switch (c)
		{
		case 'b':
			if (mode)
				goto FAIL;
			mode = BOSS;
			break;
		case 'c':
			if (mode)
				goto FAIL;
			mode = CLIENT;
			break;
		case 'k':
			if (mode)
				goto FAIL;
			mode = KILL;
			break;
		case 'h':
			hname = strdup (optarg);
			break;
		case 'm':
			nchars = (unsigned) atoi (optarg);
			break;
		case 'p':
			port = (unsigned) atoi (optarg);
			break;
		default:
			goto FAIL;
		}														/* end switch */
	}															/* end while */

	/* check that the input is correct */
	if (!mode)
	{
		MESSAGE (0, "you must only one of the c b k mode");
		goto FAIL;
	}
	if ((mode == CLIENT) && (!nchars))
	{
		MESSAGE (0,
						 "you must choose how many bytes to send/recieve in client mode");
		goto FAIL;
	}
	if (!port)
	{
		MESSAGE (0, "you must choose a connection port");
		goto FAIL;
	}

	/* ending correctly */
	return 0;

	/* failure status */
FAIL:
	net_usage (argv[0]);
	exit (1);
}

int main (int argc,
					char **argv)
{
	/* local variables */
	EGsocket_t lskt;
	EGsocket_t *skt = &lskt;
	EGrandState_t s;
	char *str = 0,
	  type,
	  ctmp,
	  ctmp2;
	int rval,
	  itmp,
	  itmp2;
	unsigned int utmp,
	  utmp2;
	int stmp,
	  stmp2;
	unsigned ustmp,
	  ustmp2;
	double dtmp,
	  dtmp2;
	register unsigned i;
	EGlib_info();
	EGlib_version();
	EGrandInit(&s);
	parseargs (argc, argv);
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	srand ((unsigned int) (time (0)));
	EGnetInitSocket(skt);
	switch (mode)
	{
	case BOSS:
		rval = EGnetListen (skt, port);
		CHECKRVAL (rval);
		while (1)
		{
			rval = EGnetStartRead (skt);
			CHECKRVAL (rval);
			rval = EGnetRecvChar (skt, &type);
			CHECKRVAL (rval);
			if (type == KILL)
			{
				MESSAGE (0, "the boss is being killed");
				rval = EGnetStopRead (skt);
				CHECKRVAL (rval);
				break;
			}
			rval = EGnetRecvUint (skt, &nchars);
			CHECKRVAL (rval);
			MESSAGE (0, "we are going to recieve/send %u chars", nchars);
			for (i = nchars; i--;)
			{
				rval = EGnetRecvChar (skt, &ctmp);
				CHECKRVAL (rval);
				rval = EGnetSendChar (skt, ctmp);
				CHECKRVAL (rval);
			}
			MESSAGE (0, "we are going to recieve/send %u uint", nchars);
			for (i = nchars; i--;)
			{
				rval = EGnetRecvUint (skt, &utmp);
				CHECKRVAL (rval);
				rval = EGnetSendUint (skt, utmp);
				CHECKRVAL (rval);
			}
			MESSAGE (0, "we are going to recieve/send %u int", nchars);
			for (i = nchars; i--;)
			{
				rval = EGnetRecvInt (skt, &itmp);
				CHECKRVAL (rval);
				rval = EGnetSendInt (skt, itmp);
				CHECKRVAL (rval);
			}
			MESSAGE (0, "we are going to recieve/send %u ushort", nchars);
			for (i = nchars; i--;)
			{
				rval = EGnetRecvUshort (skt, &ustmp);
				CHECKRVAL (rval);
				rval = EGnetSendUshort (skt, ustmp);
				CHECKRVAL (rval);
			}
			MESSAGE (0, "we are going to recieve/send %u short", nchars);
			for (i = nchars; i--;)
			{
				rval = EGnetRecvShort (skt, &stmp);
				CHECKRVAL (rval);
				rval = EGnetSendShort (skt, stmp);
				CHECKRVAL (rval);
			}
			MESSAGE (0, "we are going to recieve/send %u double", nchars);
			for (i = nchars; i--;)
			{
				rval = EGnetRecvDouble (skt, &dtmp);
				CHECKRVAL (rval);
			}
			for (i = nchars; i--;)
			{
				rval = EGnetSendDouble (skt, dtmp);
				CHECKRVAL (rval);
			}

			rval = EGnetStopRead (skt);
			CHECKRVAL (rval);
		}
		/* re-set socket; this also un-bind the listening socket */
		EGnetClearSocket(skt);
		break;
	case CLIENT:
		rval = EGnetConnect (skt, hname, port);
		CHECKRVAL (rval);
		rval = EGnetSendChar (skt, mode);
		CHECKRVAL (rval);
		rval = EGnetSendUint (skt, nchars);
		CHECKRVAL (rval);
		MESSAGE (0, "we are going to send/recieve %u chars", nchars);
		for (i = nchars; i--;)
		{
			ctmp = (char) EGrand(&s);
			rval = EGnetSendChar (skt, ctmp);
			CHECKRVAL (rval);
			rval = EGnetRecvChar (skt, &ctmp2);
			CHECKRVAL (rval);
			TEST (ctmp != ctmp2, "wrong communication");
		}
		MESSAGE (0, "we are going to send/recieve %u uint", nchars);
		for (i = nchars; i--;)
		{
			utmp = (unsigned) EGrand (&s);
			rval = EGnetSendUint (skt, utmp);
			CHECKRVAL (rval);
			rval = EGnetRecvUint (skt, &utmp2);
			CHECKRVAL (rval);
			TEST (utmp != utmp2, "wrong communication %u != %u", utmp, utmp2);
		}
		MESSAGE (0, "we are going to send/recieve %u int", nchars);
		for (i = nchars; i--;)
		{
			itmp = (int)EGrand (&s);
			if (random () | 1)
				itmp = -itmp;
			rval = EGnetSendInt (skt, itmp);
			CHECKRVAL (rval);
			rval = EGnetRecvInt (skt, &itmp2);
			CHECKRVAL (rval);
			TEST (itmp != itmp2, "wrong communication %d != %d", itmp, itmp2);
		}
		MESSAGE (0, "we are going to send/recieve %u ushort", nchars);
		for (i = nchars; i--;)
		{
			ustmp = (unsigned short)EGrand(&s);
			rval = EGnetSendUshort (skt, ustmp);
			CHECKRVAL (rval);
			rval = EGnetRecvUshort (skt, &ustmp2);
			CHECKRVAL (rval);
			TEST (ustmp != ustmp2, "wrong communication %u != %u, packet %d", ustmp, ustmp2,nchars - i);
		}
		MESSAGE (0, "we are going to send/recieve %u short", nchars);
		for (i = nchars; i--;)
		{
			stmp = random ();
			if (random () | 1)
				stmp = -stmp;
			stmp = (short)stmp;
			rval = EGnetSendShort (skt, stmp);
			CHECKRVAL (rval);
			rval = EGnetRecvShort (skt, &stmp2);
			CHECKRVAL (rval);
			TEST (stmp != stmp2, "wrong communication %d != %d", stmp, stmp2);
		}
		MESSAGE (0, "we are going to send/recieve %u double", nchars);
		for (i = nchars; i--;)
		{
			dtmp = random ();
			dtmp /= random () + 1;
			if (random () | 1)
				dtmp = -dtmp;
			rval = EGnetSendDouble (skt, dtmp);
			CHECKRVAL (rval);
		}
		for (i = nchars; i--;)
		{
			rval = EGnetRecvDouble (skt, &dtmp2);
			CHECKRVAL (rval);
		}

		rval = EGnetDisconnect (skt);
		CHECKRVAL (rval);
		break;
	case KILL:
		rval = EGnetConnect (skt, hname, port);
		CHECKRVAL (rval);
		MESSAGE (0, "Killing boss");
		rval = EGnetSendChar (skt, mode);
		CHECKRVAL (rval);
		rval = EGnetDisconnect (skt);
		CHECKRVAL (rval);
		break;
	}
	CLEANUP:
	/* ending */
	free (str);
	return 0;
}

/* ========================================================================= */
/** @} */
