#include "pch.h"

#include "../CVSystem/eventbus/EventBus.h"
#include "../CVSystem/eventbus/EventCollector.h"

std::shared_ptr<Dexode::EventBus> bus = std::make_shared<Dexode::EventBus>();

namespace Event {
    struct Event1
    {
        int value = 0;
    };

    struct Event2
    {
        double value = 0;
    };
}

bool isEnter;
int enterCount;

void onEvent1(const Event::Event1& e)
{
    std::cout << "enter envent 1 " << std::endl;
    isEnter = true;
    enterCount++;
}
void onEvent2(const Event::Event2& e)
{
    std::cout << "enter envent 2 " << std::endl;
    isEnter = true;
    enterCount++;
}


TEST(Eventbus, Event1_Event2)
{
    //reset flag
    isEnter = false;
    enterCount = 0;

    Dexode::EventCollector _listener(bus);

    _listener.listen<Event::Event1>(std::bind(&onEvent1, std::placeholders::_1));
    _listener.listen<Event::Event2>(std::bind(&onEvent2, std::placeholders::_1));

    bus->notify(Event::Event1{ 1 });

    EXPECT_TRUE(isEnter);
    EXPECT_TRUE(enterCount == 1);//fail！ enterCount=2
}

