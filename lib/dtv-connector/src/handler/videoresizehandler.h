/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#pragma once

#include "messagehandler.h"
#include "../connector.h"
#include <util/buffer.h>
#include <util/assert.h>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <set>

namespace connector {

class Connector;

template<typename T>
class AVDescriptor;

template<typename T>
class VideoResizeHandler : public MessageHandler {
public:
	VideoResizeHandler() {};
	virtual ~VideoResizeHandler() {};

	typedef AVDescriptor<T> AVDescriptorType;
	typedef boost::shared_ptr<AVDescriptorType> AVDescriptorTypePtr;
	void send( Connector *conn, size_t id, AVDescriptorType* descriptor );

	typedef boost::function<void ( size_t id, const AVDescriptorTypePtr &descriptor )> VideoResizeEventCallback;
	void onVideoResizeEvent( const VideoResizeEventCallback &callback );

	virtual void process( util::Buffer *msg );
	virtual messages::type type() const;

private:
	VideoResizeEventCallback _onVideoResizeEvent;
};

#define SIZE sizeof(T)
#define AV_DESCRIPTOR_BUF_SIZE (MESSAGE_HEADER+sizeof(size_t)+(SIZE*4))

template<typename T>
void VideoResizeHandler<T>::send( Connector *conn, size_t id, AVDescriptor<T>* descriptor ) {
	char buf[AV_DESCRIPTOR_BUF_SIZE];
	char *ptr = (char *)MSG_PAYLOAD(buf);
	int offset = 0;

	memcpy(ptr+offset, &id, sizeof(id) );
	offset+=sizeof(id);

	T value = descriptor->getX();
	memcpy(ptr+offset, &value, SIZE );
	offset+=SIZE;

	value = descriptor->getY();
	memcpy(ptr+offset, &value, SIZE );
	offset+=SIZE;

	value = descriptor->getWidth();
	memcpy(ptr+offset, &value, SIZE );
	offset+=SIZE;

	value =descriptor->getHeight();
	memcpy(ptr+offset, &value, SIZE );
	offset += SIZE;

	DTV_ASSERT(offset+MESSAGE_HEADER <= (int)AV_DESCRIPTOR_BUF_SIZE);
	MAKE_MSG(buf,messages::videoResize,offset+MESSAGE_HEADER);

	util::Buffer msg( buf, offset+MESSAGE_HEADER, false );
	conn->send( &msg );
}


template<typename T>
void VideoResizeHandler<T>::process( util::Buffer *msg ) {
	if (!_onVideoResizeEvent.empty()) {
		AVDescriptorTypePtr msgData(new AVDescriptorType());
		util::BYTE *payload = MSG_PAYLOAD(msg->bytes());
		T value = 0;

		size_t id;
		memcpy( &id, payload, sizeof(id) );
		payload += sizeof(id);

		memcpy( &value, payload, SIZE );
		msgData->setX(value);
		payload += SIZE;

		memcpy( &value, payload, SIZE );
		msgData->setY(value);
		payload += SIZE;

		memcpy( &value, payload, SIZE );
		msgData->setWidth(value);
		payload += SIZE;

		memcpy( &value, payload, SIZE );
		msgData->setHeight(value);

		dispatch( boost::bind(_onVideoResizeEvent,id,msgData) );
	}
}

#undef SIZE
#undef AV_DESCRIPTOR_BUF_SIZE

template<typename T>
void VideoResizeHandler<T>::onVideoResizeEvent( const VideoResizeEventCallback &callback ) {
	_onVideoResizeEvent = callback;
};

template<typename T>
messages::type VideoResizeHandler<T>::type() const {
	return messages::videoResize;
}

template <typename T>
class AVDescriptor{
	public:
		AVDescriptor() {};
		AVDescriptor(T x,T y,T w,T h);
		virtual ~AVDescriptor( void ) {};
		T getX();
		T getY();
		T getWidth();
		T getHeight();
		void setX(T x);
		void setY(T y);
		void setWidth(T width);
		void setHeight(T height);
	private:
		T x;
		T y;
		T width;
		T height;
};

template <typename T>
AVDescriptor<T>::AVDescriptor(T x, T y, T width, T height ){
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

/* Getters */
template <typename T>
T AVDescriptor<T>::getX(){
	return x;
}

template <typename T>
T AVDescriptor<T>::getY(){
	return y;
}

template <typename T>
T AVDescriptor<T>::getWidth(){
	return width;
}

template <typename T>
T AVDescriptor<T>::getHeight(){
	return height;
}

/* Setters */

template <typename T>
void AVDescriptor<T>::setX(T x){
	this->x = x;
}

template <typename T>
void AVDescriptor<T>::setY(T y){
	this->y = y;
}

template <typename T>
void AVDescriptor<T>::setWidth(T width){
	this->width = width;
}

template <typename T>
void AVDescriptor<T>::setHeight(T height){
	this->height = height;
}

}

