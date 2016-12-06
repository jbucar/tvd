/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/


/*

SDL_rotozoom - rotozoomer

LGPL (c) A. Schiffler

*/

#ifndef _SDL_rotozoom_h
#define _SDL_rotozoom_h

#include <math.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
#define M_PI	3.141592654
#endif

#include <SDL/SDL.h>

	/* ---- Defines */

	/*!
	\cond PRIVATE
	\brief Disable anti-aliasing (no smoothing).
	*/
#define SMOOTHING_OFF		0

	/*!
	\brief Enable anti-aliasing (smoothing).
	*/
#define SMOOTHING_ON		1

	/* ---- Function Prototypes */

#if defined(WIN32) || defined(WIN64)
#  if defined(DLL_EXPORT) && !defined(LIBSDL_GFX_DLL_IMPORT)
#    define SDL_ROTOZOOM_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL_GFX_DLL_IMPORT
#      define SDL_ROTOZOOM_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL_ROTOZOOM_SCOPE
#  define SDL_ROTOZOOM_SCOPE extern
#endif

	/* 

	Rotozoom functions

	*/

	SDL_ROTOZOOM_SCOPE SDL_Surface *rotozoomSurface(SDL_Surface * src, double angle, double zoom, int smooth);

	SDL_ROTOZOOM_SCOPE SDL_Surface *rotozoomSurfaceXY
		(SDL_Surface * src, double angle, double zoomx, double zoomy, int smooth);


	SDL_ROTOZOOM_SCOPE void rotozoomSurfaceSize(int width, int height, double angle, double zoom, int *dstwidth,
		int *dstheight);

	SDL_ROTOZOOM_SCOPE void rotozoomSurfaceSizeXY
		(int width, int height, double angle, double zoomx, double zoomy, 
		int *dstwidth, int *dstheight);

	/* 

	Zooming functions

	*/

	SDL_ROTOZOOM_SCOPE SDL_Surface *zoomSurface(SDL_Surface * src, double zoomx, double zoomy, int smooth);

	SDL_ROTOZOOM_SCOPE void zoomSurfaceSize(int width, int height, double zoomx, double zoomy, int *dstwidth, int *dstheight);

	/* 

	Shrinking functions

	*/     

	SDL_ROTOZOOM_SCOPE SDL_Surface *shrinkSurface(SDL_Surface * src, int factorx, int factory);

	/* 

	Specialized rotation functions

	*/

	SDL_ROTOZOOM_SCOPE SDL_Surface* rotateSurface90Degrees(SDL_Surface* src, int numClockwiseTurns);

	/* Ends C function definitions when using C++ */

	/*!
	\endcond
	*/
#ifdef __cplusplus
}
#endif

#endif				/* _SDL_rotozoom_h */
