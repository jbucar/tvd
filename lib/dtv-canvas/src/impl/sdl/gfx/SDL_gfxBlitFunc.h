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

SDL_gfxBlitFunc: custom blitters (part of SDL_gfx library)

LGPL (c) A. Schiffler

*/

#ifndef _SDL_gfxBlitFunc_h
#define _SDL_gfxBlitFunc_h

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern    "C" {
#endif

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_video.h>

	/* ---- Function Prototypes */

#if defined(WIN32) || defined(WIN64)
#  if defined(DLL_EXPORT) && !defined(LIBSDL_GFX_DLL_IMPORT)
#    define SDL_GFXBLITFUNC_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL_GFX_DLL_IMPORT
#      define SDL_GFXBLITFUNC_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL_GFXBLITFUNC_SCOPE
#  define SDL_GFXBLITFUNC_SCOPE extern
#endif


	SDL_GFXBLITFUNC_SCOPE int SDL_gfxBlitRGBA(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);

	SDL_GFXBLITFUNC_SCOPE int SDL_gfxSetAlpha(SDL_Surface * src, Uint8 a);

	SDL_GFXBLITFUNC_SCOPE int SDL_gfxMultiplyAlpha(SDL_Surface * src, Uint8 a);

	/* -------- Macros */

	/* Define SDL macros locally as a substitute for an #include "SDL_blit.h", */
	/* which doesn't work since the include file doesn't get installed.       */

/*!
\cond PRIVATE
\brief The structure passed to the low level blit functions.
*/
	typedef struct {
		Uint8    *s_pixels;
		int       s_width;
		int       s_height;
		int       s_skip;
		Uint8    *d_pixels;
		int       d_width;
		int       d_height;
		int       d_skip;
		void     *aux_data;
		SDL_PixelFormat *src;
		Uint8    *table;
		SDL_PixelFormat *dst;
	} SDL_gfxBlitInfo;

/*!
\brief Unwrap RGBA values from a pixel using mask, shift and loss for surface.
*/
#define GFX_RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a)				\
	{									\
	r = ((pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss; 		\
	g = ((pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss; 		\
	b = ((pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss; 		\
	a = ((pixel&fmt->Amask)>>fmt->Ashift)<<fmt->Aloss;	 	\
	}

/*!
\brief Disassemble buffer pointer into a pixel and separate RGBA values.
*/
#define GFX_DISEMBLE_RGBA(buf, bpp, fmt, pixel, r, g, b, a)			   \
	do {									   \
	pixel = *((Uint32 *)(buf));			   		   \
	GFX_RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a);			   \
	pixel &= ~fmt->Amask;						   \
	} while(0)

/*!
\brief Wrap a pixel from RGBA values using mask, shift and loss for surface.
*/
#define GFX_PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a)				\
	{									\
	pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|				\
	((g>>fmt->Gloss)<<fmt->Gshift)|				\
	((b>>fmt->Bloss)<<fmt->Bshift)|				\
	((a<<fmt->Aloss)<<fmt->Ashift);				\
	}

/*!
\brief Assemble pixel into buffer pointer from separate RGBA values.
*/
#define GFX_ASSEMBLE_RGBA(buf, bpp, fmt, r, g, b, a)			\
	{									\
	Uint32 pixel;					\
	\
	GFX_PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a);	\
	*((Uint32 *)(buf)) = pixel;			\
	}

/*!
\brief Blend the RGB values of two pixels based on a source alpha value.
*/
#define GFX_ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB)	\
	do {						\
	dR = (((sR-dR)*(A))/255)+dR;		\
	dG = (((sG-dG)*(A))/255)+dG;		\
	dB = (((sB-dB)*(A))/255)+dB;		\
	} while(0)

/*!
\brief 4-times unrolled DUFFs loop.

This is a very useful loop for optimizing blitters.
*/
#define GFX_DUFFS_LOOP4(pixel_copy_increment, width)			\
	{ int n = (width+3)/4;							\
	switch (width & 3) {						\
	case 0: do {	pixel_copy_increment;				\
	case 3:		pixel_copy_increment;				\
	case 2:		pixel_copy_increment;				\
	case 1:		pixel_copy_increment;				\
	} while ( --n > 0 );					\
	}								\
	}



	/* Ends C function definitions when using C++ */
/*!
\endcond
*/
#ifdef __cplusplus
}
#endif

#endif /* _SDL_gfxBlitFunc_h */
