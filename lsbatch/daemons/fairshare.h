/*
 * Copyright (C) 2014 David Bigagli
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA
 *
 */

#if !defined(_FAIRSHARE_HEADER_)
#define _FAIRSHARE_HEADER_
#include "mbd.h"
#include "../../lsf/intlib/sshare.h"

/* Fairshare scheduling plugin
 */
struct fair_sched {
    char *name;
    void *handle;
    struct tree_ *root;
    int (*init)(struct qData *, struct userConf *);
};

#endif