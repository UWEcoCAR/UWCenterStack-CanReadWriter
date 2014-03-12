#include <node.h>
#include <iostream>

using namespace v8;
using namespace std;

/*
Print error message
*/
Handle<Value> Start(const Arguments& args) {

    // All V8 functions need a scope
    HandleScope scope;

    cout << "ERROR: YOU SHOULD NOT BE RUNNING CAN_READ_WRITER FROM MAC OR WINDOWS!!!" << endl; 

    return Undefined();
}

/*
Initializes module. Adds functions to module.
*/
void RegisterModule(Handle<Object> target) {
    target->Set(String::NewSymbol("start"),
        FunctionTemplate::New(Start)->GetFunction());
}

NODE_MODULE(canReadWriter, RegisterModule);
