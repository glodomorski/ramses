//  -------------------------------------------------------------------------
//  Copyright (C) 2014 BMW Car IT GmbH
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

// API
#include <gtest/gtest.h>
#include "ramses-client-api/EffectDescription.h"
#include "ramses-utils.h"

#include "RamsesClientImpl.h"
#include "SceneConfigImpl.h"
#include "Utils/File.h"
#include "gtest/gtest-typed-test.h"
#include "RamsesObjectTestTypes.h"
#include "EffectImpl.h"
#include "ClientTestUtils.h"
#include "ClientApplicationLogic.h"
#include "SceneAPI/SceneId.h"
#include "Collections/String.h"
#include "ClientEventHandlerMock.h"

#include "SceneReferencing/SceneReferenceEvent.h"

namespace ramses
{
    using namespace testing;

    class ALocalRamsesClient : public LocalTestClient, public Test
    {
    public:
        ALocalRamsesClient()
        {
            effectDescriptionEmpty.setVertexShader("void main(void) {gl_Position=vec4(0);}");
            effectDescriptionEmpty.setFragmentShader("void main(void) {gl_FragColor=vec4(0);}");
        }

    protected:
        ramses::EffectDescription effectDescriptionEmpty;
    };

    class ARamsesClient : public Test
    {
    public:
        ARamsesClient()
            : m_client(*m_framework.createClient("client"))
        {
        }

    protected:
        RamsesFramework m_framework;
        RamsesClient& m_client;
    };

    TEST_F(ARamsesClient, canBeValidated)
    {
        ramses::StatusObjectImpl::StatusObjectSet visitedObjects;
        EXPECT_EQ(StatusOK, m_client.impl.validate(0, visitedObjects));
    }

    TEST_F(ARamsesClient, failsValidationWhenContainsSceneWithInvalidRenderPass)
    {
        ramses::Scene* scene = m_client.createScene(sceneId_t(1), ramses::SceneConfig(), "");
        ASSERT_TRUE(nullptr != scene);

        RenderPass* passWithoutCamera = scene->createRenderPass();
        ASSERT_TRUE(nullptr != passWithoutCamera);

        EXPECT_NE(StatusOK, scene->validate());
        EXPECT_NE(StatusOK, m_client.validate());
    }

    TEST_F(ARamsesClient, failsValidationWhenContainsSceneWithInvalidCamera)
    {
        ramses::Scene* scene = m_client.createScene(sceneId_t(1), ramses::SceneConfig(), "");
        ASSERT_TRUE(nullptr != scene);

        Camera* cameraWithoutValidValues = scene->createPerspectiveCamera();
        ASSERT_TRUE(nullptr != cameraWithoutValidValues);

        EXPECT_NE(StatusOK, scene->validate());
        EXPECT_NE(StatusOK, m_client.validate());
    }

    TEST_F(ARamsesClient, noEventHandlerCallbacksIfNoEvents)
    {
        StrictMock<ClientEventHandlerMock> eventHandlerMock;
        m_client.dispatchEvents(eventHandlerMock);
    }

    // Not really useful and behavior is not defined, but should not crash at least
    TEST_F(ARamsesClient, canLiveParallelToAnotherClientUsingTheSameFramework)
    {
        RamsesClient& secondClient(*m_framework.createClient("client"));
        (void)secondClient;  //unused on purpose

        m_framework.connect();
        EXPECT_TRUE(m_framework.isConnected());
    }

    TEST_F(ALocalRamsesClient, canGetResourceByID)
    {
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr != effectFixture);

        const resourceId_t resourceID = effectFixture->getResourceId();
        ramses::Resource* resource = client.findResourceById(resourceID);
        ASSERT_TRUE(nullptr != resource);

        const ramses::Effect* effectFound = RamsesUtils::TryConvert<ramses::Effect>(*resource);
        ASSERT_TRUE(nullptr != effectFound);

        ASSERT_TRUE(effectFound == effectFixture);

        const resourceId_t nonExistEffectId = { 0, 0 };
        ASSERT_TRUE(nullptr == client.findResourceById(nonExistEffectId));
    }

    TEST_F(ALocalRamsesClient, returnsNULLWhenResourceWithIDCannotBeFound)
    {
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr != effectFixture);

        const resourceId_t nonExistEffectId = { 0, 0 };
        ASSERT_TRUE(nullptr == client.findResourceById(nonExistEffectId));
    }

    TEST_F(ALocalRamsesClient, returnsNULLWhenTryingToFindDeletedResource)
    {
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr != effectFixture);

        const resourceId_t resourceID = effectFixture->getResourceId();
        ramses::Resource* resource = client.findResourceById(resourceID);
        ASSERT_TRUE(nullptr != resource);

        client.destroy(*effectFixture);

        ramses::Resource* resourceFound = client.findResourceById(resourceID);
        ASSERT_TRUE(nullptr == resourceFound);
    }

    TEST_F(ALocalRamsesClient, requestNonexistantStatusMessage)
    {
        const char* msg = client.getStatusMessage(0xFFFFFFFF);
        EXPECT_TRUE(nullptr != msg);
    }

    TEST(RamsesClient, canCreateClientWithNULLNameAndCmdLineArguments)
    {
        ramses::RamsesFramework framework(0, static_cast<const char**>(nullptr));
        EXPECT_NE(framework.createClient(nullptr), nullptr);
        EXPECT_FALSE(framework.isConnected());
    }

    TEST(RamsesClient, canCreateClientWithoutCmdLineArguments)
    {
        ramses::RamsesFramework framework;
        EXPECT_NE(framework.createClient(nullptr), nullptr);
        EXPECT_FALSE(framework.isConnected());
    }

    TEST_F(ALocalRamsesClient, createsSceneWithGivenId)
    {
        const sceneId_t sceneId(33u);
        ramses::Scene* scene = client.createScene(sceneId);
        ASSERT_TRUE(scene != nullptr);
        EXPECT_EQ(sceneId, scene->getSceneId());
    }

    TEST_F(ALocalRamsesClient, createdSceneHasClientReference)
    {
        ramses::Scene* scene = client.createScene(sceneId_t(1));
        ASSERT_TRUE(scene != nullptr);
        EXPECT_EQ(&client, &scene->getRamsesClient());
    }

    TEST_F(ALocalRamsesClient, createsSceneFailsWithInvalidId)
    {
        EXPECT_EQ(nullptr, client.createScene(sceneId_t{}));
    }

    TEST_F(ALocalRamsesClient, simpleSceneGetsDestroyedProperlyWithoutExplicitDestroyCall)
    {
        ramses::Scene* scene = client.createScene(sceneId_t(1u));
        EXPECT_TRUE(scene != nullptr);
        ramses::Node* node = scene->createNode("node");
        EXPECT_TRUE(node != nullptr);
    }

    // effect from string: valid uses
    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_withName)
    {
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr != effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_withDefines)
    {
        effectDescriptionEmpty.addCompilerDefine("float dummy;");
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr != effectFixture);
    }

    // effect from string: invalid uses
    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_invalidVertexShader)
    {
        effectDescriptionEmpty.setVertexShader("void main(void) {dsadsadasd}");
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_emptyVertexShader)
    {
        effectDescriptionEmpty.setVertexShader("");
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_invalidFragmentShader)
    {
        effectDescriptionEmpty.setFragmentShader("void main(void) {dsadsadasd}");
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_emptyFragmentShader)
    {
        effectDescriptionEmpty.setFragmentShader("");
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_invalidDefines)
    {
        effectDescriptionEmpty.addCompilerDefine("thisisinvalidstuff\n8fd7f9ds");
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_withInputSemantics)
    {
        effectDescriptionEmpty.setVertexShader(
            "uniform mat4 someMatrix;"
            "void main(void)"
            "{"
            "gl_Position = someMatrix * vec4(1.0);"
            "}");
        effectDescriptionEmpty.setUniformSemantic("someMatrix", ramses::EEffectUniformSemantic_ProjectionMatrix);
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr != effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSLString_withInputSemanticsOfWrongType)
    {
        effectDescriptionEmpty.setVertexShader(
            "uniform mat2 someMatrix;"
            "void main(void)"
            "{"
            "gl_Position = someMatrix * vec4(1.0);"
            "}");
        effectDescriptionEmpty.setUniformSemantic("someMatrix", ramses::EEffectUniformSemantic_ProjectionMatrix);
        const ramses::Effect* effectFixture = client.createEffect(effectDescriptionEmpty, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_FALSE(nullptr != effectFixture);
    }

    // effect from file: valid uses
    TEST_F(ALocalRamsesClient, createEffectFromGLSL_withName)
    {
        ramses::EffectDescription effectDesc;
        effectDesc.setVertexShaderFromFile("res/ramses-client-test_minimalShader.vert");
        effectDesc.setFragmentShaderFromFile("res/ramses-client-test_minimalShader.frag");
        const ramses::Effect* effectFixture = client.createEffect(effectDesc, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr != effectFixture);
    }


    // effect from file: invalid uses
    TEST_F(ALocalRamsesClient, createEffectFromGLSL_nonExistantVertexShader)
    {
        ramses::EffectDescription effectDesc;
        effectDesc.setVertexShaderFromFile("res/this_file_should_not_exist_fdsfdsjf84w9wufw.vert");
        effectDesc.setFragmentShaderFromFile("res/ramses-client-test_minimalShader.frag");
        const ramses::Effect* effectFixture = client.createEffect(effectDesc, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSL_nonExistantFragmentShader)
    {
        ramses::EffectDescription effectDesc;
        effectDesc.setVertexShaderFromFile("res/ramses-client-test_minimalShader.frag");
        effectDesc.setFragmentShaderFromFile("res/this_file_should_not_exist_fdsfdsjf84w9wufw.vert");
        const ramses::Effect* effectFixture = client.createEffect(effectDesc, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSL_NULLVertexShader)
    {
        ramses::EffectDescription effectDesc;
        effectDesc.setVertexShaderFromFile("");
        effectDesc.setFragmentShaderFromFile("res/this_file_should_not_exist_fdsfdsjf84w9wufw.vert");
        const ramses::Effect* effectFixture = client.createEffect(effectDesc, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, createEffectFromGLSL_NULLFragmentShader)
    {
        ramses::EffectDescription effectDesc;
        effectDesc.setVertexShaderFromFile("res/this_file_should_not_exist_fdsfdsjf84w9wufw.vert");
        effectDesc.setFragmentShaderFromFile("");
        const ramses::Effect* effectFixture = client.createEffect(effectDesc, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);
    }

    TEST_F(ALocalRamsesClient, verifyHLAPILogCanHandleNullPtrReturnWhenEnabled)
    {
        ramses_internal::ELogLevel oldLogLevel = ramses_internal::CONTEXT_HLAPI_CLIENT.getLogLevel();
        ramses_internal::CONTEXT_HLAPI_CLIENT.setLogLevel(ramses_internal::ELogLevel::Trace);

        ramses::EffectDescription effectDesc;
        effectDesc.setVertexShaderFromFile("res/this_file_should_not_exist_fdsfdsjf84w9wufw.vert");
        effectDesc.setFragmentShaderFromFile("");
        const ramses::Effect* effectFixture = client.createEffect(effectDesc, ramses::ResourceCacheFlag_DoNotCache, "name");
        EXPECT_TRUE(nullptr == effectFixture);

        ramses_internal::CONTEXT_HLAPI_CLIENT.setLogLevel(oldLogLevel);
    }

    TEST_F(ALocalRamsesClient, requestValidStatusMessage)
    {
        LocalTestClient otherClient;
        Scene& scene = otherClient.createObject<Scene>();

        ramses::status_t status = client.destroy(scene);
        EXPECT_NE(ramses::StatusOK, status);
        const char* msg = client.getStatusMessage(status);
        EXPECT_TRUE(nullptr != msg);
    }

    TEST_F(ALocalRamsesClient, isUnpublishedOnDestroy)
    {
        const sceneId_t sceneId(45u);
        ramses::Scene* scene = client.createScene(sceneId);

        using ramses_internal::SceneInfoVector;
        using ramses_internal::SceneInfo;
        ramses_internal::SceneId internalSceneId(sceneId.getValue());

        EXPECT_CALL(sceneActionsCollector, handleNewScenesAvailable(SceneInfoVector(1, SceneInfo(internalSceneId, ramses_internal::String(scene->getName()))), testing::_, testing::_));
        EXPECT_CALL(sceneActionsCollector, handleInitializeScene(testing::_, testing::_));
        EXPECT_CALL(sceneActionsCollector, handleSceneActionList_rvr(ramses_internal::SceneId(sceneId.getValue()), testing::_, testing::_, testing::_));
        EXPECT_CALL(sceneActionsCollector, handleScenesBecameUnavailable(SceneInfoVector(1, SceneInfo(internalSceneId)), testing::_));

        scene->publish(ramses::EScenePublicationMode_LocalOnly);
        scene->flush();
        EXPECT_TRUE(client.impl.getClientApplication().isScenePublished(internalSceneId));

        client.destroy(*scene);
        EXPECT_FALSE(client.impl.getClientApplication().isScenePublished(internalSceneId));
    }

    TEST_F(ALocalRamsesClient, returnsNullptrOnFindSceneReferenceIfThereIsNoSceneReference)
    {
        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 123 }, sceneId_t{ 456 }), nullptr);
        client.createScene(sceneId_t{ 123 });
        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 123 }, sceneId_t{ 456 }), nullptr);
    }

    TEST_F(ALocalRamsesClient, returnsNullptrOnFindSceneReferenceIfWrongReferencedSceneIdIsProvided)
    {
        auto scene = client.createScene(sceneId_t{ 123 });
        scene->createSceneReference(sceneId_t{ 456 });

        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 123 }, sceneId_t{ 1 }), nullptr);
    }

    TEST_F(ALocalRamsesClient, returnsNullptrOnFindSceneReferenceIfWrongMasterSceneIdIsProvided)
    {
        auto scene = client.createScene(sceneId_t{ 123 });
        scene->createSceneReference(sceneId_t{ 456 });
        auto scene2 = client.createScene(sceneId_t{ 1234 });
        scene2->createSceneReference(sceneId_t{ 4567 });

        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 1 }, sceneId_t{ 456 }), nullptr);
        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 1234 }, sceneId_t{ 456 }), nullptr);
        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 123 }, sceneId_t{ 4567 }), nullptr);
    }

    TEST_F(ALocalRamsesClient, returnsSceneReferenceOnFindSceneReference)
    {
        auto scene = client.createScene(sceneId_t{ 123 });
        auto scene2 = client.createScene(sceneId_t{ 1234 });
        auto sr = scene->createSceneReference(sceneId_t{ 456 });
        auto sr2 = scene->createSceneReference(sceneId_t{ 12345 });
        auto sr3 = scene2->createSceneReference(sceneId_t{ 12345 });

        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 123 }, sceneId_t{ 456 }), sr);
        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 123 }, sceneId_t{ 12345 }), sr2);
        EXPECT_EQ(client.impl.findSceneReference(sceneId_t{ 1234 }, sceneId_t{ 12345 }), sr3);
    }

    TEST_F(ALocalRamsesClient, callsAppropriateNotificationForSceneStateChangedEvent)
    {
        constexpr auto masterScene = ramses_internal::SceneId{ 123 };
        constexpr auto reffedScene = ramses_internal::SceneId{ 456 };

        auto scene = client.createScene(sceneId_t{ masterScene.getValue() });
        auto sr = scene->createSceneReference(sceneId_t{ reffedScene.getValue() });

        ramses_internal::SceneReferenceEvent event(masterScene);
        event.referencedScene = reffedScene;
        event.type = ramses_internal::SceneReferenceEventType::SceneStateChanged;
        event.sceneState = ramses_internal::RendererSceneState::Rendered;

        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});

        testing::StrictMock<ClientEventHandlerMock> handler;
        EXPECT_CALL(handler, sceneReferenceStateChanged(_, RendererSceneState::Rendered)).WillOnce([sr](SceneReference& ref, RendererSceneState)
            {
                EXPECT_EQ(&ref, sr);
            });
        client.dispatchEvents(handler);
    }

    TEST_F(ALocalRamsesClient, callsAppropriateNotificationForSceneFlushedEvent)
    {
        constexpr auto masterScene = ramses_internal::SceneId{ 123 };
        constexpr auto reffedScene = ramses_internal::SceneId{ 456 };

        auto scene = client.createScene(sceneId_t{ masterScene.getValue() });
        auto sr = scene->createSceneReference(sceneId_t{ reffedScene.getValue() });

        ramses_internal::SceneReferenceEvent event(masterScene);
        event.referencedScene = reffedScene;
        event.type = ramses_internal::SceneReferenceEventType::SceneFlushed;
        event.tag = ramses_internal::SceneVersionTag{ 567 };

        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});

        testing::StrictMock<ClientEventHandlerMock> handler;
        EXPECT_CALL(handler, sceneReferenceFlushed(_, sceneVersionTag_t{ 567 })).WillOnce([sr](SceneReference& ref, sceneVersionTag_t)
            {
                EXPECT_EQ(&ref, sr);
            });
        client.dispatchEvents(handler);
    }

    TEST_F(ALocalRamsesClient, callsAppropriateNotificationForDataLinkedEvent)
    {
        constexpr auto masterScene = ramses_internal::SceneId{ 123 };
        constexpr auto reffedScene = ramses_internal::SceneId{ 456 };

        auto scene = client.createScene(sceneId_t{ masterScene.getValue() });
        scene->createSceneReference(sceneId_t{ reffedScene.getValue() });

        ramses_internal::SceneReferenceEvent event(masterScene);
        event.type = ramses_internal::SceneReferenceEventType::DataLinked;
        event.providerScene = masterScene;
        event.consumerScene = reffedScene;
        event.dataProvider = ramses_internal::DataSlotId{ 123 };
        event.dataConsumer = ramses_internal::DataSlotId{ 987 };
        event.status = false;

        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});

        testing::StrictMock<ClientEventHandlerMock> handler;
        EXPECT_CALL(handler, dataLinked(sceneId_t{ masterScene.getValue() }, dataProviderId_t{ 123 }, sceneId_t{ reffedScene.getValue() }, dataConsumerId_t{ 987 }, false));
        client.dispatchEvents(handler);
    }

    TEST_F(ALocalRamsesClient, callsAppropriateNotificationForDataUnlinkedEvent)
    {
        constexpr auto masterScene = ramses_internal::SceneId{ 123 };
        constexpr auto reffedScene = ramses_internal::SceneId{ 456 };

        auto scene = client.createScene(sceneId_t{ masterScene.getValue() });
        scene->createSceneReference(sceneId_t{ reffedScene.getValue() });

        ramses_internal::SceneReferenceEvent event(masterScene);
        event.type = ramses_internal::SceneReferenceEventType::DataUnlinked;
        event.consumerScene = reffedScene;
        event.dataConsumer = ramses_internal::DataSlotId{ 987 };
        event.status = true;

        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});

        testing::StrictMock<ClientEventHandlerMock> handler;
        EXPECT_CALL(handler, dataUnlinked(sceneId_t{ reffedScene.getValue() }, dataConsumerId_t{ 987 }, true));
        client.dispatchEvents(handler);
    }

    TEST_F(ALocalRamsesClient, callsNotificationForEachEvent)
    {
        constexpr auto masterScene = ramses_internal::SceneId{ 123 };
        constexpr auto reffedScene = ramses_internal::SceneId{ 456 };

        auto scene = client.createScene(sceneId_t{ masterScene.getValue() });
        auto sr = scene->createSceneReference(sceneId_t{ reffedScene.getValue() });

        ramses_internal::SceneReferenceEvent event(masterScene);
        event.referencedScene = reffedScene;
        event.type = ramses_internal::SceneReferenceEventType::SceneFlushed;
        event.tag = ramses_internal::SceneVersionTag{ 567 };
        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});
        event.tag = ramses_internal::SceneVersionTag{ 568 };
        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});
        event.tag = ramses_internal::SceneVersionTag{ 569 };
        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});

        testing::StrictMock<ClientEventHandlerMock> handler;
        EXPECT_CALL(handler, sceneReferenceFlushed(_, sceneVersionTag_t{ 567 })).WillOnce([sr](SceneReference& ref, sceneVersionTag_t)
            {
                EXPECT_EQ(&ref, sr);
            });
        EXPECT_CALL(handler, sceneReferenceFlushed(_, sceneVersionTag_t{ 568 })).WillOnce([sr](SceneReference& ref, sceneVersionTag_t)
            {
                EXPECT_EQ(&ref, sr);
            });
        EXPECT_CALL(handler, sceneReferenceFlushed(_, sceneVersionTag_t{ 569 })).WillOnce([sr](SceneReference& ref, sceneVersionTag_t)
            {
                EXPECT_EQ(&ref, sr);
            });
        client.dispatchEvents(handler);
    }

    TEST_F(ALocalRamsesClient, doesNotCallAnyNotificationIfSceneReferenceDoesNotExistForNonDataLinkEvents)
    {
        constexpr auto masterScene = ramses_internal::SceneId{ 123 };
        constexpr auto reffedScene = ramses_internal::SceneId{ 456 };

        client.createScene(sceneId_t{ masterScene.getValue() });

        ramses_internal::SceneReferenceEvent event(masterScene);
        event.referencedScene = reffedScene;
        event.type = ramses_internal::SceneReferenceEventType::SceneFlushed;
        event.tag = ramses_internal::SceneVersionTag{ 567 };

        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});

        event.referencedScene = reffedScene;
        event.type = ramses_internal::SceneReferenceEventType::SceneFlushed;
        event.tag = ramses_internal::SceneVersionTag{ 567 };

        client.impl.getClientApplication().handleSceneReferenceEvent(event, {});

        testing::StrictMock<ClientEventHandlerMock> handler;
        client.dispatchEvents(handler);
    }

    TEST(ARamsesFrameworkImplInAClientLib, canCreateAClient)
    {
        ramses::RamsesFramework fw;

        auto client = fw.createClient("client");
        EXPECT_NE(client, nullptr);
    }

    TEST(ARamsesFrameworkImplInAClientLib, canCreateMultipleClients)
    {
        ramses::RamsesFramework fw;

        auto client1 = fw.createClient("first client");
        auto client2 = fw.createClient("second client");
        EXPECT_NE(nullptr, client1);
        EXPECT_NE(nullptr, client2);
    }

    TEST(ARamsesFrameworkImplInAClientLib, acceptsLocallyCreatedClientsForDestruction)
    {
        ramses::RamsesFramework fw;

        auto client1 = fw.createClient("first client");
        auto client2 = fw.createClient("second client");
        EXPECT_EQ(StatusOK, fw.destroyClient(*client1));
        EXPECT_EQ(StatusOK, fw.destroyClient(*client2));
    }

    TEST(ARamsesFrameworkImplInAClientLib, doesNotAcceptForeignCreatedClientsForDestruction)
    {
        ramses::RamsesFramework fw1;
        ramses::RamsesFramework fw2;

        auto client1 = fw1.createClient("first client");
        auto client2 = fw2.createClient("second client");
        EXPECT_NE(StatusOK, fw2.destroyClient(*client1));
        EXPECT_NE(StatusOK, fw1.destroyClient(*client2));
    }

    TEST(ARamsesFrameworkImplInAClientLib, doesNotAcceptSameClientTwiceForDestruction)
    {
        ramses::RamsesFramework fw;

        auto client = fw.createClient("client");
        EXPECT_EQ(StatusOK, fw.destroyClient(*client));
        EXPECT_NE(StatusOK, fw.destroyClient(*client));
    }

    TEST(ARamsesFrameworkImplInAClientLib, canCreateDestroyAndRecreateAClient)
    {
        ramses::RamsesFramework fw;

        auto client = fw.createClient("client");
        EXPECT_NE(nullptr, client);
        EXPECT_EQ(fw.destroyClient(*client), StatusOK);
        client = fw.createClient("client");
        EXPECT_NE(nullptr, client);
    }
}
