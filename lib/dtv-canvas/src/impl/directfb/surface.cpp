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

#include "surface.h"
#include "canvas.h"
#include "dfb.h"
#include "font.h"
#include "glyphrun.h"
#include "generated/config.h"
#include <util/tool.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/fs.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/algorithm/string.hpp>
#include <png.h>
#include <zlib.h>
#include <map>
#include <algorithm>

#define DFB_DEL_SURFACE( s ) s->ReleaseSource(s); DFB_DEL( s )

namespace bfs = boost::filesystem;

namespace canvas {
namespace directfb {

namespace impl {

/* Converts native endian ARGB => RGBA bytes */
static void convert_data( png_structp /*png*/, png_row_infop row_info, png_bytep data ) {
    unsigned int i;
    for (i=0; i < row_info->rowbytes; i+=4) {
		uint8_t *b = &data[i];
        uint32_t pixel;

		memcpy( &pixel, b, sizeof(uint32_t) );
		b[3] = (uint8_t)((pixel & 0xff000000) >> 24);
		double a = 255.0 / ((b[3]>0)?(double)b[3]:1.0);
		b[0] = (uint8_t) boost::math::iround( ((pixel & 0x00ff0000) >> 16) * a );
		b[1] = (uint8_t) boost::math::iround( ((pixel & 0x0000ff00) >> 8) * a );
		b[2] = (uint8_t) boost::math::iround( (pixel & 0x000000ff) * a );
    }
}

static DFBSurfacePorterDuffRule getDFBCompositionMode( composition::mode mode ) {
	switch (mode) {
		case composition::source_over: return DSPD_SRC_OVER;
		case composition::source: return DSPD_SRC;
		case composition::clear: return DSPD_CLEAR;
		default: throw std::runtime_error("[canvas::directfb::Surface] composition::mode not suported!!!");
	}
}

static inline void dfb_init_surface_desc( DFBSurfaceDescription *dsc, int w, int h ) {
	dsc->flags = (DFBSurfaceDescriptionFlags) (DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS);
	dsc->caps = DSCAPS_PREMULTIPLIED;
	dsc->width = w;
	dsc->height = h;
	dsc->pixelformat = getFormat();
}

static inline void dfb_set_blit_flags( IDirectFBSurface *surface, DFBSurfaceBlittingFlags extraFlags ) {
	DFBResult err = surface->SetBlittingFlags( surface,
		(DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | extraFlags)
	);
	DFB_CHECK_THROW( "dfb_set_blit_flags fails" );
}

static inline void dfb_set_operator( IDirectFBSurface *surface, DFBSurfacePorterDuffRule dfbOperator ) {
	DFBResult err = surface->SetPorterDuff(surface, dfbOperator);
	DFB_CHECK_THROW( "dfb_set_operator fails" );
}

static inline void dfb_create_surface( IDirectFB *dfb, IDirectFBSurface **surface, DFBSurfaceDescription *dsc ) {
	DFBResult err = dfb->CreateSurface(dfb, dsc, surface);
	DFB_CHECK_THROW( "dfb_create_surface fails" );

	dfb_set_blit_flags(*surface, (DFBSurfaceBlittingFlags)0);

	err = (*surface)->SetDrawingFlags( (*surface), (DFBSurfaceDrawingFlags)(DSDRAW_BLEND) );
	DFB_CHECK_THROW( "dfb_create_surface fails" );

	dfb_set_operator(*surface, DSPD_SRC_OVER);
}

}

Surface::Surface( Canvas *canvas, ImageData *img )
	: canvas::Surface( Point(0,0) ), _canvas(canvas), _size( img->size.w, img->size.h )
{
	DFBSurfaceDescription dsc;
	int stride = img->size.w * 4;

	if (img->length < (img->size.h*stride)) {
		throw std::runtime_error( "Cannot create directfb surface/context!" );
	}

	impl::dfb_init_surface_desc( &dsc, img->size.w, img->size.h );
	dsc.flags = (DFBSurfaceDescriptionFlags)(dsc.flags | DSDESC_PREALLOCATED);
	dsc.preallocated[0].data = img->data;
	dsc.preallocated[0].pitch = stride;

	impl::dfb_create_surface(getDFB(), &_surface, &dsc);
	init();
}

Surface::Surface( Canvas *canvas, const Rect &rect, bool primary )
	: canvas::Surface( Point(rect.x,rect.y) ), _canvas(canvas), _size(rect.w, rect.h)
{
	if (rect.w<=0 || rect.h<=0 ) {
		throw std::runtime_error("[canvas::directfb::Surface] Cannot create surface!!! Invalid bounds");
	}

	DFBSurfaceDescription dsc;
	if (primary) {
		dsc.flags = (DFBSurfaceDescriptionFlags) ( DSDESC_CAPS | DSDESC_HINTS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT );
		dsc.caps = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY | DSCAPS_VIDEOONLY | DSCAPS_DOUBLE | DSCAPS_PREMULTIPLIED);
		dsc.hints = DSHF_LAYER;
		dsc.width = _size.w;
		dsc.height = _size.h;
		dsc.pixelformat = getFormat();
	} else {
		impl::dfb_init_surface_desc( &dsc, rect.w, rect.h );
	}
	impl::dfb_create_surface(getDFB(), &_surface, &dsc);
	_surface->Clear( _surface, 0, 0, 0, 0);
	init();
}

Surface::Surface( Canvas *canvas, const std::string &file )
	: canvas::Surface( Point(0,0) ), _canvas(canvas)
{
	DFBSurfaceDescription dsc;
	IDirectFBImageProvider *provider;

	DFBResult err = getDFB()->CreateImageProvider(getDFB(), file.c_str(), &provider);
	DFB_CHECK_THROW( "CreateImageProvider" );
	
	provider->GetSurfaceDescription(provider, &dsc);
	_size.w = dsc.width;
	_size.h = dsc.height;

	dsc.flags = (DFBSurfaceDescriptionFlags) ( dsc.flags | DSDESC_PIXELFORMAT );
	dsc.pixelformat = getFormat();

	impl::dfb_create_surface(getDFB(), &_surface, &dsc);

	_surface->Clear( _surface, 0, 0, 0, 0 );
	provider->RenderTo(provider, _surface, NULL);

	_surface->SetColor( _surface, 0, 0, 0, 255 );
	impl::dfb_set_blit_flags(_surface, DSBLIT_SRC_PREMULTIPLY);
	_surface->SetPorterDuff(_surface, DSPD_SRC);

	_surface->Blit(_surface, _surface, NULL, 0, 0);

	_surface->SetColor( _surface, 0, 0, 0, 0 );
	impl::dfb_set_blit_flags(_surface, (DFBSurfaceBlittingFlags) 0);
	_surface->SetPorterDuff(_surface, DSPD_SRC_OVER);

	DFB_DEL(provider);

	init();
}

void Surface::init() {
	_alpha = 255;
	setCompositionModeImpl( getCompositionMode() );
}

Surface::~Surface()
{
	DFB_DEL_SURFACE(_surface);
}

canvas::Canvas *Surface::canvas() const {
	return _canvas;
}

Size Surface::getSize() const {
	return _size;
}

bool Surface::getClip( Rect &rect ) {
	DFBRegion r;

	_surface->GetClip(_surface, &r);
	rect.x = r.x1;
	rect.y = r.y1;
	rect.w = (r.x2 - r.x1) + 1;
	rect.h = (r.y2 - r.y1) + 1;

	return true;
}

bool Surface::setOpacity( util::BYTE alpha ) {
	_alpha = alpha;
	markDirty();
	return true;
}

util::BYTE Surface::getOpacity() const {
	return _alpha;
}

bool Surface::saveAsImage( const std::string &file ) {
	unsigned char *buffer;
	int pitch;
	png_bytep *row_pointers;
	png_structp png_ptr;
	png_infop info_ptr;
	png_text txt_ptr[4];
	int i;
	int bit_depth=0, color_type;
	const Size curSize=getSize();

	DFBResult err=_surface->Lock( _surface, DSLF_READ, (void **)&buffer, &pitch );
	DFB_CHECK( "Lock" );
	
	FILE *fd = fopen(file.c_str(), "wb");
	if (!fd) {
		_surface->Unlock( _surface );
		LERROR("directfb::Surface", "couldn't open file: file=%s", file.c_str());
		return false;
	}
		
	png_ptr = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, 
		(png_voidp)NULL,
		(png_error_ptr)NULL,
		(png_error_ptr)NULL
	);
	if (!png_ptr) {
		_surface->Unlock( _surface );
		LERROR("directfb::Surface", "couldn't create PNG write struct.");
		return false;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		_surface->Unlock( _surface );
		LERROR("directfb::Surface", "couldn't create PNG info struct.");
		return false;
	}

	txt_ptr[0].key=(char *)"Name";
	txt_ptr[0].text=(char *)"canvas::directfb::Surface screenshot";
	txt_ptr[0].compression=PNG_TEXT_COMPRESSION_NONE;
	txt_ptr[1].key=(char *)"Date";
	txt_ptr[1].text=(char *)"Current Date";
	txt_ptr[1].compression=PNG_TEXT_COMPRESSION_NONE;
	txt_ptr[2].key=(char *)"Hostname";
	txt_ptr[2].text=(char *)"way";
	txt_ptr[2].compression=PNG_TEXT_COMPRESSION_NONE;
	txt_ptr[3].key=(char *)"Program";
	txt_ptr[3].text=(char *)"v.1.0";
	txt_ptr[3].compression=PNG_TEXT_COMPRESSION_NONE;

	png_set_text(png_ptr, info_ptr, txt_ptr, 4);
	png_init_io(png_ptr, fd);   
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	row_pointers=(png_bytep*)malloc(sizeof(png_bytep)*curSize.h);
	bit_depth=8;
	color_type=PNG_COLOR_TYPE_RGB_ALPHA;
	for (i=0; i<curSize.h; i++) {
		row_pointers[i]=buffer+i*4*curSize.w;
	}

	png_set_IHDR(
		png_ptr,
		info_ptr,
		curSize.w,
		curSize.h, 
		bit_depth,
		color_type,
		PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	png_set_write_user_transform_fn (png_ptr, impl::convert_data);
	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	free(row_pointers);
	fclose(fd);
	_surface->Unlock( _surface );
	return true;
}

bool Surface::hackDejaVuFont() const {
	return false;
}

void Surface::blitImpl( const Point &target, canvas::Surface *srcSurface, const Rect &source ) {
	Surface *src=dynamic_cast<Surface *>(srcSurface);
	_surface->SetColor( _surface, 0, 0, 0, src->getOpacity() );
	DFBRectangle rect = {source.x, source.y, source.w, source.h};
	_surface->Blit(_surface, src->getContent(), &rect, target.x, target.y);
	setColor( getColor() );
}

void Surface::scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	Surface *src = NULL;
	Rect r(Point(0,0),srcSurface->getSize());
	if (r != sourceRect) {
		canvas::Surface *tmp = canvas()->createSurface(Size(sourceRect.w, sourceRect.h));
		tmp->setOpacity(srcSurface->getOpacity());
		tmp->blit(Point(0,0), srcSurface, sourceRect);
		src = dynamic_cast<Surface *>(tmp);
	} else {
		src = dynamic_cast<Surface *>(srcSurface);
	}
	IDirectFBSurface *tmp1;
	IDirectFBSurface *tmp2;
	DFBSurfaceDescription dsc;
	impl::dfb_init_surface_desc( &dsc, sourceRect.w, sourceRect.h );
	impl::dfb_create_surface(getDFB(), &tmp1, &dsc);
	tmp1->Clear( tmp1, 0, 0, 0, 0);

	tmp1->SetColor( tmp1, 0, 0, 0, 255 );
	tmp1->Blit(tmp1, src->getContent(), NULL, 0, 0);
	if (flipw) {
		impl::dfb_create_surface(getDFB(), &tmp2, &dsc);
		tmp2->Clear( tmp2, 0, 0, 0, 0);
		impl::dfb_set_blit_flags(tmp2, DSBLIT_FLIP_HORIZONTAL);
		tmp2->SetColor( tmp2, 0, 0, 0, 255 );
		tmp2->Blit(tmp2, tmp1, NULL, 0, 0);
		DFB_DEL_SURFACE(tmp1);
		tmp1 = tmp2;
		tmp2 = NULL;
	}
	if (fliph) {
		impl::dfb_create_surface(getDFB(), &tmp2, &dsc);
		tmp2->Clear( tmp2, 0, 0, 0, 0);
		impl::dfb_set_blit_flags(tmp2, DSBLIT_FLIP_VERTICAL);
		tmp2->SetColor( tmp2, 0, 0, 0, 255 );
		tmp2->Blit(tmp2, tmp1, NULL, 0, 0);
		DFB_DEL_SURFACE(tmp1);
		tmp1 = tmp2;
		tmp2 = NULL;
	}

	_surface->SetColor( _surface, 0, 0, 0, src->getOpacity() );
	DFBRectangle destRect = {targetRect.x, targetRect.y, targetRect.w, targetRect.h};
	DFBResult err = _surface->StretchBlit(_surface, tmp1, NULL, &destRect);
	DFB_CHECK_WARN( "blitImpl fails" );
	DFB_DEL_SURFACE(tmp1);
	if (r != sourceRect) {
		canvas::Surface *tmp = dynamic_cast<canvas::Surface *>(src);
		canvas()->destroy( tmp );
	}
	setColor( getColor() );
}

canvas::Surface *Surface::rotateImpl( int degrees ) {
	canvas::Size size = getSize();
	canvas::Point pos = getLocation();
	canvas::Rect tmpRect( 0, 0, std::max(size.h,size.w), std::max(size.h,size.w) );
	canvas::Surface *tmpSurface = createSimilar( tmpRect );
	DFBSurfaceBlittingFlags flags = DSBLIT_NOFX;

	if (degrees==90) {
		flags = DSBLIT_ROTATE270;
	} else if (degrees==180) {
		flags = DSBLIT_ROTATE180;
	} else if (degrees==270) {
		flags = DSBLIT_ROTATE90;
	}

	IDirectFBSurface *srf = dynamic_cast<Surface *>(tmpSurface)->getContent();
	impl::dfb_set_blit_flags( srf, flags );
	srf->SetColor( srf, 0, 0, 0, 255 );
	DFBResult err = srf->Blit(srf, getContent(), NULL, 0, 0);
	DFB_CHECK_WARN( "rotateImpl fails" );
	impl::dfb_set_blit_flags( srf, DSBLIT_NOFX );

	canvas::Rect rect;
	if (degrees==90 || degrees==270) {
		rect = canvas::Rect(pos.x,pos.y,size.h,size.w);
	} else {
		rect = canvas::Rect(pos.x,pos.y,size.w,size.h);
	}
	canvas::Surface *newSurface = createSimilar( rect );
	rect.x = 0;
	rect.y = 0;
	newSurface->blit( canvas::Point(0,0), tmpSurface, rect);
	_canvas->destroy( tmpSurface );
	return newSurface;
}

void Surface::setClipImpl( const Rect &rect ) {
	DFBRegion clip = {rect.x, rect.y, rect.x + rect.w - 1, rect.y + rect.h - 1};
	DFBResult err = _surface->SetClip( _surface, &clip );
	DFB_CHECK_WARN("SetClip fails");
}

void Surface::setColorImpl( Color &color ) {
	double a = ((color.alpha<1)?1.0:(double)color.alpha) / 255.0;
	_surface->SetColor(_surface,
			(u8) boost::math::iround(color.r * a),
			(u8) boost::math::iround(color.g * a),
			(u8) boost::math::iround(color.b * a),
			(u8) ((color.alpha<1)?1:color.alpha));
}

void Surface::drawLineImpl( int x1, int y1, int x2, int y2 ) {
	DFBResult err = _surface->DrawLine(_surface, x1, y1, x2, y2);
	DFB_CHECK_WARN("DrawLine fails");
}

void Surface::drawRectImpl( const Rect &rect ) {
	DFBResult err = _surface->DrawRectangle(_surface, rect.x, rect.y, rect.w, rect.h);
	DFB_CHECK_WARN("DrawRectangle fails");
}

void Surface::fillRectImpl( const Rect &rect ) {
	_surface->FillRectangle(_surface, rect.x, rect.y, rect.w, rect.h);
}

void Surface::drawArc(bool fill, const Point center, const Size size, float start, float end) {
	int two_a_square = 2 * boost::math::iround(pow(size.w,2));
	int two_b_square = 2 * boost::math::iround(pow(size.h,2));
	int x = size.w;
	int y = 0;
	int x_change = boost::math::iround(pow(size.h, 2)) * (1 - 2 * size.w);
	int y_change = boost::math::iround(pow(size.w, 2));
	int ellipse_error = 0;
	int stopping_x = two_b_square * size.w;
	int stopping_y = 0;
	std::vector<Point> drawnPoints;

	while (stopping_x >= stopping_y) {
		if(fill){
			fillArcPoints(Point(x,y), center, start, end, drawnPoints);
		} else {
			plotArcPoints(Point(x,y), center, start, end, drawnPoints);
		}
		y++;
		stopping_y += two_a_square;
		ellipse_error += y_change;
		y_change += two_a_square;
		if ((2*ellipse_error + x_change) > 0) {
			x--;
			stopping_x -= two_b_square;
			ellipse_error += x_change;
			x_change += two_b_square;
		}
	}
	x = 0;
	y = size.h;
	x_change = boost::math::iround(pow(size.h,2));
	y_change = boost::math::iround(pow(size.w,2)) * (1 - 2*size.h);
	ellipse_error = 0;
	stopping_x = 0;
	stopping_y = two_a_square * size.h;
	while (stopping_x <= stopping_y){
		if (fill) {
			fillArcPoints(Point(x,y), center, start, end, drawnPoints);
		} else {
			plotArcPoints(Point(x,y), center, start, end, drawnPoints);
		}
		x++;
		stopping_x += two_b_square;
		ellipse_error += x_change;
		x_change += two_b_square;
		if ((2*ellipse_error + y_change) > 0){
			y--;
			stopping_y -= two_a_square;
			ellipse_error += y_change;
			y_change += two_a_square;
		}
	}
}

void Surface::plotArcPoints( const Point pos, const Point center, float start, float end, std::vector<Point> &drawnPoints ) {
	double startd = start * M_PI / 180;
	double endd   = end   * M_PI / 180;
	double angle;
	if (pos.x != 0 || pos.y != 0) {
		angle = atan2(pos.y,pos.x);
		if (startd <= angle && angle <= endd) {
			drawPixel(Point(center.x+pos.x, center.y-pos.y), drawnPoints);
		}
		angle = 2 * M_PI + atan2(-pos.y,pos.x);
		if (startd <= angle && angle <= endd) {
			drawPixel(Point(center.x+pos.x, center.y+pos.y), drawnPoints);
		}
		angle = atan2(pos.y,-pos.x);
		if (startd <= angle && angle <= endd) {
			drawPixel(Point(center.x-pos.x, center.y-pos.y), drawnPoints);
		}
		angle = 2 * M_PI + atan2(-pos.y,-pos.x);
		if (startd <= angle && angle <= endd) {
			drawPixel(Point(center.x-pos.x, center.y+pos.y), drawnPoints);
		}
	}
}

void Surface::fillArcPoints( const Point pos, const Point center, float start, float end, std::vector<Point> &drawnPoints ) {
	double startd = start * M_PI / 180.0;
	double endd = end   * M_PI / 180.0;
	double angle;
	for (int x_i = -abs(pos.x); x_i < abs(pos.x); ++x_i) {
		angle = atan2(pos.y,x_i);
		if (startd <= angle && angle <= endd) {
			drawPixel(Point(center.x+x_i, center.y-pos.y), drawnPoints);
		}
		angle = 2.0f * M_PI + atan2(-pos.y,x_i);
		if (startd <= angle && angle <= endd) {
			drawPixel(Point(center.x+x_i, center.y+pos.y), drawnPoints);
		}
	}
}

void Surface::drawRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	int radius = (int) ceil((arcW + arcH) / 2);
	if (radius >= (rect.w / 2)) {
		radius = rect.w / 2 - 1;
	}
	if (radius >= (rect.h/ 2)) {
		radius = rect.h / 2 - 1;
	}

	Size size(radius,radius);
	int minRectX = rect.x + radius;
	int maxRectX = rect.x + rect.w - radius - 1;
	int minRectY = rect.y + radius;
	int maxRectY = rect.y + rect.h - radius - 1;

	Point center1(maxRectX, minRectY);
	Point center2(maxRectX, maxRectY);
	Point center3(minRectX, maxRectY);
	Point center4(minRectX, minRectY);

	_surface->DrawLine(_surface, minRectX+1, rect.y, maxRectX-1, rect.y);
	_surface->DrawLine(_surface, rect.x+rect.w-1, minRectY+1, rect.x+rect.w-1, maxRectY-1);
	_surface->DrawLine(_surface, minRectX+1, rect.y+rect.h-1, maxRectX-1, rect.y+rect.h-1);
	_surface->DrawLine(_surface, rect.x, minRectY+1, rect.x, maxRectY-1);
	if (radius>0) {
		drawArc(false, center1, size, 0, 90 );
		drawArc(false, center2, size, 270, 360 );
		drawArc(false, center3, size, 180, 270 );
		drawArc(false, center4, size, 90, 180 );
	}
}

void Surface::fillRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	int radius = (int) ceil((arcW + arcH) / 2);
	if (radius >= (rect.w / 2)) {
		radius = rect.w / 2 - 1;
	}
	if (radius >= (rect.h/ 2)) {
		radius = rect.h / 2 - 1;
	}

	Size size(radius,radius);
	int minRectX = rect.x + radius;
	int maxRectX = rect.x + rect.w - radius - 1;
	int minRectY = rect.y + radius;
	int maxRectY = rect.y + rect.h - radius - 1;

	Point center1(maxRectX+1, minRectY);
	Point center2(maxRectX+1, maxRectY);
	Point center3(minRectX, maxRectY);
	Point center4(minRectX, minRectY);
	_surface->FillRectangle(_surface, minRectX+1, rect.y, maxRectX-minRectX, rect.h);
	if (radius>0) {
		_surface->FillRectangle(_surface, rect.x, minRectY+1, radius+1, maxRectY-minRectY-1);
		_surface->FillRectangle(_surface, maxRectX+1, minRectY+1, radius, maxRectY-minRectY-1);
		drawArc(true, center1, size, 0, 90 );
		drawArc(true, center2, size, 270, 360 );
		drawArc(true, center3, size, 180, 270 );
		_surface->FillRectangle(_surface, center3.x, center3.y, 1, 1);
		drawArc(true, center4, size, 90, 180 );
		_surface->FillRectangle(_surface, center4.x, center4.y, 1, 1);
	}
}

//     TODO: open polygon
void Surface::drawPolygonImpl( const std::vector<Point> &vertices, bool closed ) {
	std::vector<Point>::const_iterator it = vertices.begin();

	Point firstPoint = *it;
	Point lastPoint;
	it++;
	while (it != vertices.end()) {
		Point p = *(--it);
		lastPoint = *(++it);
		if (lastPoint.x>p.x) {
			p.x++;
		} else if(lastPoint.x<p.x) {
			p.x--;
		}
		if (lastPoint.y>p.y) {
			p.y++;
		} else if(lastPoint.y<p.y) {
			p.y--;
		}
		_surface->DrawLine(_surface, p.x, p.y, lastPoint.x, lastPoint.y);
		++it;
	}
	if (closed) {
		if (firstPoint.x>lastPoint.x) {
			lastPoint.x++;
		} else if(firstPoint.x<lastPoint.x) {
			lastPoint.x--;
		}
		if (firstPoint.y>lastPoint.y) {
			lastPoint.y++;
		} else if(firstPoint.y<lastPoint.y) {
			lastPoint.y--;
		}
		_surface->DrawLine(_surface, lastPoint.x, lastPoint.y, firstPoint.x, firstPoint.y);
	}
}

bool pnpoly(const std::vector<Point>& vertices, int testx, int testy) {
	int i;
	int j;
	bool c=false;
	int n = vertices.size();

	for (i=0, j=n-1; i<n; j=i++) {
		if (((vertices[i].y>testy)!=(vertices[j].y>testy)) &&
			(testx <= boost::math::iround(vertices[j].x-vertices[i].x) * (testy-vertices[i].y) / (vertices[j].y-vertices[i].y) + vertices[i].x)) {
			c=!c;
		}
	}
	return c;
}

void Surface::fillPolygonImpl( const std::vector<Point>& vertices ) {
	int minX=vertices[0].x;
	int maxX=vertices[0].x;
	int minY=vertices[0].y;
	int maxY=vertices[0].y;
	BOOST_FOREACH( Point p, vertices) {
		minX = std::min(minX, p.x);
		maxX = std::max(maxX, p.x);
		minY = std::min(minY, p.y);
		maxY = std::max(maxY, p.y);
	}
	for (int x=minX; x<=maxX; x++) {
		for (int y=minY; y<=maxY; y++) {
			if (pnpoly(vertices, x, y)) {
				_surface->DrawRectangle(_surface, x, y, 1, 1);
			}
		}
	}
}

void Surface::drawPixel(const Point &pos, std::vector<Point> &drawnPoints) {
	if (std::find(drawnPoints.begin(), drawnPoints.end(), pos) == drawnPoints.end()) {
		drawnPoints.push_back(pos);
		_surface->DrawRectangle(_surface, pos.x, pos.y, 1, 1);
	}
}

void Surface::plot4EllipsePoints(const Point &pos, const Point &center, std::vector<Point> &drawnPoints) {
	if (pos.x != 0 || pos.y != 0) {
		drawPixel(Point(center.x+pos.x, center.y-pos.y), drawnPoints);
		drawPixel(Point(center.x+pos.x, center.y+pos.y), drawnPoints);
		drawPixel(Point(center.x-pos.x, center.y-pos.y), drawnPoints);
		drawPixel(Point(center.x-pos.x, center.y+pos.y), drawnPoints);
	}
}

void Surface::drawEllipseImpl( const Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	int two_a_square = 2 * boost::math::iround(pow(rw,2));
	int two_b_square = 2 * boost::math::iround(pow(rh,2));
	int x = rw;
	int y = 0;
	int x_change = boost::math::iround(pow(rh, 2) * (1 - 2 * rw));
	int y_change = boost::math::iround(pow(rw, 2));
	int ellipse_error = 0;
	int stopping_x = two_b_square * rw;
	int stopping_y = 0;
	std::vector<Point> drawnPoints;

	while (stopping_x >= stopping_y) {
		plot4EllipsePoints(Point(x,y), center, drawnPoints);
		y++;
		stopping_y += two_a_square;
		ellipse_error += y_change;
		y_change += two_a_square;
		if ((2*ellipse_error + x_change) > 0) {
			x--;
			stopping_x -= two_b_square;
			ellipse_error += x_change;
			x_change += two_b_square;
		}
	}
	x = 0;
	y = rh;
	x_change = boost::math::iround(pow(rh,2));
	y_change = boost::math::iround(pow(rw, 2) * (1 - 2*rh));
	ellipse_error = 0;
	stopping_x = 0;
	stopping_y = two_a_square * rh;
	while (stopping_x <= stopping_y){
		plot4EllipsePoints(Point(x,y), center, drawnPoints);
		x++;
		stopping_x += two_b_square;
		ellipse_error += x_change;
		x_change += two_b_square;
		if ((2*ellipse_error + y_change) > 0){
			y--;
			stopping_y -= two_a_square;
			ellipse_error += y_change;
			y_change += two_a_square;
		}
	}
}

void Surface::fillEllipseImpl( const Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	int two_a_square = 2 * boost::math::iround(pow(rw,2));
	int two_b_square = 2 * boost::math::iround(pow(rh,2));
	int x = rw;
	int y = 0;
	int x_change = boost::math::iround(pow(rh, 2) * (1 - 2 * rw));
	int y_change = boost::math::iround(pow(rw, 2));
	int ellipse_error = 0;
	int stopping_x = two_b_square * rw;
	int stopping_y = 0;
	std::map<int, int> lines;
	std::map<int, int>::iterator it;

	while (stopping_x >= stopping_y) {
		it = lines.find(y);
		if (it == lines.end()) {
			lines.insert(std::pair<int,int>(y,x));
		} else if (it->second > x) {
			lines.erase(y);
			lines.insert(std::pair<int,int>(y,x));
		}
		y++;
		stopping_y += two_a_square;
		ellipse_error += y_change;
		y_change += two_a_square;
		if ((2*ellipse_error + x_change) > 0) {
			x--;
			stopping_x -= two_b_square;
			ellipse_error += x_change;
			x_change += two_b_square;
		}
	}
	x = 0;
	y = rh;
	x_change = boost::math::iround(pow(rh,2));
	y_change = boost::math::iround(pow(rw, 2) * (1 - 2*rh));
	ellipse_error = 0;
	stopping_x = 0;
	stopping_y = two_a_square * rh;
	while (stopping_x <= stopping_y){
		it = lines.find(y);
		if (it == lines.end()) {
			lines.insert(std::pair<int,int>(y,x));
		} else if (it->second > x) {
			lines.erase(y);
			lines.insert(std::pair<int,int>(y,x));
		}
		x++;
		stopping_x += two_b_square;
		ellipse_error += x_change;
		x_change += two_b_square;
		if ((2*ellipse_error + y_change) > 0){
			y--;
			stopping_y -= two_a_square;
			ellipse_error += y_change;
			y_change += two_a_square;
		}
	}
	for (it=lines.begin(); it!=lines.end(); it++) {
		int px = it->second;
		int py = it->first;
		_surface->DrawLine(_surface, center.x-abs(px), center.y-py, center.x+abs(px), center.y-py);
		if( py > 0) {
			_surface->DrawLine(_surface, center.x-abs(px), center.y+py, center.x+abs(px), center.y+py);
		}
	}
}

IDirectFBSurface *Surface::getContent() {
	return _surface;
}

util::DWORD Surface::getPixel( const Point &pos ) {
	unsigned char *data;
	int pitch;
	util::DWORD pixel;

	_surface->Lock(_surface, DSLF_READ, (void **) &data, &pitch );
	util::DWORD *pixelptr = (util::DWORD *)(data + pos.y * pitch); // row
	pixel = pixelptr[pos.x]; // pixel
	_surface->Unlock(_surface);

	return pixel;
}

void Surface::getColorFromPixel( util::DWORD pixel, Color &color ) {
	DFBSurfacePixelFormat format;
	_surface->GetPixelFormat(_surface, &format);
	if (format == DSPF_ARGB) {
		color.alpha = (util::BYTE) ((pixel & 0xFF000000) >> 24);
	} else if (format == DSPF_RGB32) {
		color.alpha = 255;
	} else {
		LERROR("directfb::Surface", "invalid surface pixel format");
		return;
	}
	if (color.alpha>0) {
		double a = 255.0 / ((double)color.alpha);
		color.r = (util::BYTE) boost::math::iround( ((pixel & 0x00FF0000) >> 16) * a );
		color.g = (util::BYTE) boost::math::iround( ((pixel & 0x0000FF00) >> 8) * a );
		color.b = (util::BYTE) boost::math::iround( (pixel & 0x000000FF) * a );
	} else {
		color.r = (util::BYTE) (pixel & 0x00FF0000) >> 16;
		color.g = (util::BYTE) (pixel & 0x0000FF00) >> 8;
		color.b = pixel & 0x000000FF;
	}
}

void Surface::getPixelColorImpl( const Point &pos, Color &color ) {
	util::DWORD pixel = getPixel(pos);
	getColorFromPixel(pixel, color);
}

#define THRESHOLD 20
bool Surface::equalsImage( const std::string &file ) {
	canvas::Surface *image = canvas()->createSurfaceFromPath( file );
	bool check = true;

	if (!image) {
		return false;
	}
	Size s_size = getSize();
	Size i_size = image->getSize();

	if (s_size.w != i_size.w || s_size.h != i_size.h) {
		canvas()->destroy( image );
		return false;
	} else {
		unsigned char *data1;
		unsigned char *data2;
		int pitch1, pitch2;

		Surface *dfbimage = dynamic_cast<Surface *>(image);
		_surface->Lock(_surface, DSLF_READ, (void **) &data1, &pitch1 );
		dfbimage->getContent()->Lock(dfbimage->getContent(), DSLF_READ, (void **) &data2, &pitch2 );
		DTV_ASSERT( pitch1==pitch2 );
		for ( int y=0; y<i_size.h; y++ ) {
			util::DWORD *pixelptr1 = (util::DWORD *)(data1 + y * pitch1); // row
			util::DWORD *pixelptr2 = (util::DWORD *)(data2 + y * pitch2); // row
			for ( int x=0; x<i_size.w; x++ ) {
				if (pixelptr1[x] != pixelptr2[x]) { // pixels
					Color c1,c2;
					getColorFromPixel(pixelptr1[x], c1);
					getColorFromPixel(pixelptr2[x], c2);
					if (!c1.equals(c2, THRESHOLD ) ) {
						LINFO("Surface", "Difference found in pixel (%d, %d): s_color=(%d,%d,%d,%d), i_color=(%d,%d,%d,%d).",
							x, y, c1.r, c1.g, c1.b, c1.alpha, c2.r, c2.g, c2.b, c2.alpha );
						check=false;
						break;
					}
				}
			}
			if (!check) {
				break;
			}
		}
		dfbimage->getContent()->Unlock(dfbimage->getContent());
		_surface->Unlock(_surface);
	}
	canvas()->destroy( image );
	return check;
}

void Surface::setPixelColorImpl( const Point &pos, const Color &color ) {
	DFBRegion oldClip;
	_surface->GetClip(_surface, &oldClip);

	DFBRegion clip = {pos.x, pos.y, pos.x, pos.y};
	_surface->SetClip(_surface, &clip);
	double a = ((color.alpha<1)?1.0:(double)color.alpha) / 255.0;
	DFBResult err = _surface->Clear(
		_surface,
		(u8) boost::math::iround(color.r * a),
		(u8) boost::math::iround(color.g * a),
		(u8) boost::math::iround(color.b * a),
		(u8) (color.alpha ? color.alpha : 1)
	);
	DFB_CHECK_WARN("setPixelColor");
	_surface->SetClip(_surface, &oldClip);
}

canvas::GlyphRun *Surface::createGlyphRun( const std::string &text, const Point &/*pos*/ ) {
	return new GlyphRun( text.length() );
}

void Surface::renderText( canvas::GlyphRun *glyphRun, const Point &/*pos*/ ) {
	DFBResult err = _surface->SetFont(_surface, dynamic_cast<const Font*>(font())->dfbFont());
	if (err != DFB_OK) {
		LWARN("directfb::Surface", "Any font applied!");
	}

	GlyphRun *gr = dynamic_cast<GlyphRun*>(glyphRun);
	DFBGlyph *glyphs = gr->glyphs();
	int cant = gr->cant();
	for (int i=0; i<cant; i++) {
		_surface->DrawGlyph(_surface, glyphs[i].c, glyphs[i].pos.x, glyphs[i].pos.y, (DFBSurfaceTextFlags)0);
	}
}

IDirectFB *Surface::getDFB() const {
	return ((Canvas*) canvas())->getDFB();
}

void Surface::setCompositionModeImpl( composition::mode mode ) {
	impl::dfb_set_operator(_surface, impl::getDFBCompositionMode(mode));
}

void Surface::resizeImpl( const Size &size, bool scaleContent/*=false*/ ) {
	DFBSurfaceDescription dsc;
	IDirectFBSurface *surface;
	Color c = getColor();

	impl::dfb_init_surface_desc( &dsc, size.w, size.h );
	impl::dfb_create_surface(getDFB(), &surface, &dsc);
	surface->Clear( surface, 0, 0, 0, 0);
	surface->SetColor(surface, c.r, c.g, c.b, c.alpha);

	if (scaleContent) {
		DFBRectangle destRect = {0, 0, size.w, size.h};
		surface->SetColor(surface, 0, 0, 0, _alpha);
		DFBResult err = surface->StretchBlit(surface, _surface, NULL, &destRect);
		DFB_CHECK_WARN( "resizeImpl fails" );
	}
	DFB_DEL_SURFACE(_surface);
	_surface = surface;
	_size = size;
	setCompositionModeImpl(getCompositionMode());
}

}
}
