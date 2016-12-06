#pragma once

#include <iconv.h>

namespace sample {

	/**
	* Clase perteneciente a la documentación "build_cmake".
	*/
        class Sample {
        public:
                //      Constructors
                Sample( void );
                virtual ~Sample( void );

		/**
		* Convierte el string recibido como parámetro de ISO_8859-1 a UTF-8
		* @param in_string texto en formato ISO_8859-1 a ser convertido.
		* @return texto convertido a formato UTF-8
		*/
		char* convert(char* in_string);
	private:
		iconv_t _conv_desc;
	};
}
