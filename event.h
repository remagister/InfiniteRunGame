#pragma once

#ifndef EVENT_H
#define EVENT_H


// Object & 1 parameter

template <class Object, class Args_type>
class Event{
public:
	typedef typename Event<Object, Args_type> _Myt;
	typedef typename Object _myobj;
	typedef typename Args_type _myargs;
	typedef void (_myobj::*_event)(const _myargs &);

	Event(_event evn, _myobj *ptr){
		_MyEvent = evn;
		_optr = ptr;
	}
	Event(const _Myt& t): _MyEvent(t._MyEvent), _optr(t._optr){
	}
	Event(){
		_MyEvent = 0;
	}

	operator _event(){
		return _MyEvent;
	}

	_event GetEvent()const{
		return _MyEvent;
	}

	_myobj* GetObjectPtr()const{
		return _optr;
	}

	void Exec(const _myargs& what){
		if(_MyEvent) _MyEvent(what);
	}
	void SetEvent(_event evn){
		_MyEvent = evn;
	}
	_Myt& operator= (const _Myt& t){
		_MyEvent = t._MyEvent;
		_optr = t._optr;
		return *this;
	}
	_Myt& operator= (_event evn){
		_MyEvent = evn;
		return *this;
	}
	void operator()(const _myargs& what){
		(_optr->*_MyEvent)(what);
	}
private:
	_myobj* _optr;
	_event _MyEvent;
};

// noreturn & 1 parameter

template <class Arg>
class Event<void, Arg>{
public:
	typedef typename Event<void, Arg> _Myt;
	typedef typename Arg _myarg;
	typedef void (*_event)(const _myarg&);

	Event(_event evn){
		_MyEvent = evn;
	}

	Event(){
		_MyEvent = 0;
	}
	operator _event(){
		return _MyEvent;
	}


	_event GetEvent()const{
		return _MyEvent;
	}

	void Exec(const _myarg& what){
		if(_MyEvent) _MyEvent(what);
	}
	void SetEvent(_event evn){
		_MyEvent = evn;
	}
	template<class Obj>
	void SetEvent(void (Obj::*evn)(const _myarg&)){
		
	}
	_Myt& operator= (const _Myt& t){
		_MyEvent = t._MyEvent;
		return *this;
	}
	_Myt& operator= (_event evn){
		_MyEvent = evn;
		return *this;
	}
	void operator()(const _myarg& what){
		_MyEvent(what);
	}
private:
	_event _MyEvent;
};

template <class Return>
// resolved event
class Event<Return, void>{
public:
	typedef typename Event<Return, void> _Myt;
	typedef typename Return _myret;
	typedef _myret (*_event)();

	Event(typename Return (*evn) ()){
		_MyEvent = evn;
	}
	Event(){
		_MyEvent = 0;
	}
	operator _event(){
		return _MyEvent;
	}
	_event GetEvent()const{
		return _MyEvent;
	}

	typename Return Exec(){
		if(_MyEvent)
			return _MyEvent();
		else return 0;
	}
	void SetEvent(typename Return (*evn) ()){
		_MyEvent = evn;
	}
	Event<Return,void> & operator= (Event<Return, void>& t){
		_MyEvent = t._MyEvent;
		return *this;
	}
	Event<Return, void>& operator= (typename Return (*evn) ()){
		_MyEvent = evn;
		return *this;
	}
	_myret operator()(void){
		return _MyEvent();
	}
private:
	_event _MyEvent;
};

template <>
// resolved event
class Event<void, void>{
public:
	Event(void (*evn) ()){
		_MyEvent = evn;
	}
	Event(){
		_MyEvent = 0;
	}

	void Exec(){
		if(_MyEvent)
			_MyEvent();
	}
	void SetEvent(void (*evn) ()){
		_MyEvent = evn;
	}
	Event<void,void>& operator= (const Event<void, void>& t){
		_MyEvent = t._MyEvent;
		return *this;
	}
	Event<void, void>& operator= (void (*evn) ()){
		_MyEvent = evn;
		return *this;
	}

	void operator()(void){
		_MyEvent();
	}
	typedef void (*_event)();

	operator _event(){
		return _MyEvent;
	}

	_event GetEvent()const{
		return _MyEvent;
	}

private:
	_event _MyEvent;
};

template <class Object, class Argument>
class EBinder{
public:
	typedef typename Object _obj;
	typedef typename Argument _arg;
	typedef void(_obj::*_evn)(const _arg&);
	typedef EBinder<_obj, _arg> _myt;

	EBinder(){}
	EBinder(_evn eventFunc, _obj * optr, const _arg& argument): _event(eventFunc, optr), _myarg(argument){}

	void Bind(_evn newEvent, const _arg& argument){
		_event = newEvent;
		_myarg = argument;
	}

	void Bind(const _arg& arg){
		_myarg = arg;
	}

	_myt& operator = (_evn evn){
		_event = evn;
	}
	_myt& operator= (const _myt& t){
		_event = t._event;
		_myarg = t._myarg;
		return *this;
	}
	void Exec(){
		_event(_myarg);
	}

private:
	Event<_obj, _arg> _event;
	_arg _myarg;
};

template <class Argument>
class EBinder<void, Argument>{
public:
	typedef typename Argument _arg;
	typedef void (*_evn)(const _arg&);
	typedef EBinder<void, Argument> _myt;

	EBinder(){}
	EBinder(_evn eventFunc, const _arg& argument): _event(eventFunc), _myarg(argument){}

	void Bind(_evn newEvent, const _arg& argument){
		_event = newEvent;
		_myarg = argument;
	}

	_myt& operator = (const _evn& evn){
		_event = evn;
	}
	_myt& operator= (const _myt& t){
		_event = t._event;
		_myarg = t._myarg;
		return *this;
	}
	void Bind(const _arg& arg){
		_myarg = arg;
	}

	void Exec(){
		_event(_myarg);
	}

protected:
	Event<void, _arg> _event;
	_arg _myarg;
};

template <class Return>
class EBinder<Return, void>{
public:
	typedef typename Return _ret;
	typedef EBinder<Return,void> _myt;
	typedef void (*_evn)(void);

	EBinder(){}
	EBinder(_evn eventFunc): _event(eventFunc){}

	void Bind(_evn newEvent){
		_event = newEvent;
	}

	_myt& operator = (const _evn& evn){
		_event = evn;
	}
	_myt& operator= (const _myt& r){
		_event = r._event;
		return *this;
	}

	_ret Exec(){
		return _event();
	}
protected: 
	Event<_ret, void> _event;
};

template <>
class EBinder<void, void>{
public:
	typedef void (*_evn)(void);
	typedef EBinder<void,void> _myt;
	EBinder(){}
	EBinder(_evn eventFunc): _event(eventFunc){}

	_myt& operator = (const _evn& evn){
		_event = evn;
	}
	_myt& operator= (const _myt& r){
		_event = r._event;
		return *this;
	}
	
	void Bind(_evn newEvent){
		_event = newEvent;
	}
	void Exec(){
		_event();
	}

protected:
	Event<void, void> _event;
};

#ifndef EVENT_F_
#define EVENT_F_
template <class Object, class Argument>
EBinder<Object, Argument> BindEvent(void (Object::*event)(const Argument& t), Object* ptr, const Argument& toReturn){
	return EBinder<Object, Argument>(event, ptr, toReturn);
}

template <class Return>
EBinder<Return, void> BindEvent(Return (*event)(void)){
	return EBinder<Return, void>(event);
}

template <class Argument>
EBinder<void, Argument> BindEvent(void (*event)(const Argument& t), const Argument& toReturn){
	return EBinder<void, Argument>(event, toReturn);
}

template <>
EBinder<void, void> BindEvent(void (*event)(void)){
	return EBinder<void, void>(event);
}
#endif EVENT_F_
#endif // event h