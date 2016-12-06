/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "graphicplayer.h"
#include "graphicproperties.h"
#include "focusproperties.h"
#include "../property/propertyimpl.h"
#include "../device.h"
#include <util/assert.h>
#include <util/log.h>
#include <canvas/system.h>
#include <canvas/point.h>
#include <canvas/size.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <boost/bind.hpp>

namespace player {

static bool checkFit( const std::string &fitMode ) {
	if (fitMode == "fill" ||
		fitMode == "hidden" ||
		fitMode == "meet" ||
		fitMode == "meetBest" ||
		fitMode == "slice") {
		return true;
	}
	else {
		return false;
	}
}

GraphicPlayer::GraphicPlayer( Device *dev )
	: Player( dev ), _fitMode( "fill" )
{
	canvas::Canvas *canv = canvas();
	DTV_ASSERT(canv);
	_graphic = new GraphicProperties( canv );
	_focus = new FocusProperties( canv );

	//	Register graphics properties events
	_graphic->onSizeChanged( boost::bind(&GraphicPlayer::onSizeChanged,this,_1) );
	_graphic->onPositionChanged( boost::bind(&GraphicPlayer::onPositionChanged,this,_1) );
	_graphic->onBoundsChanged( boost::bind(&GraphicPlayer::onBoundsChanged,this,_1) );	

	_zIndex = 0;
	_visible = true;
}

GraphicPlayer::~GraphicPlayer()
{
	delete _graphic;
	delete _focus;
}

//	Initialization
bool GraphicPlayer::startPlay() {
	LDEBUG("GraphicPlayer", "Start play");
	if (!_graphic->createSurface()) {
		return false;
	}

	if (!_focus->createSurface()) {
		_graphic->destroy();
		return false;
	}

	canvas::Rect bounds = _focus->surface()->getBounds();
	_layer = canvas()->createSurface( bounds );
	_layer->autoFlush( true );

	return true;
}

void GraphicPlayer::stopPlay() {
	LDEBUG("GraphicPlayer", "Stop play");

	_graphic->destroy();
	_focus->destroy();

	canvas()->destroy( _layer );
	canvas()->flush();
}

void GraphicPlayer::beginRefresh() {
	LDEBUG("GraphicPlayer", "Begin refresh");
	_graphic->clear();
}

void GraphicPlayer::endRefresh() {
	redraw();
}

void GraphicPlayer::redraw() {
	canvas::Size focusSize = _focus->surface()->getSize();

	_layer->clear();
	_layer->setLocation( _focus->surface()->getLocation() );
	if (focusSize!=_layer->getSize()) {
		_layer->resize( focusSize );
	}

	if (_visible) {
		canvas::Point layerLoc = _layer->getLocation();
		canvas::Point loc = _graphic->surface()->getLocation();
		loc.x -= layerLoc.x;
		loc.y -= layerLoc.y;
		_layer->blit( loc, _graphic->surface() );

		loc.x = 0;
		loc.y = 0;
		_layer->blit( loc, _focus->surface() );
	}
	_layer->flushCompositionMode( flushCompositionMode() );
	_layer->flush();
}

void GraphicPlayer::onPropertyChanged() {
	LDEBUG("GraphicPlayer", "On property changed");
	if (isPlaying()) {
		_focus->refresh();
		redraw();
	}
}

void GraphicPlayer::registerProperties() {
	Player::registerProperties();
	_graphic->registerProperties(this);
	_focus->registerProperties(this);

	{	//	Property fit
		PropertyImpl<std::string> *fit = new PropertyImpl<std::string>(true,_fitMode);
		fit->setCheck( boost::bind(&checkFit,_1) );
		addProperty( property::type::fit, fit );
	}
	{	//	Add zIndex
		PropertyImpl<int> *zIndex = new PropertyImpl<int>( false, _zIndex );
		zIndex->setCheck( boost::bind( std::greater_equal<int>(), _1, 0 ) );
		zIndex->setApply( boost::bind( &GraphicPlayer::applyZIndex,this ) );
		addProperty( property::type::zIndex, zIndex );
	}
	{	//	Add visible
		PropertyImpl<bool> *visible=new PropertyImpl<bool>( false, _visible );
		visible->setApply( boost::bind(&GraphicPlayer::applyVisible,this) );
		addProperty( property::type::visible, visible );
	}
}

void GraphicPlayer::applyZIndex() {
	LDEBUG("GraphicPlayer", "apply zIndex, value=%d", _zIndex);
	_layer->setZIndex( _zIndex );
}

//	Events
void GraphicPlayer::onBoundsChanged( const canvas::Rect &rect ) {
	{
		canvas::Size size(rect);
		onSizeChanged(size);
	}

	{
		canvas::Point point(rect);
		onPositionChanged(point);
	}
}

void GraphicPlayer::onSizeChanged( const canvas::Size & /*size*/ ) {
}

void GraphicPlayer::onPositionChanged( const canvas::Point & /*point*/ ) {
}

//	Getters
canvas::Canvas *GraphicPlayer::canvas() const {
	return device()->system()->canvas();
}

canvas::Surface *GraphicPlayer::surface() const {
	return _graphic->surface();
}

void GraphicPlayer::applyVisible() {
	LDEBUG("GraphicPlayer", "apply visible, value=%d", _visible );
	_layer->setVisible( _visible );
}

bool GraphicPlayer::isVisible() const {
	return _visible;
}

bool GraphicPlayer::isGraphic() const {
	return true;
}

fit::mode::type GraphicPlayer::getFitMode() const {
	fit::mode::type mode;
	if (_fitMode == "hidden") {
		mode = fit::mode::hidden;
	}
	else if (_fitMode == "meet") {
		mode = fit::mode::meet;
	}
	else if (_fitMode == "meetBest") {
		mode = fit::mode::meetBest;
	}
	else if (_fitMode == "slice") {
		mode = fit::mode::slice;
	}
	else {
		mode = fit::mode::fill;
	}
	return mode;
}

canvas::composition::mode GraphicPlayer::flushCompositionMode() const {
	return canvas::composition::source_over;
}

}
