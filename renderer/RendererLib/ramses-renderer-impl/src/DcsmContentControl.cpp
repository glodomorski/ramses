//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#include "ramses-renderer-api/DcsmContentControl.h"
#include "DcsmContentControlImpl.h"
#include "ramses-renderer-api/RamsesRenderer.h"
#include "ramses-framework-api/RamsesFramework.h"
#include "ramses-framework-api/DcsmConsumer.h"
#include "DcsmConsumerImpl.h"
#include "APILoggingMacros.h"
#include "RamsesFrameworkTypesImpl.h"

namespace ramses
{
    DcsmContentControl::DcsmContentControl(DcsmContentControlImpl& impl_)
        : StatusObject(impl_)
        , m_impl(impl_)
    {
    }

    DcsmContentControl::~DcsmContentControl() = default;

    status_t DcsmContentControl::requestContentReady(ContentID contentID, uint64_t timeOut)
    {
        const auto status = m_impl.requestContentReady(contentID, timeOut);
        LOG_HL_RENDERER_API2(status, contentID, timeOut);
        return status;
    }

    status_t DcsmContentControl::showContent(ContentID contentID, AnimationInformation timingInfo)
    {
        const auto status = m_impl.showContent(contentID, timingInfo);
        LOG_HL_RENDERER_API3(status, contentID, timingInfo.startTime, timingInfo.finishTime);
        return status;
    }

    status_t DcsmContentControl::hideContent(ContentID contentID, AnimationInformation timingInfo)
    {
        const auto status = m_impl.hideContent(contentID, timingInfo);
        LOG_HL_RENDERER_API3(status, contentID, timingInfo.startTime, timingInfo.finishTime);
        return status;
    }

    status_t DcsmContentControl::releaseContent(ContentID contentID, AnimationInformation timingInfo)
    {
        const auto status = m_impl.releaseContent(contentID, timingInfo);
        LOG_HL_RENDERER_API3(status, contentID, timingInfo.startTime, timingInfo.finishTime);
        return status;
    }

    status_t DcsmContentControl::setCategorySize(Category categoryId, SizeInfo size, AnimationInformation timingInfo)
    {
        const auto status = m_impl.setCategorySize(categoryId, size, timingInfo);
        LOG_HL_RENDERER_API5(status, categoryId, size.width, size.height, timingInfo.startTime, timingInfo.finishTime);
        return status;
    }

    status_t DcsmContentControl::acceptStopOffer(ContentID contentID, AnimationInformation timingInfo)
    {
        const auto status = m_impl.acceptStopOffer(contentID, timingInfo);
        LOG_HL_RENDERER_API3(status, contentID, timingInfo.startTime, timingInfo.finishTime);
        return status;
    }

    status_t DcsmContentControl::assignContentToDisplayBuffer(ContentID contentID, displayBufferId_t displayBuffer, int32_t renderOrder)
    {
        const auto status = m_impl.assignContentToDisplayBuffer(contentID, displayBuffer, renderOrder);
        LOG_HL_RENDERER_API3(status, contentID, displayBuffer, renderOrder);
        return status;
    }

    status_t DcsmContentControl::setDisplayBufferClearColor(displayId_t display, displayBufferId_t displayBuffer, float r, float g, float b, float a)
    {
        const auto status = m_impl.setDisplayBufferClearColor(display, displayBuffer, r, g, b, a);
        LOG_HL_RENDERER_API5(status, displayBuffer, r, g, b, a);
        return status;
    }

    status_t DcsmContentControl::linkOffscreenBuffer(displayBufferId_t offscreenBufferId, ContentID consumerContentID, dataConsumerId_t consumerId)
    {
        const auto status = m_impl.linkOffscreenBuffer(offscreenBufferId, consumerContentID, consumerId);
        LOG_HL_RENDERER_API3(status, offscreenBufferId, consumerContentID, consumerId);
        return status;
    }

    status_t DcsmContentControl::linkData(ContentID providerContentID, dataProviderId_t providerId, ContentID consumerContentID, dataConsumerId_t consumerId)
    {
        const auto status = m_impl.linkData(providerContentID, providerId, consumerContentID, consumerId);
        LOG_HL_RENDERER_API4(status, providerContentID, providerId, consumerContentID, consumerId);
        return status;
    }

    status_t DcsmContentControl::update(uint64_t timeStampNow, IDcsmContentControlEventHandler& eventHandler)
    {
        const auto status = m_impl.update(timeStampNow, eventHandler);
        LOG_HL_RENDERER_API2(status, timeStampNow, LOG_API_GENERIC_OBJECT_STRING(eventHandler));
        return status;
    }
}
