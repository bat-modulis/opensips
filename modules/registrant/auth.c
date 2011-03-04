/*
 * $Id$
 *
 * Copyright (C) 2011 VoIP Embedded Inc.
 * Copyright (C) 2005 Voice Sistem SRL
 *
 * This file is part of opensips, a free SIP server.
 *
 * Registrant OpenSIPS-module is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Registrant OpenSIPS-module is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *
 * History:
 * ---------
 *  2005-01-31  first version (ramona)
 *  2006-03-02  UAC authentication looks first in AVPs for credential (bogdan)
 *  2011-02-20  import file from uac (Ovidiu Sas)
 */


#include <ctype.h>
#include <string.h>

#include "../../str.h"
#include "../../dprint.h"
#include "../../pvar.h"
#include "../../data_lump.h"
#include "../../mem/mem.h"
#include "../tm/tm_load.h"

#include "auth.h"
#include "auth_alg.h"
#include "auth_hdr.h"


#define  duplicate_str(_strd, _strs, _error) \
	do { \
		_strd.s = (char*)pkg_malloc(_strs.len); \
		if (_strd.s==0) \
		{ \
			LM_ERR("no more pkg memory\n");\
			goto _error; \
		} \
		memcpy( _strd.s, _strs.s, _strs.len); \
		_strd.len = _strs.len; \
	}while(0)


static str nc = {"00000001", 8};
static str cnonce = {"o", 1};

struct hdr_field *get_autenticate_hdr(struct sip_msg *rpl,
																int rpl_code)
{
	struct hdr_field *hdr;
	str hdr_name;

	/* what hdr should we look for */
	if (rpl_code==WWW_AUTH_CODE)
	{
		hdr_name.s = WWW_AUTH_HDR;
		hdr_name.len = WWW_AUTH_HDR_LEN;
	} else if (rpl_code==PROXY_AUTH_CODE) {
		hdr_name.s = PROXY_AUTH_HDR;
		hdr_name.len = PROXY_AUTH_HDR_LEN;
	} else {
		LM_ERR("reply is not an "
			"auth request\n");
		goto error;
	}

	LM_DBG("looking for header \"%.*s\"\n",
		hdr_name.len, hdr_name.s);

	/* search the auth hdr, but first parse them all */
	if (parse_headers( rpl, HDR_EOH_F, 0)<0)
	{
		LM_ERR("failed to parse reply\n");
		goto error;
	}
	hdr = get_header_by_name( rpl , hdr_name.s, hdr_name.len);
	if (hdr)
		return hdr;

	LM_ERR("reply has no "
		"auth hdr (%.*s)\n", hdr_name.len, hdr_name.s);
error:
	return 0;
}


void do_uac_auth(str *method, str *uri, struct uac_credential *crd,
		struct authenticate_body *auth, HASHHEX response)
{
	HASHHEX ha1;
	HASHHEX ha2;

	if((auth->flags&QOP_AUTH) || (auth->flags&QOP_AUTH_INT))
	{
		/* if qop generate nonce-count and cnonce */
		cnonce.s = int2str(core_hash(&auth->nonce, 0, 0),&cnonce.len);

		/* do authentication */
		uac_calc_HA1( crd, auth, &cnonce, ha1);
		uac_calc_HA2( method, uri, auth, 0/*hentity*/, ha2 );

		uac_calc_response( ha1, ha2, auth, &nc, &cnonce, response);
		auth->nc = &nc;
		auth->cnonce = &cnonce;
	} else {
		/* do authentication */
		uac_calc_HA1( crd, auth, 0/*cnonce*/, ha1);
		uac_calc_HA2( method, uri, auth, 0/*hentity*/, ha2 );

		uac_calc_response( ha1, ha2, auth, 0/*nc*/, 0/*cnonce*/, response);
	}
}


