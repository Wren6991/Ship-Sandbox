#include <GameLib/GameEventDispatcher.h>

#include "gmock/gmock.h"

class _MockHandler : public IGameEventHandler
{
public:
    
    MOCK_METHOD3(OnDestroy, void(Material const * material, bool isUnderwater, unsigned int size));
    MOCK_METHOD0(OnDraw, void());
    MOCK_METHOD3(OnBreak, void(Material const * material, bool isUnderwater, unsigned int size));
    MOCK_METHOD3(OnStress, void(Material const * material, bool isUnderwater, unsigned int size));
    MOCK_METHOD1(OnSinkingBegin, void(unsigned int shipId));
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

    EXPECT_CALL(handler, OnDestroy(_, _, _)).Times(0);

    dispatcher.OnDestroy(pm1, true, 3);
    dispatcher.OnDestroy(pm1, true, 2);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnDestroy(pm1, true, 5)).Times(1);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnDestroy_MultipleKeys)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);
    Material * pm2 = reinterpret_cast<Material *>(21);

    EXPECT_CALL(handler, OnDestroy(_, _, _)).Times(0);

    dispatcher.OnDestroy(pm2, false, 1);
    dispatcher.OnDestroy(pm1, false, 3);
    dispatcher.OnDestroy(pm2, false, 2);
    dispatcher.OnDestroy(pm1, false, 9);
    dispatcher.OnDestroy(pm1, false, 1);
    dispatcher.OnDestroy(pm2, true, 2);
    dispatcher.OnDestroy(pm2, true, 2);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnDestroy(pm1, false, 13)).Times(1);
    EXPECT_CALL(handler, OnDestroy(pm2, false, 3)).Times(1);
    EXPECT_CALL(handler, OnDestroy(pm2, true, 4)).Times(1);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnBreak)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);

    EXPECT_CALL(handler, OnBreak(_, _, _)).Times(0);

    dispatcher.OnBreak(pm1, false, 3);
    dispatcher.OnBreak(pm1, false, 2);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnBreak(pm1, false, 5)).Times(1);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnBreak_MultipleKeys)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);
    Material * pm2 = reinterpret_cast<Material *>(21);

    EXPECT_CALL(handler, OnBreak(_, _, _)).Times(0);

    dispatcher.OnBreak(pm2, false, 1);
    dispatcher.OnBreak(pm1, false, 3);
    dispatcher.OnBreak(pm2, false, 2);
    dispatcher.OnBreak(pm2, true, 2);
    dispatcher.OnBreak(pm1, false, 9);
    dispatcher.OnBreak(pm1, false, 1);
    dispatcher.OnBreak(pm2, true, 2);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnBreak(pm1, false, 13)).Times(1);
    EXPECT_CALL(handler, OnBreak(pm2, false, 3)).Times(1);
    EXPECT_CALL(handler, OnBreak(pm2, true, 4)).Times(1);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnSinkingBegin)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    EXPECT_CALL(handler, OnSinkingBegin(_)).Times(0);

    dispatcher.OnSinkingBegin(7);
    dispatcher.OnSinkingBegin(7);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnSinkingBegin(7)).Times(1);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, Aggregates_OnSinkingBegin_MultipleShips)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    EXPECT_CALL(handler, OnSinkingBegin(_)).Times(0);

    dispatcher.OnSinkingBegin(7);
    dispatcher.OnSinkingBegin(3);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnSinkingBegin(3)).Times(1);
    EXPECT_CALL(handler, OnSinkingBegin(7)).Times(1);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);
}

TEST(GameEventDispatcherTests, ClearsStateAtUpdate)
{
    MockHandler handler;

    GameEventDispatcher dispatcher;
    dispatcher.RegisterSink(&handler);

    Material * pm1 = reinterpret_cast<Material *>(7);

    EXPECT_CALL(handler, OnDestroy(_, _, _)).Times(0);

    dispatcher.OnDestroy(pm1, false, 3);
    dispatcher.OnDestroy(pm1, false, 2);

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnDestroy(pm1, false, 5)).Times(1);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);

    EXPECT_CALL(handler, OnDestroy(_, _, _)).Times(0);

    dispatcher.Flush();

    Mock::VerifyAndClear(&handler);
}
