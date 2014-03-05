#include <node.h>

// C standard library
#include <cstdlib>
#include <ctime>

using namespace v8;

// This funtion will be called when a competition message has been read.
// Right now, this just represents an example in which a callback is made with
// parameters "voltage" and value 5. Please see the CanReadWriter.js for how these
// parameters come into play.
Handle<Value> ReadResult(const Arguments& args) {
  HandleScope scope;

  Local<Function> callback = Local<Function>::Cast(args[0]);

  const unsigned argc = 2; //two parameters for callback function
  Local<Value> argv[argc] = {
    Local<Value>::New(String::New("voltage")),
    Local<Value>::New(Integer::New(5))
  };

  callback->Call(Context::GetCurrent()->Global(), argc, argv);
  return Undefined();
}

// this is the initialization function that will be called for the module
// this is where we need to start the threads for reading
void init(Handle<Object> target) {

    // target is the module object you see when require()ing the .node file.
    target->Set(String::NewSymbol("readResult"),
        FunctionTemplate::New(ReadResult)->GetFunction());
}

NODE_MODULE(canReadWriter, init);
