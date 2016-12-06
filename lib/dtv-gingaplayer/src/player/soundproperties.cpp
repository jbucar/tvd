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

#include "soundproperties.h"
#include "../property/propertyimpl.h"
#include "../player.h"
#include <canvas/mediaplayer.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>

namespace player {

/**
 * Constructor base.
 * @param media Instancia de la clase @b canvas::MediaPlayer.
 */
SoundProperties::SoundProperties( canvas::MediaPlayer *media )
	: _media( media )
{
	DTV_ASSERT(_media);
	
	_soundLevel = 1.0f;
	_balanceLevel = 1.0f;
	_trebleLevel = 1.0f;
	_bassLevel = 1.0f;
}

/**
 * Destructor base.
 */
SoundProperties::~SoundProperties()
{
}

/**
 * Registra las propiedades de sonido en el Player.
 * @param player Instancia de Player al cual se le agregaran las propiedades de sonido.
 */
void SoundProperties::registerProperties( Player *player ) {
	{	//	Add soundLevel
		PropertyImpl<float> *prop = new PropertyImpl<float>( false, _soundLevel );
		prop->setCheck( boost::bind( &check::range<float>, _1, 0.0f, 1.0f ) );
		prop->setApply( boost::bind( &SoundProperties::applySoundLevel, this ) );
		player->addProperty( property::type::soundLevel, prop );
	}

	// {	//	Add balanceLevel
	// 	PropertyImpl<float> *prop = new PropertyImpl<float>( false, _balanceLevel );
	// 	prop->setCheck( boost::bind( &check::range<float>, _1, 0.0f, 1.0f ) );
	// 	prop->setApply( boost::bind( &SoundProperties::applyBalanceLevel, this ) );		
	// 	player->addProperty( property::type::balanceLevel, prop );
	// }

	// {	//	Add trebleLevel
	// 	PropertyImpl<float> *prop = new PropertyImpl<float>( false, _trebleLevel );
	// 	prop->setCheck( boost::bind( &check::range<float>, _1, 0.0f, 1.0f ) );
	// 	prop->setApply( boost::bind( &SoundProperties::applyTrebleLevel, this ) );				
	// 	player->addProperty( property::type::trebleLevel, prop );
	// }

	// {	//	Add bassLevel
	// 	PropertyImpl<float> *prop = new PropertyImpl<float>( false, _bassLevel );
	// 	prop->setCheck( boost::bind( &check::range<float>, _1, 0.0f, 1.0f ) );
	// 	prop->setApply( boost::bind( &SoundProperties::applyBassLevel, this ) );						
	// 	player->addProperty( property::type::bassLevel, prop );
	// }
}

void SoundProperties::applySoundLevel() {
	LDEBUG("SoundProperties", "apply sound level, value=%f", _soundLevel);
	_media->volume( _soundLevel );
}

void SoundProperties::applyBalanceLevel() {
	LDEBUG("SoundProperties", "apply balance level, value=%f", _balanceLevel );	
}

void SoundProperties::applyTrebleLevel() {
	LDEBUG("SoundProperties", "apply trebe Level, value=%f", _trebleLevel);	
}

void SoundProperties::applyBassLevel() {
	LDEBUG("SoundProperties", "apply bass level, value=%f", _bassLevel);	
}

}
