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

#include "focusproperties.h"
#include "../property/propertyimpl.h"
#include "../player.h"
#include <canvas/system.h>
#include <canvas/canvas.h>
#include <canvas/surface.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <boost/bind.hpp>

namespace player {

static bool checkSource( const std::string &file ) {
	if (file.empty()) {
		return true;
	}
	return check::fileExists( file );
}

FocusProperties::FocusProperties( canvas::Canvas *canvas )
	: SurfaceProperties( canvas )
{
	_focus = false;
	_selected = false;
	_applied = false;	
	_borderColor =  util::cfg::getValue<std::string>( "gingaPlayer.default.focusBorderColor" );
	_selBorderColor = util::cfg::getValue<std::string>( "gingaPlayer.default.selBorderColor" );
	_width = util::cfg::getValue<int>( "gingaPlayer.default.focusBorderWidth" );;
	_transparency = util::cfg::getValue<float>( "gingaPlayer.settings.focusBorderTransparencyAsFloat" );
}

FocusProperties::~FocusProperties()
{
}

void FocusProperties::registerProperties( Player *player ) {
	SurfaceProperties::registerProperties( player );
	
	{	//	Add focus
		MultiPropertyImpl<bool> *prop=new MultiPropertyImpl<bool>( _applied, false, _focus );
		player->addProperty( property::type::focus, prop );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );
	}

	{	//	Add selected
		MultiPropertyImpl<bool> *prop=new MultiPropertyImpl<bool>( _applied, false, _selected );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );		
		player->addProperty( property::type::focusSelected, prop );
	}	

	{	//	Add focus border color
		MultiPropertyImpl<std::string> *prop=new MultiPropertyImpl<std::string>( _applied, false, _borderColor );
		prop->setCheck( boost::bind(&check::color,_1) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );		
		player->addProperty( property::type::focusBorderColor, prop );
	}

	{	//	Add selected border color
		MultiPropertyImpl<std::string> *prop=new MultiPropertyImpl<std::string>( _applied, false, _selBorderColor );
		prop->setCheck( boost::bind(&check::color,_1) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );
		player->addProperty( property::type::selBorderColor, prop );
	}	

	{	//	Add border width
		MultiPropertyImpl<int> *prop=new MultiPropertyImpl<int>( _applied, false, _width );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );		
		player->addProperty( property::type::focusBorderWidth, prop );
	}

	{	//	Add border transparency
		MultiPropertyImpl<float> *prop=new MultiPropertyImpl<float>( _applied, false, _transparency );
		prop->setCheck( boost::bind(&check::range<float>,_1,0.0f,1.0f) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );		
		player->addProperty( property::type::focusBorderTransparency, prop );
	}

	{	//	Focus source property
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, false, _focusSource );
		prop->setCheck( boost::bind(&checkSource,_1) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );				
		player->addProperty( property::type::focusSrc, prop );
	}

	{	//	Focus selected source property
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, false, _focusSelSource );
		prop->setCheck( boost::bind(&checkSource,_1) );
		prop->setApply( boost::bind(&FocusProperties::refresh,this) );				
		player->addProperty( property::type::focusSelSrc, prop );		
	}
}

void FocusProperties::refresh() {
	LDEBUG("FocusProperties", "Apply focus properties: focus=%d, selected=%d", _focus, _selected);

	surface()->clear();
	if (_focus) {
		if (_selected) {
 			drawSelected();
		}
 		else {
 			drawFocus();
 		}
	}
}

void FocusProperties::drawFocus() {
	if (!_focusSource.empty()) {
		composeSurface( _focusSource );
 	}
	drawBorder( _borderColor );
}

void FocusProperties::drawSelected() {
	if (!_focusSelSource.empty()) {
		composeSurface( _focusSelSource );
 	} 
	drawBorder( _selBorderColor );
}

void FocusProperties::drawBorder( const std::string &strColor ) {
	//	Set color
	canvas::Color color;
	canvas::color::get( strColor.c_str(), color );
	color.alpha = (util::BYTE) (255-(255*_transparency));
	surface()->setColor( color );

	if (_width < 0) {
		//	Draw rectangles
		canvas::Size size = surface()->getSize();
		for (int i=0; i < abs(_width); ++i) {
			canvas::Rect border;
			border.x = i;
			border.y = i;
			border.w = size.w-i*2;
			border.h = size.h-i*2;
			surface()->drawRect(border);
		}
	} else if (_width > 0) {
		const canvas::Rect &oBounds = bounds();
		const canvas::Rect &fBounds = surface()->getBounds();

		LDEBUG("FocusProperties", "drawBorder: original=(%d,%d,%d,%d), focus=(%d,%d,%d,%d), width=%d",
			oBounds.x, oBounds.y, oBounds.w, oBounds.h, fBounds.x, fBounds.y, fBounds.w, fBounds.h, _width);

		{	//	Up
			int lines=oBounds.y - fBounds.y;
			if (lines) {
				canvas::Rect rect( 0, 0, fBounds.w, lines );
				surface()->fillRect( rect );
			}
		}

		{	//	Down
			int linesUp=oBounds.y - fBounds.y;
			int linesDown=(fBounds.h - oBounds.h) - linesUp;
			canvas::Rect rect( 0, fBounds.h-linesDown, fBounds.w, linesDown );
			surface()->fillRect( rect );			
		}		

		{	//	Left
			int lines=oBounds.x - fBounds.x;
			if (lines) {
				canvas::Rect rect( 0, 0, lines, fBounds.h );
				surface()->fillRect( rect );
			}
		}

		{	//	Right
			int linesLeft=oBounds.x - fBounds.x;
			int linesRight=(fBounds.w - oBounds.w) - linesLeft;
			if (linesRight) {
				canvas::Rect rect( fBounds.w-linesRight, 0, linesRight, fBounds.h );
				surface()->fillRect( rect );
			}
		}				
	}
}

void FocusProperties::composeSurface( const std::string &imageFile ) {
	canvas::Surface *image = canvas()->createSurfaceFromPath( imageFile );
	const canvas::Size &i_size = image->getSize();
	const canvas::Size &s_size = surface()->getSize();
	surface()->scale( canvas::Rect(0,0,s_size.w,s_size.h), image, canvas::Rect(0,0,i_size.w,i_size.h) );
	canvas()->destroy( image );
}

#define GET_X(p) std::max(0,p.x-_width)
#define GET_Y(p) std::max(0,p.y-_width)
void FocusProperties::calculateBounds( canvas::Rect &bounds ) {
	if (_width > 0) {
		const canvas::Size &max=canvas()->size();
		int x = GET_X(bounds);
		int y = GET_Y(bounds);
		
		int linesLeft=bounds.x - x;
		bounds.w = bounds.w+_width+linesLeft;
		if (bounds.w+x > max.w) {
			bounds.w = max.w-x;
		}

		int linesUp=bounds.y - y;
		bounds.h = bounds.h+_width+linesUp;
		if (bounds.h+y > max.h) {
			bounds.h = max.h-y;
		}

		bounds.x = x;
		bounds.y = y;

		LDEBUG("FocusProperties", "calculateBounds: bounds=(%d,%d,%d,%d)", bounds.x, bounds.y, bounds.w, bounds.h);
	}	
}

}
