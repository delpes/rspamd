/*-
 * Copyright 2017 Vsevolod Stakhov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RSPAMD_MILTER_H
#define RSPAMD_MILTER_H

#include "config.h"
#include "fstring.h"
#include "addr.h"
#include "ref.h"

enum rspamd_milter_reply {
	RSPAMD_MILTER_ADDRCPT = '+',
	RSPAMD_MILTER_DELRCPT = '-',
	RSPAMD_MILTER_ACCEPT = 'a',
	RSPAMD_MILTER_CONTINUE = 'c',
	RSPAMD_MILTER_DISCARD = 'd',
	RSPAMD_MILTER_ADDHEADER = 'h',
	RSPAMD_MILTER_CHGHEADER = 'm',
	RSPAMD_MILTER_REJECT = 'r',
	RSPAMD_MILTER_TEMPFAIL = 't',
	RSPAMD_MILTER_REPLYCODE = 'y',
	RSPAMD_MILTER_OPTNEG = 'O',
	RSPAMD_MILTER_PROGRESS = 'p'
};

struct rspamd_email_address;
struct event_base;

struct rspamd_milter_session {
	GHashTable *macros;
	rspamd_inet_addr_t *addr;
	struct rspamd_email_address *from;
	GPtrArray *rcpts;
	rspamd_fstring_t *helo;
	rspamd_fstring_t *hostname;
	rspamd_fstring_t *message;
	void *priv;
	ref_entry_t ref;
};

typedef void (*rspamd_milter_finish) (gint fd,
		struct rspamd_milter_session *session, void *ud);
typedef void (*rspamd_milter_error) (gint fd,
		struct rspamd_milter_session *session,
		void *ud, GError *err);

/**
 * Handles socket with milter protocol
 * @param fd
 * @param finish_cb
 * @param error_cb
 * @param ud
 * @return
 */
gboolean rspamd_milter_handle_socket (gint fd, const struct timeval *tv,
		struct event_base *ev_base, rspamd_milter_finish finish_cb,
		rspamd_milter_error error_cb, void *ud);

/**
 * Sets SMTP reply string
 * @param session
 * @param xcode
 * @param rcode
 * @param reply
 * @return
 */
gboolean rspamd_milter_set_reply (struct rspamd_milter_session *session,
		rspamd_fstring_t *xcode,
		rspamd_fstring_t *rcode,
		rspamd_fstring_t *reply);

/**
 * Send some action to the MTA
 * @param fd
 * @param session
 * @param act
 * @return
 */
gboolean rspamd_milter_send_action (struct rspamd_milter_session *session,
		enum rspamd_milter_reply act, ...);

/**
 * Adds some header
 * @param session
 * @param name
 * @param value
 * @return
 */
gboolean rspamd_milter_add_header (struct rspamd_milter_session *session,
		GString *name, GString *value);

/**
 * Removes some header
 * @param session
 * @param name
 * @return
 */
gboolean rspamd_milter_del_header (struct rspamd_milter_session *session,
		GString *name);

void rspamd_milter_session_unref (struct rspamd_milter_session *session);

struct rspamd_milter_session * rspamd_milter_session_ref (
		struct rspamd_milter_session *session);

#endif
