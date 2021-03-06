//  -------------------------------------------------------------------------
//  Copyright (C) 2018 BMW Car IT GmbH
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef RAMSES_TEXT_TEXTTYPESIMPL_H
#define RAMSES_TEXT_TEXTTYPESIMPL_H

#include "ramses-text-api/FontInstanceId.h"
#include "ramses-text-api/Glyph.h"
#include "ramses-text-api/TextLine.h"
#include "Common/StronglyTypedValue.h"

MAKE_STRONGLYTYPEDVALUE_PRINTABLE(ramses::FontInstanceId);
MAKE_STRONGLYTYPEDVALUE_PRINTABLE(ramses::GlyphId);
MAKE_STRONGLYTYPEDVALUE_PRINTABLE(ramses::TextLineId);
MAKE_STRONGLYTYPEDVALUE_PRINTABLE(ramses::FontId);

#endif
