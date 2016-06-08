#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;
Factory::Factory() {
	initSpriteFrame();
}
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}
void Factory::initSpriteFrame() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
		monsterDead.pushBack(frame);
	}
}

Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364, 0, 42, 42)));
	monster.pushBack(mons);
	return mons;
}

void Factory::removeMonster(Sprite* sp) {

	cocos2d::Vector<Sprite*>::iterator it = monster.begin();
	for (; it != monster.end();) {

		if ((*it) == sp) {
			auto deadAnimation = Animation::createWithSpriteFrames(monsterDead, 0.1f);
			auto deadAnimate = Animate::create(deadAnimation);
			sp->runAction(Sequence::create(deadAnimate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL));
			monster.erase(it);
			return;
		}
		else {
			it++;
		}
	}
}

//让容器中的所有怪物都往角色移动，通过容器管理所有的怪物很方便
void Factory::moveMonster(Vec2 playerPos, float time) {

	for (auto eachMonster : monster) {

		Vec2 monPosition = eachMonster->getPosition();
		Vec2 directioin = playerPos - monPosition;
		directioin.normalize();
		eachMonster->runAction(MoveBy::create(time, directioin * 30));
	}
}

Sprite* Factory::collider(Rect rect) {

	for (auto eachMonster : monster) {

		if (rect.containsPoint(eachMonster->getPosition())) {

			return eachMonster;
		}
	}
	return NULL;
}