#include <util/string.h>
#include <util/assert.h>
#include <node/util.h>
#include <node/node_buffer.h>
#include <errno.h>
#include <sys/ioctl.h>

#define TYPE_ERROR(msg)	  \
	ThrowException(Exception::TypeError(_val(msg)));

namespace tvd {

//	ioctl( fd, request, [buffer], callback(err, ret, buffer))
void ioctl(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope scope(isolate);

	int count = args.Length();
	if (count < 3) {
		THROW_RETURN("Invalid # arguments");
	}
	int argc=0;

	int fd;
	if (!impl::get( args[argc++], fd )) {
		THROW_RETURN("Invalid parameter: \"fd\"" );
	}

	int request;
	if (!impl::get( args[argc++], request )) {
		THROW_RETURN("Invalid parameter: \"request\"" );
	}

	void *data = NULL;
	if (count == 4) {
		if (args[argc]->IsUndefined()) {
			THROW_RETURN("Invalid parameter: \"data\"" );
		}

		if (args[argc]->IsInt32()) {
			data = reinterpret_cast<void *>(args[argc]->Int32Value());
		}
		else {
			v8::Local<v8::Object> buf = args[argc]->ToObject();
			if (!node::Buffer::HasInstance(buf)) {
				THROW_RETURN("Invalid parameter: \"data\"" );
			}

			data = node::Buffer::Data(buf);
		}

		argc++;
	}

	if (!args[argc]->IsFunction()) {
		THROW_RETURN("Invalid parameter: \"cb\"" );
	}
	v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[argc++]);

	int ret = ::ioctl(fd, request, data);
	//printf( "ioctl( %d, %d, %d )-> ret=%08x, errno=%d\n", fd, request, data, ret, errno );

	{	//	Call callback
		v8::Local<v8::Value> argv[3];
		argv[0] = ret < 0 ? node::UVException(errno, "ioctl") : v8::Local<v8::Value>::New(isolate,v8::Null(isolate));
		argv[1] = _val(ret);
		argv[2] = args[2];

		cb->Call( isolate->GetCurrentContext()->Global(), 3, argv);
	}

	FIN;
}

void init( v8::Handle<v8::Object> exports ) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	//	Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, tvd::ioctl);
	exports->Set(_val("ioctl"),tpl->GetFunction());
}

}

NODE_MODULE(ioctl, tvd::init)

