/******************************************************************************
 Este arquivo eh parte da implementacao do ambiente declarativo do middleware
 Ginga (Ginga-NCL).

 Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

 Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
 os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
 Software Foundation.

 Este programa eh distribuido na expectativa de que seja util, porem, SEM
 NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
 ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
 GNU versao 2 para mais detalhes.

 Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
 com este programa; se nao, escreva para a Free Software Foundation, Inc., no
 endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

 Para maiores informacoes:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 ******************************************************************************
 This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

 Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.
 Copyright 2010 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata


 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
 details.

 You should have received a copy of the GNU General Public License version 2
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

 For further information contact:
 ncl @ telemidia.puc-rio.br
 http://www.ncl.org.br
 http://www.ginga.org.br
 http://www.telemidia.puc-rio.br
 *******************************************************************************/

#include "../../include/adapters/FormatterPlayerAdapter.h"
#include "../animation/animation.h"
#include <ncl30/util/functions.h>
#include <gingaplayer/player.h>
#include <gingaplayer/device.h>
#include <gingaplayer/system.h>
#include <gingaplayer/player/keys.h>
#include <gingaplayer/property/types.h>
#include <util/mcr.h>
#include <util/log.h>

namespace prop = player::property::type;
namespace bptnt = ::br::pucrio::telemidia::ncl::transition;

#include "../../include/adapters/ApplicationPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application;

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//	Player properties
#define check_int( value, checkedValue, check ) \
	try { \
		checkedValue = boost::lexical_cast<int>( value ); \
	} catch (boost::bad_lexical_cast &) { \
		check = false; \
	} \

#define check_float( value, checkedValue, check ) \
	try { \
		checkedValue = boost::lexical_cast<float>( value ); \
	} catch (boost::bad_lexical_cast &) { \
		check = false; \
	} \

#define check_bool( value, checkedValue, check ) \
	if (value == "true") { \
		checkedValue = true; \
	} else if( value == "false") { \
		checkedValue = false; \
	} else { \
		check = false; \
	} \

#define init_bool false
#define init_int 0
#define init_float 0.0


#define APPLY_PROPERTY( type, name, value ) \
	bool check = true; \
	type checkedValue = init_##type; \
	check_##type( value, checkedValue, check ); \
	if ( check ) { \
		result = _player->setProperty( name, checkedValue ); \
	} else { \
		LERROR("FormatterPlayerAdapter", "%s invalid value, %s expected; value=%s", name.c_str(), #type, value.c_str() ); \
	} \

#define APPLY_INVERSE_PROPERTY( type, name, value ) \
	bool check = true; \
	type checkedValue = init_##type; \
	check_##type( value, checkedValue, check ); \
	if ( check ) { \
		result = _player->setProperty( name, 1 -  checkedValue ); \
	} else { \
		LERROR("FormatterPlayerAdapter", "%s invalid value, %s expected; value=%s", name.c_str(), #type, value.c_str() ); \
	} \

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {

namespace impl {
static FormatterPlayerAdapter::KeyPressCallback _onKey;
}

FormatterPlayerAdapter::FormatterPlayerAdapter( PlayerAdapterManager* manager, player::System *system ) {
	typeSet.insert( "FormatterPlayerAdapter" );
	this->manager = manager;
	this->object = NULL;
	_player = NULL;
	_isKeyhandler = true;
	_sys = system;
}

FormatterPlayerAdapter::~FormatterPlayerAdapter() {
	object = NULL;
	manager = NULL;
	stopPlayer();
	_player->device()->destroy( _player );
}

bool FormatterPlayerAdapter::instanceOf( string s ) {
	if (!typeSet.empty()) {
		return (typeSet.find( s ) != typeSet.end());
	} else {
		return false;
	}
}

void FormatterPlayerAdapter::setPlayer( player::Player* player ) {
	_player = player;
}

void FormatterPlayerAdapter::stopPlayer() {
	_sys->delInputListener( this );
	_onButton.disconnect();
	player::Player::OnStatusChanged fnc;
	_player->onStopped( fnc );
}

set<string> FormatterPlayerAdapter::getTypeSet( void ) const {
	return typeSet;
}

PlayerAdapterManager* FormatterPlayerAdapter::getManager( void ) const {
	return manager;
}

player::Player* FormatterPlayerAdapter::getPlayer( void ) const {
	return _player;
}

player::System* FormatterPlayerAdapter::getSystem() {
	return _sys;
}

ExecutionObject* FormatterPlayerAdapter::getObject( void ) const {
	return object;
}

void FormatterPlayerAdapter::setObject( ExecutionObject* o ) {
	object = o;
	if (_player != NULL) {
		_player->onStopped( boost::bind( &PlayerAdapterManager::stop, getManager(), object, false ) );
	}
}

bool FormatterPlayerAdapter::createPlayer() {
	vector<Anchor*>* anchors;
	vector<Anchor*>::iterator i;

	vector<FormatterEvent*>* events;
	vector<FormatterEvent*>::iterator j;

	NodeEntity* dataObject;
	bool result = true;
	//TODO player player->addListener(this);

	if (object == NULL) {
		return false;
	}

	result &= setDescriptorParams();

	dataObject = (NodeEntity*) (object->getDataObject());
	if (dataObject->instanceOf( "ContentNode" )) {
		anchors = ((ContentNode*) dataObject)->getAnchors();
		if (anchors != NULL) {
			i = anchors->begin();
			while (i != anchors->end()) {
				if ((*i)->instanceOf( "PropertyAnchor" )) {
					PropertyAnchor *property = ((PropertyAnchor*) (*i));
					result &= setPropertyToPlayer( property->getPropertyName(), property->getPropertyValue() );
				}
				++i;
			}
		}
	}

	object->getDescriptor()->getFormatterRegion()->prepareOutputDisplay( this );

	events = object->getEvents();
	if (events != NULL) {
		j = events->begin();
		while (j != events->end()) {
			if (*j != NULL && (*j)->instanceOf( "AttributionEvent" )) {
				((AttributionEvent*) (*j))->setValueMaintainer( this );
			}
			++j;
		}
		delete events;
		events = NULL;
	}
	return result;
}

#define setIfNotEmpty( prop, value ) \
		if( !value.empty() ) { \
			result &= setPropertyToPlayer( prop, value ); \
		} \

bool FormatterPlayerAdapter::setDescriptorParams() {
	bool result = true;
	CascadingDescriptor* descriptor = object->getDescriptor();
	if (descriptor != NULL) {
		if (descriptor->focusDecoration() != NULL) {
			setIfNotEmpty( "focusBorderColor", descriptor->focusDecoration()->getFocusBorderColor() );
			setIfNotEmpty( "focusBorderTransparency", descriptor->focusDecoration()->getFocusBorderTransparency() );
			setIfNotEmpty( "focusBorderWidth", descriptor->focusDecoration()->getFocusBorderWidth() );
			setIfNotEmpty( "focusSrc", descriptor->focusDecoration()->getFocusSrc() );
			setIfNotEmpty( "selBorderColor", descriptor->focusDecoration()->getSelBorderColor() );
			setIfNotEmpty( "focusSelSrc", descriptor->focusDecoration()->getFocusSelSrc() );
		}
		vector<Parameter*>* params = descriptor->getParameters();
		vector<Parameter*>::iterator param;
		for (param = params->begin(); param != params->end(); param++) {
			setPropertyToPlayer( (*param)->getName(), (*param)->getValue() );
			delete (*param);
		}
		delete params;
	}
	return result;
}

void FormatterPlayerAdapter::setProperty( const std::string &propertyName, const std::string &propertyValue ) {
	_properties[propertyName] = propertyValue;
}

const char *FormatterPlayerAdapter::toPlayerProperty( const std::string &propertyName ) {
	if (propertyName == "background") {
		return "backgroundColor";
	} else if (propertyName == "transparency") {
		return "opacity";
	}
	return propertyName.c_str();
}

bool FormatterPlayerAdapter::setPropertyToPlayer( const std::string &name, const std::string &value ) {

	if (value.empty()) {
		return true;
	}

	bool result = false;

	setProperty( name, value );

	std::string propertyName = toPlayerProperty( name );
	prop::Type property = player::property::getProperty( propertyName.c_str() );

	if (property != prop::null && !value.empty()) {
		switch ( property ) {
			case prop::top:
			case prop::bottom:
			case prop::left:
			case prop::right:
			case prop::width:
			case prop::height:
			case prop::bounds:
			case prop::location:
			case prop::size:
			case prop::zIndex: {
				result = object->setProperty( propertyName, value );
				break;
			}
			case prop::opacity: {
				if (ncl_util::isPercentualValue( value )) {
					result = _player->setProperty( propertyName, 1 - ncl_util::getPercentualValue( value ) );
				} else {
					APPLY_INVERSE_PROPERTY( float, propertyName, value);
				}
				break;
			}
			case prop::focusBorderTransparency:
			case prop::soundLevel:
			case prop::balanceLevel:
			case prop::trebleLevel:
			case prop::bassLevel: {
				if (ncl_util::isPercentualValue( value )) {
					result = _player->setProperty( propertyName, ncl_util::getPercentualValue( value ) );
				} else {
					APPLY_PROPERTY( float, propertyName, value );
				}
				break;
			}
			case prop::visible: {
				APPLY_PROPERTY( bool, propertyName, value );
				break;
			}
			case prop::fontSize:
			case prop::focusBorderWidth: {
				APPLY_PROPERTY( int, propertyName, value );
				break;
			}
			case prop::focusSelSrc:
			case prop::focusSrc: {
				std::string url;
				if (getManager()->getUrl( value, url )) {
					result = _player->setProperty( propertyName, url );
				}
				break;
			}
			case prop::focusIndex: {
				object->getDescriptor()->setFocusIndex( value );
				result = true;
				break;
			}
			case prop::src: {
				//	Filter properties
				LWARN("FormatterPlayerAdapter", "property %s not supported", propertyName.c_str() );
				result = false;
				break;
			}
			case prop::explicitDur: {
				util::DWORD time;
				if( ncl_util::parseTimeString( value, time ) ) {
					result = _player->setProperty( propertyName, time );
				}
				break;
			}
			default: {
				result = _player->setProperty( propertyName, value );
				break;
			}
		};
	}

	LDEBUG("FormatterPlayerAdapter","set: property=%s, value=%s, result=%d", name.c_str(), value.c_str(), result );
	return result;
}

bool FormatterPlayerAdapter::setCurrentEvent( FormatterEvent* /*event*/) {
	return false;
}

bool FormatterPlayerAdapter::setPropertyValue( AttributionEvent* event, const std::string &value ) {

	std::string propName = (event->getAnchor())->getPropertyName();
	if (!setPropertyToPlayer( propName, value )) {
		if (this->instanceOf( "ApplicationPlayerAdapter" )) {
			((ApplicationPlayerAdapter*) this)->setCurrentEvent( event );
		}
	}

	if (!this->instanceOf( "ApplicationPlayerAdapter" )) {
		event->stop();
	}
	return true;
}

bool FormatterPlayerAdapter::startAnimation( AttributionEvent* event, const std::string &value, bptna::Animation* animation ) {
	int duration = boost::lexical_cast<int>( animation->getDuration() );
	double by = boost::lexical_cast<double>( animation->getBy() );

	std::string property = event->getAnchor()->getPropertyName();
	LayoutRegion* region = object->getDescriptor()->getFormatterRegion()->getLayoutRegion();
	int startValue = 0;
	prop::Type propertyName = player::property::getProperty( property.c_str() );
	if (propertyName != prop::null) {
		switch ( propertyName ) {
			case prop::top: {
				startValue = region->getTop();
				break;
			}
			case prop::bottom: {
				startValue = region->getBottom();
				break;
			}
			case prop::left: {
				startValue = region->getLeft();
				break;
			}
			case prop::right: {
				startValue = region->getRight();
				break;
			}
			case prop::width: {
				startValue = region->getWidth();
				break;
			}
			case prop::height: {
				startValue = region->getLeft();
				break;
			}
			default: {
				LWARN("FormatterPlayerAdapter", "property %s not supported in animation", property.c_str() );
				return false;
			}
		}
	}

	int endValue = boost::lexical_cast<int>( value );
	bptgnan::Animation *a = new bptgnan::Animation( duration, by, _sys );
	a->addProperty( property, startValue, endValue );
	a->onEnd( boost::bind( &AttributionEvent::stop, event ) );
	return object->getDescriptor()->getFormatterRegion()->startAnimation( a );
}

bool FormatterPlayerAdapter::hasPrepared() {
	FormatterEvent* mEv;
	short st;
	/*
	 if (object == NULL || player == NULL) {
	 return false;
	 }

	 presented = player->hasPresented();
	 if (presented) {
	 return false;
	 }

	 presented = player->isForcedNaturalEnd();
	 if (presented) {
	 return false;
	 }
	 */
	if (object != NULL) {
		mEv = object->getMainEvent();
		if (mEv != NULL && !object->instanceOf( "ApplicationExecutionObject" )) {
			st = mEv->getCurrentState();
			if (st != EventUtil::ST_SLEEPING) {
				return true;
			}
		}
	}
	return false;
}

bool FormatterPlayerAdapter::setKeyHandler( bool isHandler ) {
	_isKeyhandler = isHandler;
	return isHandler;
}

bool FormatterPlayerAdapter::prepare( ExecutionObject* object, FormatterEvent* event ) {
	CascadingDescriptor* descriptor;
	bool result;
	double explicitDur;

	if (hasPrepared()) {
		LWARN("FormatterPlayerAdapter", "prepare, returns false, because player is already prepared" );
		return false;
	}
	/*
	 if (!lockObject()) {
	 if (player != NULL) {
	 if (player->hasPresented() || player->isForcedNaturalEnd()) {
	 while (!lockObject());

	 } else {
	 return false;
	 }
	 }
	 }*/

	if (object == NULL) {
		LWARN("FormatterPlayerAdapter", "Trying to prepare a NULL object." );
		return false;
	}

	this->object = object;

	if (event->instanceOf( "PresentationEvent" )) {
		descriptor = object->getDescriptor();
		if (descriptor != NULL) {
			explicitDur = descriptor->getExplicitDuration();
			if (!ncl_util::isNaN( explicitDur )) {
				object->removeEvent( event );
				((PresentationEvent*) event)->setEnd( explicitDur );
				object->addEvent( event );
			}
		}
	}

	result = createPlayer();

	if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
		object->prepare( (PresentationEvent*) event, 0 );
		return result;
	} else {
		return false;
	}
}

bool FormatterPlayerAdapter::start() {
	bool visible = _player->isVisible();
	if (object != NULL) {
		if (object->canStart()) {
			if (visible && object->getDescriptor()->getFormatterRegion()->haveTransitionIn()) {
				if (_player->isGraphic()) {
					_player->setProperty( "visible", false );
					visible = true;
				}
			}
			if (_player->play()) {
				object->start();
				player::keys::onReservationChanged().connect( boost::bind(&FormatterPlayerAdapter::onReserveKeys,this,_1) );
				onReserveKeys( player::keys::reservedKeys() );
				processTransitions();
				object->getDescriptor()->getFormatterRegion()->transitionIn();
				if (_player->isGraphic()) {
					_player->setProperty( "visible", visible );
				}
				return true;
			}

		}
	}
	return false;
}

void FormatterPlayerAdapter::onReserveKeys( util::key::Keys keys ) {
	canvas::input::Callback fnc = boost::bind( &FormatterPlayerAdapter::userEventReceived, this, _1, _2 );
	_sys->addInputListener( this, fnc, keys);
}

void FormatterPlayerAdapter::processTransitions( void ) {
	if (object != NULL) {
		EventTransition* t = object->getNextTransition();
		if (t != NULL && !ncl_util::isInfinity( t->getTime() )) {
			_player->createTimer( (util::DWORD) t->getTime(), boost::bind( &PlayerAdapterManager::onTransitionTimer, getManager(), getObject(), t->getTime() ) );
		}
	}
}

bool FormatterPlayerAdapter::stop( bool proccessTransitions /*=true*/) {

	if (proccessTransitions && object != NULL && object->getDescriptor()->getFormatterRegion()->haveTransitionOut()) {
		object->getDescriptor()->getFormatterRegion()->transitionOut();
		return false;
	}

	FormatterEvent* mainEvent;
	vector<FormatterEvent*>* events = NULL;

	stopPlayer();

	if (object == NULL) {
		return false;

	} else {
		mainEvent = object->getMainEvent();
		events = object->getEvents();
	}

	if (mainEvent != NULL && mainEvent->instanceOf( "PresentationEvent" )) {
		if (checkRepeat( (PresentationEvent*) mainEvent )) {
			return true;
		}
	}

	if (events != NULL) {
		vector<FormatterEvent*>::iterator i;
		i = events->begin();
		while (i != events->end()) {
			if (*i != NULL && (*i)->instanceOf( "AttributionEvent" )) {
				((AttributionEvent*) (*i))->setValueMaintainer( NULL );
			}

			if (i != events->end()) {
				++i;
			}
		}
		delete events;
		events = NULL;
	}

	if (object != NULL) {
		object->getDescriptor()->getFormatterRegion()->unprepareOutputDisplay();
		object->stop();
		object->unprepare();
		return true;
	}
	return false;
}

bool FormatterPlayerAdapter::pause() {
	if (object != NULL && object->pause()) {
		_player->pause( true );
		//TODO player->notifyReferPlayers(EventUtil::TR_PAUSES);
		return true;

	} else {
		return false;
	}
}

bool FormatterPlayerAdapter::resume() {
	if (object != NULL && object->resume()) {
		_player->pause( false );
		// player->notifyReferPlayers(EventUtil::TR_RESUMES);
		return true;
	}
	return false;
}

bool FormatterPlayerAdapter::abort() {
	return object != NULL && object->abort();
}

bool FormatterPlayerAdapter::checkRepeat( PresentationEvent* event ) {
	if (event->getRepetitions() > 1) {
		_player->stop();
		//TODO player->notifyReferPlayers(EventUtil::TR_STOPS);
		if (object != NULL) {
			object->stop();
		}
		//player->setImmediatelyStart(true);
		return true;
	}
	return false;
}

string FormatterPlayerAdapter::getPropertyValue( AttributionEvent* event ) {
	std::string value = "";
	if (event != NULL) {
		value = _properties[event->getAnchor()->getPropertyName()];
	}
	return value;
}

util::DWORD FormatterPlayerAdapter::uptime() const {
	return _player->uptime();
}

void FormatterPlayerAdapter::userEventReceived( ::util::key::type key, bool isUp ) {

	if (isKeyHandler() && object != NULL && !isUp) {
		//TODO ? if (player->isVisible()) {
		object->selectionEvent( key, uptime() );
		if (!impl::_onKey.empty()) {
			impl::_onKey( key );
		}
	}
}
void FormatterPlayerAdapter::onButtonEvent( ::canvas::input::ButtonEvent *evt ) {
	//	TODO check onSelection event rules
	userEventReceived( ::util::key::null, !evt->isPress);
}

void FormatterPlayerAdapter::timeShift( const std::string &/*direction */) {
}

bool FormatterPlayerAdapter::isKeyHandler( void ) const {
	return _isKeyhandler;
}

void FormatterPlayerAdapter::onKeyPress( const FormatterPlayerAdapter::KeyPressCallback &callback ) {
	impl::_onKey = callback;
}

}
}
}
}
}
}
