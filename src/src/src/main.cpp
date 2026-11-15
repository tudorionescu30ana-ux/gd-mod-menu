#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool on(char const* key) { return Mod::get()->getSavedValue<bool>(key, false); }
static float speed() { return Mod::get()->getSavedValue<float>("speed", 1.f); }

class ModMenuPopup : public Popup<> {
protected:
    bool setup() override {
        this->setTitle("Mod Menu");
        auto sz = m_mainLayer->getContentSize();

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);
        m_mainLayer->addChild(menu);

        addToggle(menu, "Noclip", "noclip", {70.f, sz.height - 55.f});
        addToggle(menu, "Speedhack", "speedhack", {70.f, sz.height - 90.f});

        auto slider = Slider::create(this, menu_selector(ModMenuPopup::onSpeed), 0.8f);
        slider->setPosition({sz.width / 2, 40.f});
        slider->setValue((speed() - 0.25f) / 1.75f);
        m_mainLayer->addChild(slider);

        m_label = CCLabelBMFont::create("", "bigFont.fnt");
        m_label->setScale(0.45f);
        m_label->setPosition({sz.width / 2, 65.f});
        m_mainLayer->addChild(m_label);
        updateLabel();
        return true;
    }

    void addToggle(CCMenu* menu, char const* text, char const* key, CCPoint pos) {
        auto t = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(ModMenuPopup::onToggle), 0.7f);
        t->setPosition(pos);
        t->toggle(on(key));
        t->setID(key);
        menu->addChild(t);

        auto l = CCLabelBMFont::create(text, "bigFont.fnt");
        l->setScale(0.55f);
        l->setAnchorPoint({0.f, 0.5f});
        l->setPosition({pos.x + 25.f, pos.y});
        m_mainLayer->addChild(l);
    }

    void onToggle(CCObject* s) {
        auto t = static_cast<CCMenuItemToggler*>(s);
        Mod::get()->setSavedValue<bool>(t->getID(), !t->isToggled());
    }

    void onSpeed(CCObject* s) {
        float v = static_cast<SliderThumb*>(s)->getValue();
        Mod::get()->setSavedValue<float>("speed", 0.25f + v * 1.75f);
        updateLabel();
    }

    void updateLabel() {
        m_label->setString(fmt::format("Viteza: {:.2f}x", speed()).c_str());
    }

    CCLabelBMFont* m_label = nullptr;

public:
    static ModMenuPopup* create() {
        auto ret = new ModMenuPopup();
        if (ret->initAnchored(260.f, 190.f)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        auto bottom = this->getChildByID("bottom-menu");
        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png"),
            this, menu_selector(MyMenuLayer::onModMenu));
        bottom->addChild(btn);
        bottom->updateLayout();
        return true;
    }
    void onModMenu(CCObject*) { ModMenuPopup::create()->show(); }
};

class $modify(MyPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* p, GameObject* g) override {
        if (on("noclip")) return;
        PlayLayer::destroyPlayer(p, g);
    }
    void update(float dt) override {
        PlayLayer::update(on("speedhack") ? dt * speed() : dt);
    }
};
