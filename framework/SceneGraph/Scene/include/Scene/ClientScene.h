//  -------------------------------------------------------------------------
//  Copyright (C) 2014 BMW Car IT GmbH
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef RAMSES_CLIENTSCENE_H
#define RAMSES_CLIENTSCENE_H

#include "Scene/DataLayoutCachedScene.h"
#include "SceneReferencing/SceneReferenceAction.h"
#include "Utils/StatisticCollection.h"

namespace ramses_internal
{
    // The client scene is just a wrapper for concrete implementation of a low level scene
    // together with some additional data used in client side logic
    class ClientScene final : public DataLayoutCachedScene
    {
    public:
        ClientScene(const SceneInfo& sceneInfo = {})
            : DataLayoutCachedScene(sceneInfo)
        {
        }

        StatisticCollectionScene& getStatisticCollection()
        {
            return m_statisticCollection;
        }

    private:
        StatisticCollectionScene m_statisticCollection;
    };
}

#endif
