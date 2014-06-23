#pragma once

#ifndef GAMEHUD_H
#define GAMEHUD_H

#ifndef INCLUDES_H
#include "includes.h"
#endif // includes_h
#ifndef EVENT_H
#include "event.h"
#endif

namespace States{
	// enumeration of button states
	enum ButtonState { USUAL = -1, HIGHLIGHTED, PRESSED};
}

#ifndef TOVEC_
#define TOVEC_
// convert sf::Vector2<int> to sf::Vector2<float>
sf::Vector2f toVector2f(const sf::Vector2i& t){
	return sf::Vector2f(t.x,t.y);
}

#endif TOVEC_

namespace Styles{
	enum Align{
		NOALIGN, LEFT, CENTER, RIGHT
	};
	
	enum Size{
		NOSIZE, TINY, SMALL, NORMAL, BIG, HUGE, ENORMOUS
	};
	namespace Colors{
		static sf::Color RED(255,0,0,255);
		static sf::Color GREEN(0,255,0,255);
		static sf::Color BLUE(0,0,255,255);
		static sf::Color WHITE(255,255,255,255);
		static sf::Color WHITE_GHOST(248,248,255,255);
		static sf::Color WHITE_SNOW(255,250,250,255);
		static sf::Color BLACK(0,0,0,255);
		static sf::Color YELLOW(255,255,0,255);
		static sf::Color MAGENTA(255,0,255,255);
		static sf::Color CYAN(0,255,255,255);
		static sf::Color GRAY(128,128,128,255);
		static sf::Color SILVER(192,192,192,255);
		static sf::Color GREEN_DARK(0,128,0,255);
		static sf::Color RED_DARK(128,0,0,255);
		static sf::Color BLUE_DARK(0,0,128,255);
		static sf::Color ORANGE(255,165,0,255);
		static sf::Color GOLD(255,215,0,255);
		static sf::Color GOLDEN_ROD(218,165,32,255);
		static sf::Color GREEN_LIME(50,205,50,255);
		static sf::Color BLUE_MIDNIGHT(25,25,112,255);
		static sf::Color BLUE_VIOLET(138,43,226,255);
		static sf::Color BLUE_INDIGO(75,0,130,255);
		static sf::Color VIOLET_DARK(148,0,211,255);
		static sf::Color PINK_DEEP(255,20,147,255);
		static sf::Color VIOLET_RED(199,21,133,255);
		static sf::Color BLUE_STATE(106,90,205,255);
		static sf::Color BROWN(139,69,19,255);
		static sf::Color GRAY_STATE(112,128,144,255);
		static sf::Color GRAY_LIGHT(144,156,198,255);
		static sf::Color KHAKI(240,230,140,255);
	};
	// main game fonts
	class Fonts{
	public:
		static sf::Font HUDFONT;
	};
}

// text panel
class InfoBar : public sf::FloatRect, public sf::Drawable, public sf::Transformable{
public:
	InfoBar() :Rect(){
		_background = 0;
	}
	InfoBar(const sf::Vector2f& Pos, const sf::Vector2f& Size) : Rect(Pos, Size){
		_background = 0;
	}
	InfoBar(float x, float y, float w, float h) : Rect(x,y,w,h){
		_background = 0;
	}
	InfoBar(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Sprite &bg) : Rect(pos, size){
		_background = new sf::Sprite(bg);
	}
	InfoBar(float x, float y, float w, float h, const sf::Sprite &bg) : Rect(x,y,w,h){
		_background = new sf::Sprite(bg);
	}

	void Move(float x, float y){
		left += x;
		top += y;
		for(int i=0; i<_text_inside.size(); i++)
			_text_inside[i]->move(x,y);
		if(_background) _background->move(x,y);
	}
	void Setpos(float x, float y){
		Move(x - left, y - top);
	}

	void Draw(sf::RenderWindow &window){
		if(_background) window.draw(*_background);
		for(unsigned i=0; i<_text_inside.size(); i++)
			window.draw(*_text_inside[i]);
	}

	void SetBg(const sf::Sprite& spr){
		if(_background) delete _background;

		_background = new sf::Sprite(spr);
		sf::FloatRect b = _background->getGlobalBounds();
		_background->scale(width/b.width, height/b.height);
	}
	// str is for placement textline in a correct order 
	void AddLabel(sf::Text* t, Styles::Align al, Styles::Size sz, unsigned str = 1){
		using namespace Styles;
		t->setScale(1,1);
		switch(sz){
		case TINY:	t->setCharacterSize(5); break;
		case SMALL:	t->setCharacterSize(10); break;
		case NORMAL: t->setCharacterSize(20); break;
		case BIG: t->setCharacterSize(40); break;
		case Size::HUGE:	t->setCharacterSize(60); break;
		case ENORMOUS:	t->setCharacterSize(80);
		}
		switch(al){
		case LEFT:{
			int s = (int) height / t->getLocalBounds().height;
			t->setPosition(left+(left/20), top+(s*str));
			break;
				  }
		case RIGHT:{
			float s = height / t->getLocalBounds().height;
			t->setPosition(left+(width-left/20), top+(s*str));
			break;
				   }
		case CENTER:{
			float s = height / t->getLocalBounds().height;
			t->setPosition(left+(width/2-t->getLocalBounds().width/2), top+(s*str));
					}
		}
		_text_inside.push_back(t);
	}

	~InfoBar(){
		if(_background) delete _background;
	}
private:
	std::vector<sf::Text*> _text_inside;
	std::vector<sf::Sprite*> _icons_inside;
	sf::Sprite* _background;
};

// makes any derived class - pressable
__interface Pressable{
public:
	virtual void Press() = 0;
	virtual void Highlight() =0;
	virtual bool Release(float x, float y) = 0;
	// mouse hover - return 0
	// mouse away - return -1
	// mouse hover + pressed - return 1
	// may not be correct ^
	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& relative, const sf::Event& evnt) = 0;
	// fulscreen version of ProcessMouse()
	virtual int ProcessMouse(const sf::Mouse& m, const sf::Event& evnt) = 0;
	virtual States::ButtonState GetState()const = 0;
};

//======================================================= abstract button base classes/ template specialization
template <class Ret, class Arg>
// button abstract class
class _Button_base : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<Ret, Arg>& evnt): sf::FloatRect(rect), _myevent(evnt){
		_binder = BindEvent(evnt.GetEvent(), evnt.GetObjectPtr(), Arg(0));
	}

	virtual void Execute(){
		_binder.Exec();
	}
	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}

	virtual void Bind(void (Ret::*evn)(const Arg&), const Arg& arg){
		_myevent = evn;
		_binder.Bind(evn, arg);
	}
	virtual void Bind(const Arg& arg){
		_binder.Bind(arg);
	}
protected:
	Event<Ret, Arg> _myevent;
	EBinder<Ret, Arg> _binder;
};

template <class Ret>
class _Button_base<Ret, void> : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<Ret, void>& evnt): sf::Rect<float>(rect), _myevent(evnt), _binder(evnt.GetEvent()){}

	virtual Ret Execute(){
		return _binder.Exec();
	}

	virtual void Bind(Ret (*evn)()){
		_myevent = evn;
		_binder.Bind(evn);
	}
	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}
protected:
	Event<Ret, void> _myevent;
	EBinder<Ret, void> _binder;
};

template <class Arg>
class _Button_base<void,Arg> : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<void, Arg>& evnt): sf::Rect<float>(rect), _myevent(evnt){
		_binder = BindEvent(evnt.GetEvent(), Arg(0));
	}

	virtual void Execute(){
		_binder.Exec();
	}
	virtual void Bind(void (*evn)(const Arg&), const Arg& arg){
		_myevent = evn;
		_binder.Bind(evn, arg);
	}
	virtual void Bind(const Arg& arg){
		_binder.Bind(arg);
	}
	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}
protected:
	Event<void, Arg> _myevent;
	EBinder<void, Arg> _binder;
};

template <>
class _Button_base<void, void> : public sf::FloatRect, public sf::Transformable, public sf::Drawable, public Pressable{
public:
	_Button_base(){}
	_Button_base(const sf::FloatRect& rect, const Event<void,void>& evnt): sf::Rect<float>(rect), _myevent(evnt), _binder(evnt.GetEvent()){}

	virtual void Execute(){
		_binder.Exec();
	}
	virtual void Bind(void (*evn)()){
		_myevent = evn;
		_binder.Bind(evn);
	}

	sf::FloatRect Rect()const{
		return sf::FloatRect(left, top, width, height);
	}
protected:
	Event<void,void> _myevent;
	EBinder<void, void> _binder;
};

////////////////////////==========================================	real button classes	==

template <class Object, class EventArg>
class Button: public _Button_base<Object, EventArg>{
public:
	typedef typename EventArg _arg;
	typedef typename Object _obj;

	Button(){
		_usual = _pressed = _hited = 0;
	}
	Button(const sf::FloatRect& posit, sf::Sprite* face, sf::Sprite* hl, sf::Sprite* pressed,const Event<_obj, _arg>& evnt, const _arg& arg)
		: _Button_base<_obj,_arg>(posit, evnt)
	{
		_binder.Bind(arg);
		_usual = _pressed = _hited = 0;
		_scopy(_usual, face);
		_usual->setPosition(posit.left, posit.top);
		_scopy(_pressed, pressed);
		_pressed->setPosition(posit.left, posit.top);
		_scopy(_hited, hl);
		_hited->setPosition(posit.left, posit.top);
		_cur(_usual);
	}
	Button(const Button& t) : _Button_base<_obj, _arg>(t.Rect(),t._myevent){
		_binder = t._binder;
		_usual = _pressed = _hited = 0;
		_scopy(_usual, t._usual);
		_scopy(_pressed, t._pressed);
		_scopy(_hited, t._hited);
		_cur(_usual);
	}
	virtual void Press(){
		_cur(_pressed);
	}
	virtual void Highlight(){
		_cur(_hited);
	}

	virtual bool Release(float x, float y){
		if(_Button_base<_obj,_arg>::contains(x,y)){
			_cur(_hited);
			return true;
		}
		else{
			_cur(_usual);
			return false;
		}
	}

	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& relative, const sf::Event& evnt){
		sf::Vector2f mv = toVector2f(mouse.getPosition(relative));
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				_binder.Exec();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<_obj, _arg>::contains(mv)){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<_obj, _arg>::contains(mv)) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}
	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Event& evnt){
		sf::Vector2f mv = toVector2f(mouse.getPosition());
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				Execute();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<_obj, _arg>::contains(mv)){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<_obj, _arg>::contains(mv)) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}
	virtual States::ButtonState GetState()const{
		using namespace States;
		return (_current == _usual? USUAL : (_current == _pressed ? PRESSED : HIGHLIGHTED) );
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates state)const{
		target.draw(*_current, state);
	}

	~Button(){
		if(_usual) delete _usual;
		if(_pressed) delete _pressed;
		if(_hited) delete _hited;
	}
private:
	void _cur(sf::Sprite* spr){
		_current = spr;
	}
	void _scopy(sf::Sprite* &dest, const sf::Sprite* src){
		if(dest) delete dest;
		if(src) dest = new sf::Sprite(*src);
	}
	sf::Sprite *_current;
	sf::Sprite *_usual;
	sf::Sprite *_pressed;
	sf::Sprite *_hited;
};



template <class EventArg>
class Button<void, EventArg>: public _Button_base<void, EventArg>{
public:
	typedef typename EventArg _arg;

	Button(){
		_usual = _pressed = _hited = 0;
	}
	Button(const sf::FloatRect& posit, sf::Sprite* face, sf::Sprite* hl, sf::Sprite* pressed,const Event<void, _arg>& evnt, const _arg& arg)
		: _Button_base<void,_arg>(posit, evnt)
	{
		_binder.Bind(arg);
		_usual = _pressed = _hited = 0;
		_scopy(_usual, face);
		_usual->setPosition(posit.left, posit.top);
		_scopy(_pressed, pressed);
		_pressed->setPosition(posit.left, posit.top);
		_scopy(_hited, hl);
		_hited->setPosition(posit.left, posit.top);
		_cur(_usual);
	}
	Button(const Button& t) : _Button_base<void, _arg>(t.Rect(),t._myevent){
		_binder = t._binder;
		_usual = _pressed = _hited = 0;
		_scopy(_usual, t._usual);
		_scopy(_pressed, t._pressed);
		_scopy(_hited, t._hited);
		_cur(t._current);
	}
	virtual void Press(){
		_cur(_pressed);
	}
	virtual void Highlight(){
		_cur(_hited);
	}

	virtual bool Release(float x, float y){
		if(_Button_base<void,_arg>::contains(x,y)){
			_cur(_hited);
			return true;
		}
		else{
			_cur(_usual);
			return false;
		}
	}

	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& relative, const sf::Event& evnt){
		sf::Vector2f mv = toVector2f(mouse.getPosition(relative));
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				_binder.Exec();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, _arg>::contains(mv)){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, _arg>::contains(mv)) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}
	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Event& evnt){
		sf::Vector2f mv = toVector2f(mouse.getPosition());
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				Execute();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, _arg>::contains(mv)){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, _arg>::contains(mv)) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}
	virtual States::ButtonState GetState()const{
		using namespace States;
		return (_current == _usual? USUAL : (_current == _pressed ? PRESSED : HIGHLIGHTED) );
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates state)const{
		target.draw(*_current, state);
	}

	~Button(){
		if(_usual) delete _usual;
		if(_pressed) delete _pressed;
		if(_hited) delete _hited;
	}
private:
	void _cur(sf::Sprite* spr){
		_current = spr;
	}
	void _scopy(sf::Sprite* &dest, const sf::Sprite* src){
		if(dest) delete dest;
		if(src) dest = new sf::Sprite(*src);
	}
	sf::Sprite *_current;
	sf::Sprite *_usual;
	sf::Sprite *_pressed;
	sf::Sprite *_hited;
};

// void button specialization
template <>
class Button<void, void> : _Button_base<void, void>{
public: Button(const sf::FloatRect& posit, sf::Sprite* face, sf::Sprite* hl, sf::Sprite* pressed,const Event<void, void>& evnt):
	_Button_base(posit,evnt)
	{
		_usual = _pressed = _hited = 0;
		_scopy(_usual, face);
		_usual->setPosition(posit.left, posit.top);
		_scopy(_pressed, pressed);
		_pressed->setPosition(posit.left, posit.top);
		_scopy(_hited, hl);
		_hited->setPosition(posit.left, posit.top);
		_cur(_usual);
	}
	Button(const Button& t) : _Button_base<void, void>(t.Rect(),t._myevent){
		_usual = _pressed = _hited = 0;
		_scopy(_usual, t._usual);
		_scopy(_pressed, t._pressed);
		_scopy(_hited, t._hited);
		_cur(t._current);
	}
	virtual void Press(){
		_cur(_pressed);
	}
	virtual void Highlight(){
		_cur(_hited);
	}

	virtual bool Release(float x, float y){
		if(_Button_base<void,void>::contains(x,y)){
			_cur(_hited);
			return true;
		}
		else{
			_cur(_usual);
			return false;
		}
	}

	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& relative, const sf::Event& evnt){
		sf::Vector2f mv = toVector2f(mouse.getPosition(relative));
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				_binder.Exec();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, void>::contains(mv)){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, void>::contains(mv)) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}
	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Event& evnt){
		sf::Vector2f mv = toVector2f(mouse.getPosition());
		if(GetState() == States::PRESSED){
			if(evnt.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				Execute();
		}
		if(GetState() == States::HIGHLIGHTED && !_Button_base<void, void>::contains(mv)){
			_cur(_usual);
			return -1;
		}
		if(_Button_base<void, void>::contains(mv)) {
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(GetState() == States::HIGHLIGHTED) _cur(_pressed);
				return 1;
			}
			_cur(_hited);
			return 0;
		}
		else return -1;

	}

	virtual States::ButtonState GetState()const{
		using namespace States;
		return (_current == _usual? USUAL : (_current == _pressed ? PRESSED : HIGHLIGHTED) );
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates state)const{
		target.draw(*_current, state);
	}

	~Button(){
		if(_usual) delete _usual;
		if(_pressed) delete _pressed;
		if(_hited) delete _hited;
	}
private:
	void _cur(sf::Sprite* spr){
		_current = spr;
	}
	void _scopy(sf::Sprite* &dest, const sf::Sprite* src){
		if(dest) delete dest;
		if(src) dest = new sf::Sprite(*src);
	}
	sf::Sprite *_current;
	sf::Sprite *_usual;
	sf::Sprite *_pressed;
	sf::Sprite *_hited;
};

////////////////////////==========================================	pressable icons		==


template <class EventArg>
class IconButton : public _Button_base<void, EventArg>{
public:
	typedef typename EventArg _arg;
	typedef IconButton<_arg> _myt;
	IconButton(){}
	IconButton(const sf::FloatRect& rect, const sf::Sprite* spr, const Event<void, _arg> &evn , const _arg& arg) :
		_Button_base<void, _arg>(rect, evn), _state(States::USUAL){
		_binder.Bind(arg);
		_ico = new sf::Sprite(*spr);
		_ico->setPosition(left, top);
	}
	// this version of ctor makes an IconButton from texture
	// where trect - texture rectangle
	// pos - rectangle of a button, placed anywhere on the screen
	IconButton(const sf::FloatRect& pos, const sf::IntRect &trect, const sf::Texture& tex,const Event<void, _arg> &evn , const _arg& arg):
		_Button_base<void, _arg>(pos, evn){
		_ico = new sf::Sprite(tex, trect);
		_binder.Bind(arg);

	}
	IconButton(const IconButton<_arg>& t): _Button_base<void, _arg>(sf::FloatRect(t.left,t.top,t.width,t.height),t._myevent){
		_state = t.GetState();
		_binder = t._binder;
		if(t._ico) _ico = new sf::Sprite (*t._ico);
	}
	virtual void Press(){
		_state = States::PRESSED;
		_ico->setColor(Styles::Colors::GRAY);
	}
	virtual void Highlight(){
		_state = States::HIGHLIGHTED;
		_ico->setColor(Styles::Colors::GRAY_LIGHT);
	}
	virtual bool Release(float x, float y){
		if(contains(x,y) && GetState() == States::PRESSED){
			_state = States::HIGHLIGHTED;
			_ico->setColor(Styles::Colors::GRAY_LIGHT);
			return true;
		}
		else _ico->setColor(Styles::Colors::WHITE);
		_state = States::USUAL;
	return false;
	}
	virtual int ProcessMouse(const sf::Mouse& m, const sf::Window& win, const sf::Event& e){
		sf::Vector2f mv = toVector2f(m.getPosition(win));
		if(_state == States::PRESSED){
			if(e.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				_binder.Exec();
		}
		if(_state == States::HIGHLIGHTED && !_Button_base<void, _arg>::contains(mv)){
			_ico->setColor(Styles::Colors::WHITE);
			return -1;
		}
		if(_Button_base<void, _arg>::contains(mv)) {
			if(m.isButtonPressed(sf::Mouse::Button::Left)){
				if(_state == States::HIGHLIGHTED) Press();
				return 1;
			}
			Highlight();
			return 0;
		}
		else return -1;
	}
	virtual int ProcessMouse(const sf::Mouse& m, const sf::Event& e){
		sf::Vector2f mv = toVector2f(m.getPosition());
		if(_state == States::PRESSED){
			if(e.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y))
				_binder.Exec();
		}
		if(_state == States::HIGHLIGHTED && !_Button_base<void, _arg>::contains(mv)){
			_ico->setColor(Styles::Colors::WHITE);
			return -1;
		}
		if(_Button_base<void, _arg>::contains(mv)) {
			if(m.isButtonPressed(sf::Mouse::Button::Left)){
				if(_state == States::HIGHLIGHTED) Press();
				return 1;
			}
			Highlight();
			return 0;
		}
		else return -1;
	}


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
		target.draw(*_ico,states);
	}

	States::ButtonState GetState()const{
		return _state;
	}

	~IconButton(){
		if(_ico) delete _ico;
	}
private:
	States::ButtonState _state;
	sf::Sprite* _ico;
};

template <>
class IconButton<void> : public _Button_base<void,void>{
public:
	typedef IconButton<void> _myt;
	IconButton(){}
	IconButton(const sf::FloatRect& rect, const sf::Sprite* spr, const Event<void, void> &evn) :
		_Button_base<void,void>(rect, evn), _state(States::USUAL){
		_ico = new sf::Sprite(*spr);
		_ico->setPosition(left, top);
	}
	// this version of ctor makes an IconButton from texture
	// where trect - texture rectangle
	// pos - rectangle of a button, placed anywhere on the screen
	IconButton(const sf::FloatRect& pos, const sf::IntRect &trect, const sf::Texture& tex,const Event<void,void> &evn):
		_Button_base<void,void>(pos, evn){
		_ico = new sf::Sprite(tex, trect);

	}
	IconButton(const _myt& t): _Button_base<void, void>(sf::FloatRect(t.left,t.top,t.width,t.height),t._myevent){
		_binder = t._binder;
		if(t._ico) _ico = new sf::Sprite (*t._ico);
	}
	virtual void Press(){
		_state = States::PRESSED;
		_ico->setColor(Styles::Colors::GRAY);
	}
	virtual void Highlight(){
		_state = States::HIGHLIGHTED;
		_ico->setColor(Styles::Colors::GRAY_LIGHT);
	}
	virtual bool Release(float x, float y){
		if(contains(x,y) && GetState() == States::PRESSED){
			_state = States::HIGHLIGHTED;
			_ico->setColor(Styles::Colors::GRAY_LIGHT);
			return true;
		}
		else _ico->setColor(Styles::Colors::WHITE);
		_state = States::USUAL;
	return false;
	}
	virtual int ProcessMouse(const sf::Mouse& m, const sf::Window& win, const sf::Event& e){
		if(_state == States::PRESSED){
			if(e.type == sf::Event::MouseButtonReleased && Release(m.getPosition(win).x, m.getPosition(win).y))
				_binder.Exec();
		}
		if(_state == States::HIGHLIGHTED && !_Button_base<void,void>::contains(toVector2f(m.getPosition(win)))){
			_ico->setColor(Styles::Colors::WHITE);
			return -1;
		}
		if(_Button_base<void,void>::contains(toVector2f(m.getPosition(win)))) {
			if(m.isButtonPressed(sf::Mouse::Button::Left)){
				if(_state == States::HIGHLIGHTED) Press();
				return 1;
			}
			Highlight();
			return 0;
		}
		else return -1;
	}
	virtual int ProcessMouse(const sf::Mouse& m, const sf::Event& e){
		if(_state == States::PRESSED){
			if(e.type == sf::Event::MouseButtonReleased && Release(m.getPosition().x, m.getPosition().y))
				_binder.Exec();
		}
		if(_state == States::HIGHLIGHTED && !_Button_base<void,void>::contains(toVector2f(m.getPosition()))){
			_ico->setColor(Styles::Colors::WHITE);
			return -1;
		}
		if(_Button_base<void,void>::contains(toVector2f(m.getPosition()))) {
			if(m.isButtonPressed(sf::Mouse::Button::Left)){
				if(_state == States::HIGHLIGHTED) Press();
				return 1;
			}
			Highlight();
			return 0;
		}
		else return -1;
	}


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
		target.draw(*_ico,states);
	}

	States::ButtonState GetState()const{
		return _state;
	}

	~IconButton(){
		if(_ico) delete _ico;
	}
private:
	States::ButtonState _state;
	sf::Sprite* _ico;

};

/////////////////////======================================================	movable elements classes	==

class MovableElement: public Pressable, public sf::Drawable{
public:
	MovableElement(){}
	MovableElement(const sf::FloatRect& rect, const sf::Sprite* sprite) : _rect(rect), _state(States::USUAL){
		_ico = new sf::Sprite(*sprite);
		_lastPosition.x = rect.left;
		_lastPosition.y = rect.top;
	}
	virtual void Press(){
		_state = States::PRESSED;
		_ico->setColor(Styles::Colors::GRAY);
	}
	virtual void Highlight(){
		_state = States::HIGHLIGHTED;
		_ico->setColor(Styles::Colors::GRAY_LIGHT);
	}
	virtual bool Release(float x, float y){
		if(_rect.contains(x,y) && GetState() == States::PRESSED){
			_ico->setColor(Styles::Colors::GRAY_LIGHT);
			_state = States::HIGHLIGHTED;
			return true;
		}
		else _ico->setColor(Styles::Colors::WHITE);
		_state = States::USUAL;
	return false;
	}

	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Window& win, const sf::Event& e){
		sf::Vector2f mv = toVector2f(mouse.getPosition(win));
		if(_state == States::PRESSED){
			_ico->move(_mouseOffset);
			_mouseOffset = mv - _lastPosition;
			_lastPosition = mv;
			if(e.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y)){
				Highlight();
			}
		}
		if(_state == States::HIGHLIGHTED && !_ico->getGlobalBounds().contains(mv)){
			_ico->setColor(Styles::Colors::WHITE);
			return -1;
		}
		if(_ico->getGlobalBounds().contains(mv)){
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(_state == States::HIGHLIGHTED){
					_lastPosition = _ico->getPosition();
					_mouseOffset = mv - _lastPosition;
					Press();
				}
				return 1;
			}
			Highlight();
			return 0;
		}
		else return -1;
	}
	virtual int ProcessMouse(const sf::Mouse& mouse, const sf::Event& e){
		sf::Vector2f mv = toVector2f(mouse.getPosition());
		if(_state == States::PRESSED){
			_mouseOffset = mv - _lastPosition;
			_lastPosition = _ico->getPosition();
			_ico->move(_mouseOffset);
			if(e.type == sf::Event::MouseButtonReleased && Release(mv.x, mv.y)){
				Highlight();
			}
		}
		if(_state == States::HIGHLIGHTED && !_ico->getGlobalBounds().contains(mv)){
			_ico->setColor(Styles::Colors::WHITE);
			return -1;
		}
		if(_ico->getGlobalBounds().contains(mv)){
			if(mouse.isButtonPressed(sf::Mouse::Button::Left)){
				if(_state == States::HIGHLIGHTED){
					_lastPosition = _ico->getPosition();
					_mouseOffset = mv - _lastPosition;
					Press();
				}
				return 1;
			}
			Highlight();
			return 0;
		}
		else return -1;
	}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
		target.draw(*_ico,states);
	}

	States::ButtonState GetState()const{
		return _state;
	}

private:
	States::ButtonState _state;
	sf::Vector2f _lastPosition;
	sf::Vector2f _mouseOffset;
	sf::Sprite* _ico;
	sf::FloatRect _rect;
};

// void* universal storage
class Storage{
public:
	Storage(){}
	template <class Type>
	Type* Push(const Type& what){
		Type* p = new Type(what);
		_cont.push_back((void*)p);
		return p;
	}
	void Push(void* ptr){
		_cont.push_back(ptr);
	}
	void Clear(){
		for(unsigned i=0; i<_cont.size(); i++)
			delete _cont[i];
	}

	template <class Type>
	Type* Get(int x)const{
		return (Type*)(_cont[x]);
	}
	void Remove(int x){
		delete _cont[x];
		_cont.erase(_cont.begin()+x);
	}
	unsigned Size()const{
		return _cont.size();
	}
	~Storage(){
		for(unsigned i=0; i<_cont.size(); i++)
			delete _cont[i];
	}
private:
	std::vector<void*> _cont;
};

// HUD (heads-up display). user interface
class HUD : public sf::Rect<float>, public sf::Drawable{
public:
	HUD(sf::RenderWindow& windw){
	}
	HUD(){}

	template <class Obj, class Type>
	void AddButton(Button<Obj, Type> &btn){
		Button<Obj, Type>* p = _mystor.Push<Button<Obj, Type>>(btn);
		_elems.push_back(dynamic_cast<Pressable*>(p));
		_drelems.push_back(dynamic_cast<sf::Drawable*>(p));
	}
	// adds new button to the HUD's buttons list
	template<class Type>
	void AddButton(Button<void, Type> &butn){
		Button<void, Type>* p = _mystor.Push<Button<void, Type>>(butn);
		_elems.push_back(dynamic_cast<Pressable*>(p));
		_drelems.push_back(dynamic_cast<sf::Drawable*>(p));
	}
	// adds new button to the HUD's buttons list 
	template<class Type>
	void AddButton(Button<void, Type> *butn){
		_mystor.Push((void*)butn);
		_elems.push_back(dynamic_cast<Pressable*>(butn));
		_drelems.push_back(dynamic_cast<sf::Drawable*>(butn));
	}
	// adds new button to the HUD's buttons list
	template<class Type>
	void AddButton(IconButton<Type> &butn){
		IconButton<Type>* p = _mystor.Push<IconButton<Type>>(butn);
		_elems.push_back(dynamic_cast<Pressable*>(p));
		_drelems.push_back(dynamic_cast<sf::Drawable*>(p));
	}
	// adds new button to the HUD's buttons list
	template <class Type>
	void AddButton(IconButton<Type> *butn){
		_mystor.Push((void*)butn);
		_elems.push_back(dynamic_cast<Pressable*>(butn));
		_drelems.push_back(dynamic_cast<sf::Drawable*>(butn));
	}
	// adds a new movable item to the list
	void AddElement(MovableElement &element){
		MovableElement *p = _mystor.Push<MovableElement>(element);
		_elems.push_back(dynamic_cast<Pressable*>(p));
		_drelems.push_back(dynamic_cast<sf::Drawable*>(p));
	}
	// adds new button to the HUD's buttons list
	void AddElement(const sf::Text &text){
		sf::Text* my = _mystor.Push<sf::Text>(text);
		_drelems.push_back(dynamic_cast<sf::Drawable*>(my));
	}
	// adds a new element to the hud
	void AddElement(sf::Text *text){
		_mystor.Push((void*)text);
		_drelems.push_back(dynamic_cast<sf::Drawable*>(text));
	}
	// processes all buttons
	void Process(const sf::Mouse& mouse, const sf::Window& window, const sf::Event& e){
	for(std::vector<Pressable*>::iterator i= _elems.begin(); i!= _elems.end(); i++)
			(*i)->ProcessMouse(mouse, window, e);
	}
	// processes all buttons
	void Process(const sf::Mouse& mouse, const sf::Event& e){
		for(std::vector<Pressable*>::iterator i= _elems.begin(); i!= _elems.end(); i++)
			(*i)->ProcessMouse(mouse, e);
	}
	// draw all components
	void Draw(sf::RenderWindow& w){
		for(std::vector<sf::Drawable*>::iterator i= _drelems.begin(); i!= _drelems.end(); i++)
			w.draw(**i);
	}
	// true drawing function!!!
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const{
		for(std::vector<sf::Drawable*>::const_iterator i= _drelems.begin(); i!= _drelems.end(); i++)
			target.draw(**i,states);
	}
	// removes button from common buttons list (sorry but you have to remember the order, buttons added)
	void RemoveElement(int listed){
		_elems.erase(_elems.begin()+listed);
		_drelems.erase(_drelems.begin()+listed);
		_mystor.Remove(listed);
	}
	~HUD(){

	}
private:
	// pressable elemets list
	std::vector<Pressable*> _elems;
	// drawable elements list
	std::vector<sf::Drawable*> _drelems;
	Storage _mystor;
};

#endif // gamehud_h