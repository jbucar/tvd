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

#include "textplayer.h"
#include "../property/propertyimpl.h"
#include <util/log.h>
#include <canvas/rect.h>
#include <canvas/color.h>
#include <canvas/fontinfo.h>
#include <canvas/surface.h>
#include <boost/bind.hpp>
#include <fstream>

namespace player {

static bool checkStyle( const std::string &style ) {
	return !strcmp( "normal", style.c_str() ) || !strcmp( "italic", style.c_str() );
}

static bool checkVariant( const std::string &variant ) {
	return !strcmp( "normal", variant.c_str() ) || !strcmp( "small-caps", variant.c_str() );
}

static bool checkWeight( const std::string &weight ) {
	return !strcmp( "normal", weight.c_str() ) || !strcmp( "bold", weight.c_str() );
}

static bool checkColor( const std::string &color ) {
	canvas::Color tmp;
	return canvas::color::get( color.c_str(), tmp );
}

TextPlayer::TextPlayer( Device *dev )
	: GraphicPlayer( dev )
{
	_applied = false;
	_size = -1;
	_family = "Tiresias";
	_color = "white";
	_style = "normal";
	_variant = "normal";
	_weight = "normal";
}

TextPlayer::~TextPlayer() {
}

void TextPlayer::refresh() {
	std::string text;
	if (readFile( text )) {
		int w = surface()->getBounds().w;
		int h = surface()->getBounds().h;
		surface()->drawText((canvas::Rect(0,0,w,h)), text, canvas::wrapByWord );
	}
}

bool TextPlayer::readFile( std::string &text ) {
	std::ifstream ifs;
	std::string tmp;

	ifs.open( body().c_str() );
	if (!ifs.is_open()) {
		LERROR("TextPlayer", "failed to open file %s", body().c_str());
		return false;
	}
	while (!ifs.eof()) {
		getline( ifs, tmp );
		Player::convertText( tmp );
		text +=  tmp + "\n";
	}
	ifs.close();
	return true;
}

void TextPlayer::registerProperties() {
	GraphicPlayer::registerProperties();
	
	{	//	Add fontSize
		MultiPropertyImpl<int> *prop = new MultiPropertyImpl<int>( _applied, true, _size );
		prop->setCheck( boost::bind( std::greater<int>(), _1, 0) );
		prop->setApply( boost::bind( &TextPlayer::applyFont, this) );
		addProperty( property::type::fontSize, prop );
	}
	{	//	Add fontFamily
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _family );
		prop->setCheck( boost::bind( &check::notEmpty, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontFamily, prop );
	}
	{	//	Add fontStyle
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _style );
		prop->setCheck( boost::bind( &checkStyle, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontStyle, prop );
	}
	{	//	Add fontVariant
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _variant );
		prop->setCheck( boost::bind( &checkVariant, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontVariant, prop );
	}
	{	//	Add fontWeight
		MultiPropertyImpl<std::string> *prop = new MultiPropertyImpl<std::string>( _applied, true, _weight );
		prop->setCheck( boost::bind( &checkWeight, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyFont,this) );
		addProperty( property::type::fontWeight, prop );
	}
	{	//	Add fontColor
		PropertyImpl<std::string> *prop = new PropertyImpl<std::string>( true, _color );
		prop->setCheck( boost::bind( &checkColor, _1 ) );
		prop->setApply( boost::bind( &TextPlayer::applyColor,this) );
		addProperty( property::type::fontColor, prop );
	}
}

void TextPlayer::applyFont() {
	canvas::FontInfo font;
	
	LINFO("TextPlayer", "Apply font: family=%s, style=%s, variant=%s, weight=%s, size=%d",
		_family.c_str(), _style.c_str(), _variant.c_str(), _weight.c_str(), _size);

	//	Apply size
	font.size( _size );

	//	Apply style
	font.italic( !strcmp( "italic", _style.c_str() ) );

	//	Apply variant
	font.smallCaps( !strcmp( "small-caps", _variant.c_str() ) );

	//	Apply weight
	font.bold( !strcmp( "bold", _weight.c_str() ) );
	
	{	//	Apply family
		font.families(_family);
		surface()->setFont(font);
	}
}

void TextPlayer::applyColor() {
	LINFO("TextPlayer", "Apply font color: color=%s", _color.c_str());
	canvas::Color c;
	canvas::color::get( _color.c_str(), c );
	surface()->setColor( c );
}

}

