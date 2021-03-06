/*
 * $Id$
 *
 * Copyright (C) 2007 Voice System SRL
 *
 * This file is part of opensips, a free SIP server.
 *
 * opensips is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * opensips is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * History:
 * --------
 * 2007-07-10  initial version (ancuta)
*/




#ifndef DLG_REQUEST_WITHIN_H
#define DLG_REQUEST_WITHIN_H

#include "../../statistics.h"
#include "../../str.h"
#include "../../mi/mi.h"
#include "../tm/tm_load.h"
#include "dlg_hash.h"

#define MAX_FWD			"70"
#define MAX_SIZE		256
#define RCV_BYE_REPLY	1

#define MI_DIALOG_NOT_FOUND 		"Requested Dialog not found"
#define MI_DIALOG_NOT_FOUND_LEN 	(sizeof(MI_DIALOG_NOT_FOUND)-1)
#define MI_DLG_OPERATION_ERR		"Operation failed"
#define MI_DLG_OPERATION_ERR_LEN	(sizeof(MI_DLG_OPERATION_ERR)-1)

extern struct tm_binds d_tmb;
extern int dlg_enable_stats;
extern stat_var * active_dlgs;

typedef void (dlg_request_callback)(struct cell *t,int type,
					struct tmcb_params* ps);
typedef void (dlg_release_func)(void *param);

int dlg_end_dlg(struct dlg_cell *dlg, str *extra_hdrs);

struct mi_root * mi_terminate_dlg(struct mi_root *cmd_tree, void *param );

int send_leg_msg(struct dlg_cell *dlg,str *method,int src_leg,int dst_leg,
		str *hdrs,str *body,dlg_request_callback func,void *param,dlg_release_func release);
#endif
