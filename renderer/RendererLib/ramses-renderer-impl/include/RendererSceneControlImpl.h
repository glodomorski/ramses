//  -------------------------------------------------------------------------
//  Copyright (C) 2020 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef RAMSES_RENDERERSCENECONTROLIMPL_H
#define RAMSES_RENDERERSCENECONTROLIMPL_H

#include "ramses-renderer-api/RendererSceneControl.h"
#include "StatusObjectImpl.h"
#include "RendererSceneControlLogic.h"
#include "RendererLib/RendererCommands.h"
#include "RendererLib/RendererEvent.h"
#include "Math3d/Vector4.h"

namespace ramses
{
    class RamsesRendererImpl;

    class IRendererSceneControl
    {
    public:
        virtual status_t setSceneState(sceneId_t sceneId, RendererSceneState state) = 0;
        virtual status_t setSceneMapping(sceneId_t sceneId, displayId_t displayId) = 0;
        virtual status_t setSceneDisplayBufferAssignment(sceneId_t sceneId, displayBufferId_t displayBuffer, int32_t sceneRenderOrder) = 0;
        virtual status_t setDisplayBufferClearColor(displayId_t display, displayBufferId_t displayBuffer, float r, float g, float b, float a) = 0;
        virtual status_t linkOffscreenBuffer(displayBufferId_t offscreenBufferId, sceneId_t consumerSceneId, dataConsumerId_t consumerDataSlotId) = 0;
        virtual status_t linkData(sceneId_t providerSceneId, dataProviderId_t providerId, sceneId_t consumerSceneId, dataConsumerId_t consumerId) = 0;
        virtual status_t unlinkData(sceneId_t consumerSceneId, dataConsumerId_t consumerId) = 0;
        virtual status_t flush() = 0;
        virtual status_t dispatchEvents(IRendererSceneControlEventHandler& eventHandler) = 0;

        virtual ~IRendererSceneControl() = default;
    };

    class RendererSceneControlImpl final : public IRendererSceneControl, public StatusObjectImpl
    {
    public:
        RendererSceneControlImpl(RamsesRendererImpl& renderer);

        virtual status_t setSceneState(sceneId_t sceneId, RendererSceneState state) override;
        virtual status_t setSceneMapping(sceneId_t sceneId, displayId_t displayId) override;
        virtual status_t setSceneDisplayBufferAssignment(sceneId_t sceneId, displayBufferId_t displayBuffer, int32_t sceneRenderOrder) override;
        virtual status_t setDisplayBufferClearColor(displayId_t display, displayBufferId_t displayBuffer, float r, float g, float b, float a) override;
        virtual status_t linkOffscreenBuffer(displayBufferId_t offscreenBufferId, sceneId_t consumerSceneId, dataConsumerId_t consumerDataSlotId) override;
        virtual status_t linkData(sceneId_t providerSceneId, dataProviderId_t providerId, sceneId_t consumerSceneId, dataConsumerId_t consumerId) override;
        virtual status_t unlinkData(sceneId_t consumerSceneId, dataConsumerId_t consumerId) override;

        virtual status_t flush() override;
        virtual status_t dispatchEvents(IRendererSceneControlEventHandler& eventHandler) override;

        const ramses_internal::RendererCommands& getPendingCommands() const;

    private:
        RamsesRendererImpl& m_renderer;
        ramses_internal::RendererCommands m_pendingRendererCommands;

        struct SceneInfo
        {
            bool mappingSet = false;
            RendererSceneState currState = RendererSceneState::Unavailable;
            RendererSceneState targetState = RendererSceneState::Unavailable;
        };
        std::unordered_map<sceneId_t, SceneInfo> m_sceneInfos;

        // keep allocated containers which are used to swap internal data
        ramses_internal::RendererEventVector m_tempRendererEvents;
    };
}

#endif
