/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * recode.h : charsets translating routines declarations
 *
 * Compiled from hpt/recode
 * by Stas Degteff <g@grumbler.org>, 2:5080/102@fidonet
 *
 * Code taken from ifmail written by Eugene G. Crosser <crosser@pccross.msk.su>
 * Ported to HPT by Dmitry Borowskoy <dim@bacup.ru>
 *
 * Portions copyright (C) Matthias Tichy
 *                        Fido:     2:2433/1245 2:2433/1247 2:2432/605.14
 *                        Internet: mtt@tichy.de
 * Portions copyright (C) Max Levenkov
 *                        Fido:     2:5000/117
 *                        Internet: sackett@mail.ru
 *
 * This file is part of FIDOCONFIG library (part of the Husky FIDOnet
 * software project)
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FIDOCONFIG library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FIDOCONFIG library; see the file COPYING.  If not, write
 * to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA
 * or visit http://www.gnu.org
 *****************************************************************************
 * $Id$
 */

#ifndef RECODE_H
#define RECODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "typesize.h"
#include "fidoconf.h"

FCONF_EXT VOID initCharsets(VOID);
FCONF_EXT VOID doneCharsets(VOID);
FCONF_EXT VOID recodeToInternalCharset( CHAR *string);
FCONF_EXT VOID recodeToTransportCharset( CHAR *string);
FCONF_EXT void getctab(CHAR *dest,  UCHAR *charMapFileName);

FCONF_EXT CHAR *intab;
FCONF_EXT CHAR *outtab;

#ifdef __cplusplus
}
#endif

#endif
