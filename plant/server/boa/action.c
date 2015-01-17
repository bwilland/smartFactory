/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <paulp@go2net.com>
 *  Some changes Copyright (C) 1996,99 Larry Doolittle <ldoolitt@boa.org>
 *  Some changes Copyright (C) 1996-2002 Jon Nelson <jnelson@boa.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* $Id: action.c,v 1.0.0.0 2014/02/28 03:05:59 Bill Exp $*/
#include "boa.h"
#include "actiondispatch.h"

int init_action(request * req) {
	char requestString[2048];
	char actionString[2048];
	char responseMsg[104800];//100K

	//fprintf(stderr,"requestString len = %d \n",strlen(req->logline));

	memcpy(requestString, req->logline, strlen(req->logline));
	char *p, *token;
	p= requestString;
	int i = 0;
	// divide the protocol head in blank
	for (i = 0; i < 3; i++) {
		token = strsep(&p, " ");
		if (i == 1) {
			strcpy(actionString, token);
			break;
		}
	}

	char clientString[32];
	sprintf(clientString,"&CLIENTIP=%s",req->remote_ip_addr);
	strcat(actionString,clientString);

	actionDispatch(actionString, responseMsg);

	req->buffer_start = 0;
	req->buffer_end = 0;
	req->filesize = strlen(responseMsg);
	send_r_request_ok(req); /* All's well */

	if (sigsetjmp(env, 1) == 0) {
		handle_sigbus = 1;
		memcpy(req->buffer + req->buffer_end, responseMsg, req->filesize);
		handle_sigbus = 0;
		//fprintf(stderr, "Buffer Wrote:\n%s\n", responseMsg);
		/* OK, SIGBUS **after** this point is very bad! */
	} else {
		/* sigbus! */
		log_error_doc(req);
		reset_output_buffer(req);
		send_r_error(req);
		fprintf(stderr, "%s Got SIGBUS in memcpy BILL!\n",
				get_commonlog_time());
		return 0;
	}
	req->buffer_end += req->filesize;

	req->status = DONE;

	/* We lose statbuf here, so make sure response has been sent */
	return 1;

}
