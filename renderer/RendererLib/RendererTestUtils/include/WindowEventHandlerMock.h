//  -------------------------------------------------------------------------
//  Copyright (C) 2012 BMW Car IT GmbH
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef RAMSES_WINDOWEVENTHANDLERMOCK_H
#define RAMSES_WINDOWEVENTHANDLERMOCK_H

#include "renderer_common_gmock_header.h"
#include "gmock/gmock.h"
#include "RendererAPI/IWindowEventHandler.h"

#include "RendererAPI/IWindow.h"

namespace ramses_internal
{
    class WindowEventHandlerMock : public IWindowEventHandler
    {
    public:
        WindowEventHandlerMock();
        ~WindowEventHandlerMock() override;

        MOCK_METHOD2(onResize, void(UInt32 width, UInt32 height));
        MOCK_METHOD3(onKeyEvent, void(EKeyEventType event, UInt32 modifiers, EKeyCode keyCode));
        MOCK_METHOD3(onMouseEvent, void(EMouseEventType event, Int32 posX, Int32 posY));
        MOCK_METHOD0(onClose, void());
        MOCK_METHOD2(onWindowMove, void(Int32 posX, Int32 posY));
    };
}

#endif
