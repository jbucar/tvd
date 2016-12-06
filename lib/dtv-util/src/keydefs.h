/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#ifndef KEYDEFS_H
#define KEYDEFS_H

#include <set>

#define UTIL_KEY_LIST(fnc) \
	fnc( number_0          , 0                 , NUMBER_0          ) \
	fnc( number_1          , 1                 , NUMBER_1          ) \
	fnc( number_2          , 2                 , NUMBER_2          ) \
	fnc( number_3          , 3                 , NUMBER_3          ) \
	fnc( number_4          , 4                 , NUMBER_4          ) \
	fnc( number_5          , 5                 , NUMBER_5          ) \
	fnc( number_6          , 6                 , NUMBER_6          ) \
	fnc( number_7          , 7                 , NUMBER_7          ) \
	fnc( number_8          , 8                 , NUMBER_8          ) \
	fnc( number_9          , 9                 , NUMBER_9          ) \
	fnc( small_a           , a                 , SMALL_A           ) \
	fnc( small_b           , b                 , SMALL_B           ) \
	fnc( small_c           , c                 , SMALL_C           ) \
	fnc( small_d           , d                 , SMALL_D           ) \
	fnc( small_e           , e                 , SMALL_E           ) \
	fnc( small_f           , f                 , SMALL_F           ) \
	fnc( small_g           , g                 , SMALL_G           ) \
	fnc( small_h           , h                 , SMALL_H           ) \
	fnc( small_i           , i                 , SMALL_I           ) \
	fnc( small_j           , j                 , SMALL_J           ) \
	fnc( small_k           , k                 , SMALL_K           ) \
	fnc( small_l           , l                 , SMALL_L           ) \
	fnc( small_m           , m                 , SMALL_M           ) \
	fnc( small_n           , n                 , SMALL_N           ) \
	fnc( small_o           , o                 , SMALL_O           ) \
	fnc( small_p           , p                 , SMALL_P           ) \
	fnc( small_q           , q                 , SMALL_Q           ) \
	fnc( small_r           , r                 , SMALL_R           ) \
	fnc( small_s           , s                 , SMALL_S           ) \
	fnc( small_t           , t                 , SMALL_T           ) \
	fnc( small_u           , u                 , SMALL_U           ) \
	fnc( small_v           , v                 , SMALL_V           ) \
	fnc( small_w           , w                 , SMALL_W           ) \
	fnc( small_x           , x                 , SMALL_X           ) \
	fnc( small_y           , y                 , SMALL_Y           ) \
	fnc( small_z           , z                 , SMALL_Z           ) \
	fnc( capital_a         , A                 , CAPITAL_A         ) \
	fnc( capital_b         , B                 , CAPITAL_B         ) \
	fnc( capital_c         , C                 , CAPITAL_C         ) \
	fnc( capital_d         , D                 , CAPITAL_D         ) \
	fnc( capital_e         , E                 , CAPITAL_E         ) \
	fnc( capital_f         , F                 , CAPITAL_F         ) \
	fnc( capital_g         , G                 , CAPITAL_G         ) \
	fnc( capital_h         , H                 , CAPITAL_H         ) \
	fnc( capital_i         , I                 , CAPITAL_I         ) \
	fnc( capital_j         , J                 , CAPITAL_J         ) \
	fnc( capital_k         , K                 , CAPITAL_K         ) \
	fnc( capital_l         , L                 , CAPITAL_L         ) \
	fnc( capital_m         , M                 , CAPITAL_M         ) \
	fnc( capital_n         , N                 , CAPITAL_N         ) \
	fnc( capital_o         , O                 , CAPITAL_O         ) \
	fnc( capital_p         , P                 , CAPITAL_P         ) \
	fnc( capital_q         , Q                 , CAPITAL_Q         ) \
	fnc( capital_r         , R                 , CAPITAL_R         ) \
	fnc( capital_s         , S                 , CAPITAL_S         ) \
	fnc( capital_t         , T                 , CAPITAL_T         ) \
	fnc( capital_u         , U                 , CAPITAL_U         ) \
	fnc( capital_v         , V                 , CAPITAL_V         ) \
	fnc( capital_w         , W                 , CAPITAL_W         ) \
	fnc( capital_x         , X                 , CAPITAL_X         ) \
	fnc( capital_y         , Y                 , CAPITAL_Y         ) \
	fnc( capital_z         , Z                 , CAPITAL_Z         ) \
	fnc( page_down         , PAGE_DOWN         , PAGE_DOWN         ) \
	fnc( page_up           , PAGE_UP           , PAGE_UP           ) \
	fnc( f1                , F1                , F1                ) \
	fnc( f2                , F2                , F2                ) \
	fnc( f3                , F3                , F3                ) \
	fnc( f4                , F4                , F4                ) \
	fnc( f5                , F5                , F5                ) \
	fnc( f6                , F6                , F6                ) \
	fnc( f7                , F7                , F7                ) \
	fnc( f8                , F8                , F8                ) \
	fnc( f9                , F9                , F9                ) \
	fnc( f10               , F10               , F10               ) \
	fnc( f11               , F11               , F11               ) \
	fnc( f12               , F12               , F12               ) \
	fnc( plus_sign         , PLUS_SIGN         , PLUS_SIGN         ) \
	fnc( minus_sign        , MINUS_SIGN        , MINUS_SIGN        ) \
	fnc( asterisk          , ASTERISK          , ASTERISK          ) \
	fnc( number_sign       , NUMBER_SIGN       , NUMBER_SIGN       ) \
	fnc( period            , PERIOD            , PERIOD            ) \
	fnc( super             , SUPER             , SUPER             ) \
	fnc( printscreen       , PRINTSCREEN       , PRINTSCREEN       ) \
	fnc( menu              , MENU              , MENU              ) \
	fnc( info              , INFO              , INFO              ) \
	fnc( epg               , EPG               , EPG               ) \
	fnc( cursor_down       , CURSOR_DOWN       , CURSOR_DOWN       ) \
	fnc( cursor_left       , CURSOR_LEFT       , CURSOR_LEFT       ) \
	fnc( cursor_right      , CURSOR_RIGHT      , CURSOR_RIGHT      ) \
	fnc( cursor_up         , CURSOR_UP         , CURSOR_UP         ) \
	fnc( channel_down      , CHANNEL_DOWN      , CHANNEL_DOWN      ) \
	fnc( channel_up        , CHANNEL_UP        , CHANNEL_UP        ) \
	fnc( volume_down       , VOLUME_DOWN       , VOLUME_DOWN       ) \
	fnc( volume_up         , VOLUME_UP         , VOLUME_UP         ) \
	fnc( enter             , ENTER             , ENTER             ) \
	fnc( ok                , OK                , OK                ) \
	fnc( red               , RED               , RED               ) \
	fnc( green             , GREEN             , GREEN             ) \
	fnc( yellow            , YELLOW            , YELLOW            ) \
	fnc( blue              , BLUE              , BLUE              ) \
	fnc( backspace         , BACKSPACE         , BACKSPACE         ) \
	fnc( back              , BACK              , BACK              ) \
	fnc( escape            , ESCAPE            , ESCAPE            ) \
	fnc( exit              , EXIT              , EXIT              ) \
	fnc( power             , POWER             , POWER             ) \
	fnc( rewind            , REWIND            , REWIND            ) \
	fnc( forward           , FORWARD           , FORWARD           ) \
	fnc( stop              , STOP              , STOP              ) \
	fnc( eject             , EJECT             , EJECT             ) \
	fnc( play              , PLAY              , PLAY              ) \
	fnc( record            , RECORD            , RECORD            ) \
	fnc( pause             , PAUSE             , PAUSE             ) \
	fnc( greater_than_sign , GREATER_THAN_SIGN , GREATER_THAN_SIGN ) \
	fnc( less_than_sign    , LESS_THAN_SIGN    , LESS_THAN_SIGN    ) \
	fnc( tap               , TAP               , TAP               ) \
	fnc( mute              , MUTE              , MUTE              ) \
	fnc( pvr               , PVR               , PVR               ) \
	fnc( audio             , AUDIO             , AUDIO             ) \
	fnc( previous          , PREVIOUS          , PREVIOUS          ) \
	fnc( favorites         , FAVORITES         , FAVORITES         ) \
	fnc( subtitle          , SUBTITLE          , SUBTITLE          ) \
	fnc( text              , TEXT              , TEXT              ) \
	fnc( aspect            , ASPECT            , ASPECT            ) \
	fnc( ch34              , CH34              , CH34              ) \
	fnc( mode              , MODE              , MODE              ) \
	fnc( nav               , NAV               , NAV               ) \
	fnc( chlist            , CHLIST            , CHLIST            )


namespace util {
namespace key {

/**
* @brief Enumeration for key identification.
*/

#define DO_ENUM_KEY( c, n, u ) c,
enum type {
	null = 0,
	UTIL_KEY_LIST(DO_ENUM_KEY)
	LAST
};
#undef DO_ENUM_KEY

typedef std::set<type> Keys;

//	Returns key name
const char *getKeyName( type kValue );
type getKey( const char *keyName );

}
}

#endif /* KEYDEFS_H */

