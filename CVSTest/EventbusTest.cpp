#include "pch.h"

#include "../CVSystem/CVSystem.h"


Dexode::EventBus bus;

namespace Event { // Example namespace for events
    struct Gold // Event that will be proceed when our gold changes
    {
        int value = 0;
    };
}

enum class Monster
{
    Frog,
    Tux,
    Shark
};

///-------------------------------------------------------------------------------------------------
/// <summary> 一个示例类. </summary>
///
/// <remarks> Dx, 2019/1/13. </remarks>
///-------------------------------------------------------------------------------------------------
class ShopButton
{
public:
    ShopButton(const std::shared_ptr<Dexode::EventBus>& eventBus)
        : _listener{ eventBus }
    {
        // We can use lambda or bind your choice
        _listener.listen<Event::Gold>(
            std::bind(&ShopButton::onGoldUpdated, this, std::placeholders::_1));
        // Also we use RAII idiom to handle unlisten
    }

    bool isEnabled() const
    {
        return _isEnabled;
    }

private:
    Dexode::EventCollector _listener;
    bool _isEnabled = false;

    void onGoldUpdated(const Event::Gold& event)
    {
        _isEnabled = event.value > 0;
        std::cout << "Shop button is:" << _isEnabled << std::endl; // some kind of logs
    }
};


bool isEnter;
int enterCount;

//内存事件的响应
void onMemEvent(const Event::Gold& e)
{
    isEnter = true;
    enterCount++;
}


TEST(Eventbus, base)
{
    isEnter = false;
    enterCount = 0;

    Dexode::EventCollector _listener(dxlib::Event::GetInst()->bus);

    //绑定了两次就会进入两次函数
    _listener.listen<Event::Gold>(onMemEvent);
    _listener.listen<Event::Gold>(onMemEvent);

    dxlib::Event::GetInst()->bus->notify(Event::Gold{ 123 });

    EXPECT_TRUE(isEnter);

    EXPECT_TRUE(enterCount == 2);
}

