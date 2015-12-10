#include "GLTestingLayer.h"

cocos2d::Scene* GLTestingLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = GLTestingLayer::create();
	scene->addChild(layer);
	return scene;
}

bool GLTestingLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	this->setPosition(0, 0);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto sprite = Sprite::create("HelloWorld.png");
	sprite->setAnchorPoint(Point(0.5, 0.5));
	sprite->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(sprite);
	graySprite(sprite);

	this->setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_POSITION_COLOR));

	return true;
}

void GLTestingLayer::menuCloseCallback(cocos2d::Ref* pSender)
{

}

void GLTestingLayer::graySprite(Sprite * sprite)
{
	if (sprite)
	{
		GLProgram * p = new GLProgram();
		p->initWithFilenames("gray.vsh", "gray.fsh");
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
		p->link();
		p->updateUniforms();
		sprite->setShaderProgram(p);
	}

}

void GLTestingLayer::visit(Renderer* renderer, const kmMat4 &parentTransform, bool parentTransformUpdated)
{
	Layer::visit(renderer, parentTransform, parentTransformUpdated);
	_command.init(_globalZOrder);
	_command.func = CC_CALLBACK_0(GLTestingLayer::onDraw, this);
	Director::getInstance()->getRenderer()->addCommand(&_command);
}

void GLTestingLayer::onDraw()
{
	auto glProgram = this->getShaderProgram();
	glProgram->use();
	glProgram->setUniformsForBuiltins();
	glPointSize(10.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	auto size = Director::getInstance()->getWinSize();

	float vertercies[] = { 100, 100,
		200, 200,
		300, 100 };
	float color[] = { 1, 0, 0, 1,
		0, 1, 0, 1,
		0, 0, 1, 1 };
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertercies);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_TRUE, 0, color);
	// 绘制三角形  
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//通知cocos2d-x 的renderer，让它在合适的时候调用这些OpenGL命令  
	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 3);
	CHECK_GL_ERROR_DEBUG();

}

GLTestingLayer.cpp