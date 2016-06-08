#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Global.h"
#include "GameScene.h"
#include <regex>
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;
USING_NS_CC;

using namespace cocostudio::timeline;

#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace  rapidjson;

Scene* LoginScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = LoginScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


// on "init" you need to initialize your instance
bool LoginScene::init()
{
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size size = Director::getInstance()->getVisibleSize();
	visibleHeight = size.height;
	visibleWidth = size.width;

	textField = TextField::create("Player Name", "Arial", 30);
	textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
	this->addChild(textField, 2);

	auto button = Button::create();
	button->setTitleText("Login");
	button->setTitleFontSize(30);
	button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
	this->addChild(button, 2);

	button->addClickEventListener(CC_CALLBACK_1(LoginScene::buttonClick, this));

	return true;
}

void LoginScene::buttonClick(Ref *pSender) {
	HttpRequest* request = new HttpRequest();
	request->setUrl("http://localhost:8080/login");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(LoginScene::onHttpRequestCompleted, this));

	string username = "username=" + textField->getStringValue();
	const char* postData = username.c_str();
	request->setRequestData(postData, strlen(postData));
	request->setTag("POST");
	
	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	request->setHeaders(headers);

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void LoginScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
	if (!response) return;
	if (!response->isSucceed()) {
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseHeader();
	log("Http Test, dump data: ");
	for (int i = 0; i < buffer->size(); i++) {
		CCLog("%c", (*buffer)[i]);
	}
	string sessionid = Global::toString(buffer);
	Global::gameSessionId = Global::getSessionIdFromHeader(sessionid);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, GameScene::createScene()));

}
