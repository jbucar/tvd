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

#pragma once

#include <util/types.h>
#include <string>

#define PROPERTY_LIST(fnc) \
        fnc ( device )                     \
                                           \
        fnc ( left )                       \
        fnc ( right )                      \
        fnc ( top )                        \
        fnc ( bottom )                     \
        fnc ( width )                      \
        fnc ( height )                     \
        fnc ( size )                       \
        fnc ( location )                   \
        fnc ( bounds )                     \
        fnc ( zIndex )                     \
        fnc ( title )                      \
                                           \
        fnc ( region )                     \
        fnc ( explicitDur )                \
        fnc ( freeze )                     \
        fnc ( player )                     \
                                           \
        fnc ( focusIndex )                 \
        fnc ( focusBorderColor )           \
        fnc ( focusBorderWidth )           \
        fnc ( focusBorderTransparency )    \
        fnc ( focusSrc )                   \
        fnc ( focusSelSrc )                \
        fnc ( selBorderColor )             \
        fnc ( focus )                      \
        fnc ( focusSelected )              \
                                           \
        fnc ( moveLeft )                   \
        fnc ( moveRight )                  \
        fnc ( moveUp )                     \
        fnc ( moveDown )                   \
                                           \
        fnc ( transIn )                    \
        fnc ( transOut )                   \
                                           \
        fnc ( soundLevel )                 \
        fnc ( balanceLevel )               \
        fnc ( trebleLevel )                \
        fnc ( bassLevel )                  \
                                           \
        fnc ( backgroundColor )            \
        fnc ( visible )                    \
        fnc ( opacity )                    \
        fnc ( fit )                        \
        fnc ( fill )                       \
        fnc ( hidden )                     \
        fnc ( meet )                       \
        fnc ( meetBest )                   \
        fnc ( slice )                      \
                                           \
        fnc ( scroll )                     \
        fnc ( fontStyle )                  \
        fnc ( fontColor )                  \
        fnc ( fontFamily )                 \
        fnc ( fontSize )                   \
        fnc ( fontVariant )                \
        fnc ( fontWeight )                 \
                                           \
        fnc ( reusePlayer )                \
        fnc ( playerLife )                 \
                                           \
        fnc ( type  )                      \
        fnc ( subtype )                    \
        fnc ( direction )                  \
        fnc ( dur )                        \
        fnc ( startProgress )              \
        fnc ( endProgress )                \
        fnc ( fadeColor )                  \
        fnc ( horRepeat )                  \
        fnc ( vertRepeat )                 \
        fnc ( borderWidth )                \
        fnc ( borderColor )                \
                                           \
        fnc ( leftToRight )                \
        fnc ( topToBootom )                \
        fnc ( rectangle )                  \
        fnc ( diamond )                    \
        fnc ( clockwiseTwelve )            \
        fnc ( clockwiseThree )             \
        fnc ( clockwiseSix )               \
        fnc ( clockwiseNine )              \
        fnc ( topLeftHorizontal )          \
        fnc ( topLeftVertical )            \
        fnc ( topLeftDiagonal )            \
        fnc ( topRightDiagonal )           \
        fnc ( bottomRightDiagonal )        \
        fnc ( bottomLeftDiagonal )         \
        fnc ( crossfade )                  \
        fnc ( fadeToColor )                \
        fnc ( fadeFromColor )              \
        fnc ( src )                        \
        fnc ( descriptor )                 \
        fnc ( referInstance )              \
                                           \
        fnc ( plan )                       \
        fnc ( style )                      \
        fnc ( refer )                      \
                                           \
        fnc ( component )                  \
        fnc ( interface )                  \
                                           \
        fnc ( presentationEvent )          \
        fnc ( selectionEvent )             \
        fnc ( attributionEvent )

namespace player {
namespace property {
namespace type {

/**
* @brief Enumeration for properties identification.
*/
#define DO_ENUM_PROPERTY( p ) p,
enum Type {
	null = 0,
	PROPERTY_LIST(DO_ENUM_PROPERTY)
	LAST
};
#undef DO_ENUM_PROPERTY

};

const char *getPropertyName( type::Type pValue );
type::Type getProperty( const char *propertyName );

}
}

