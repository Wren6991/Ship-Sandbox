#include <GameLib/GameEventDispatcher.h>

#include "gmock/gmock.h"

class _MockHandler : public IGameEventHandler
{
public:
    
    MOCK_METHOD2(OnDestroy, void(Material const * material, unsigned int size));
    MOCK_METHOD2(OnBreak, void(Material const * material, unsigned int size));
    MOCK_METHOD0(OnSinkingBegin, void());
};

using namespace ::testing;

using MockHandler = StrictMock<_MockHandler>;

/////////////////////////////////////////////////////////////////

TEST(GameEventDispatcherTests, Aggregates_OnDestroy)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);

    EXPECT_CALL(handler, OnDestroy(_, _)).Times(0);

    dispatcher.OnStepStart();    

    dispatcher.OnDestroy(pm1, 3);
    dispatcher.OnDestroy(pm1, 2);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnDestroy(pm1, 5)).Times(1);

    dispatcher.OnStepEnd();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnDestroy_MultipleMaterials)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);
    Material * pm2 = reinterpret_cast<Material *>(21);

    EXPECT_CALL(handler, OnDestroy(_, _)).Times(0);

    dispatcher.OnStepStart();

    dispatcher.OnDestroy(pm2, 1);
    dispatcher.OnDestroy(pm1, 3);
    dispatcher.OnDestroy(pm2, 2);
    dispatcher.OnDestroy(pm1, 9);
    dispatcher.OnDestroy(pm1, 1);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnDestroy(pm1, 13)).Times(1);
    EXPECT_CALL(handler, OnDestroy(pm2, 3)).Times(1);

    dispatcher.OnStepEnd();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnBreak)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);

    EXPECT_CALL(handler, OnBreak(_, _)).Times(0);

    dispatcher.OnStepStart();

    dispatcher.OnBreak(pm1, 3);
    dispatcher.OnBreak(pm1, 2);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnBreak(pm1, 5)).Times(1);

    dispatcher.OnStepEnd();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnBreak_MultipleMaterials)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);
    Material * pm2 = reinterpret_cast<Material *>(21);

    EXPECT_CALL(handler, OnBreak(_, _)).Times(0);

    dispatcher.OnStepStart();

    dispatcher.OnBreak(pm2, 1);
    dispatcher.OnBreak(pm1, 3);
    dispatcher.OnBreak(pm2, 2);
    dispatcher.OnBreak(pm1, 9);
    dispatcher.OnBreak(pm1, 1);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnBreak(pm1, 13)).Times(1);
    EXPECT_CALL(handler, OnBreak(pm2, 3)).Times(1);

    dispatcher.OnStepEnd();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnSinkingBegin)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    EXPECT_CALL(handler, OnSinkingBegin()).Times(0);

    dispatcher.OnStepStart();

    dispatcher.OnSinkingBegin();
    dispatcher.OnSinkingBegin();

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnSinkingBegin()).Times(1);

    dispatcher.OnStepEnd();

    Mock::VerifyAndClear(&handler);
}
