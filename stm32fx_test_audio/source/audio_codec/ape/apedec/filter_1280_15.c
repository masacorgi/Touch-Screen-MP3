/*

libdemac - A Monkey's Audio decoder

$Id: filter_1280_15.c 24376 2010-01-30 02:20:54Z unhelpful $

Copyright (C) Dave Chapman 2007

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA

*/
#include "hwdefs.h"
#include "demac_config.h"
#ifndef FILTER256_IRAM
#undef ICODE_ATTR_DEMAC
#define ICODE_ATTR_DEMAC
#endif
#define ORDER 1280
#define FRACBITS 15
#include "filter.c"
