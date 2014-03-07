#include <node.h>

#include <queue>
#include <string>
#include <sstream>
#include <unistd.h>

// C standard library
#include <cstdlib>
#include <ctime>

using namespace v8;
using namespace std;

// A single signal processed from a message
struct canSignal {
    string name;
    double value;
};

// The data from a message received
struct canMessage {
    int id;
    char data[8];
    int length;
};

// Data to pass to ReadMessages
struct canReadBaton {
    // bus params
    int channel;
    int baudRate;
    int tseg1;
    int tseg2;
    int sjw;
    int samplePoints;
    int syncMode;
    int canFlags;

    // synchronization
    queue<canMessage*>* readQueue;
    uv_mutex_t* readQueueLock;
    uv_cond_t* readQueueNotEmpty;
};

// Data to pass to ProcessMessages
struct canProcessBaton {
    // TODO process function
    // TODO message map

    // read side synchronization
    queue<canMessage*>* readQueue;
    uv_mutex_t* readQueueLock;
    uv_cond_t* readQueueNotEmpty;

    // processed side synchronization
    queue<canSignal*>* processedQueue;
    uv_mutex_t* processedQueueLock;
    uv_async_t* processedAsync;
};

// Data to pass to ExecuteCallbacks
struct canCallbackBaton {
    // callback function
    Persistent<Function> callback;

    // synchronization
    queue<canSignal*>* processedQueue;
    uv_mutex_t* processedQueueLock;
};

/*
Fires the callback function for each signal in the processedQueue.
This function should be signaled via the async when a signal is added to the processedQueue.
This function must run in the V8 thread
*/
void ExecuteCallbacks(uv_async_t* handle /*UNUSED*/, int status /*UNUSED*/) {

    // Retrieve baton
    canCallbackBaton* baton = (canCallbackBaton*) handle->data;

    // Lock the processedQueue
    uv_mutex_lock(baton->processedQueueLock);

    // Run until it is empty
    while (!baton->processedQueue->empty()) {

        // Dequeue a signal
        canSignal* s = baton->processedQueue->front();
        baton->processedQueue->pop();

        // Let others access the queue while we callback
        uv_mutex_unlock(baton->processedQueueLock);

        // Callback to the JS
        const unsigned argc = 2;
        Local<Value> argv[argc] = {
            Local<Value>::New(String::New(s->name.c_str())),
            Local<Value>::New(Integer::New(s->value))
        };
        baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);

        // Clean up
        delete s;

        // Regain the lock before looping again
        uv_mutex_lock(baton->processedQueueLock);
    }

    // We are all finished with the queue, so let others fill it up
    uv_mutex_unlock(baton->processedQueueLock);
}

/*
Constantly reads messages from a CAN bus using the baton's params (never exiting).
Pushes messages onto the baton's readQueue.
req->data should be a canReadBaton.
Does not need to run in the V8 thread.
*/
void ReadMessages(uv_work_t* req) {

    // Retrieve baton
    canReadBaton* baton = (canReadBaton*) req->data;

    int i = 0;
    while (1) {
        // TODO canReadWait goes here
        // TODO ensure it is an interesting message here
        usleep(20000); // Simulate a canReadWaitTime

        // Create message
        canMessage* m = new canMessage;
        m->id = 1024;
        m->data[0] = i++;
        m->data[1] = i++;
        m->data[2] = i++;
        m->data[3] = i++;
        m->data[4] = i++;
        m->data[5] = i++;
        m->data[6] = i++;
        m->data[7] = i++;
        m->length = 8;

        // Add message to hsReadQueue
        uv_mutex_lock(baton->readQueueLock);
        baton->readQueue->push(m);
        if (baton->readQueue->size() >= 10) {
            printf("WARNING: There are %lu unprocessed messages\n", baton->readQueue->size());
        }
        uv_mutex_unlock(baton->readQueueLock);

        // Let others know there is something to process
        uv_cond_signal(baton->readQueueNotEmpty);
    }
}

/*
Constantly processes messages from the hsReadQueue into signals that
are placed on the processedQueue (never exiting).
Does not need to run in the V8 thread.
*/
void ProcessMessages(uv_work_t* req) {

    // Retrieve baton
    canProcessBaton* baton = (canProcessBaton*) req->data;

    while (1) {

        // Lock readQueue
        uv_mutex_lock(baton->readQueueLock);

        // Wait for a message to come in
        while (baton->readQueue->empty()) {
            uv_cond_wait(baton->readQueueNotEmpty, baton->readQueueLock);
        }

        // Pop the message off the queue
        canMessage* m = baton->readQueue->front();
        baton->readQueue->pop();

        // Unlock hsReadQueue while we process the message
        uv_mutex_unlock(baton->readQueueLock);

        // Lock processedQueue
        uv_mutex_lock(baton->processedQueueLock);

        // Fake process signal
        for (int i = 0; i < m->length; i++) {
            canSignal* s = new canSignal;

            ostringstream oss;
            oss << "signal" << i;

            s->name = oss.str();
            s->value = m->data[i];

            // Push processed signal onto processedQueue
            baton->processedQueue->push(s);
        }
        if (baton->processedQueue->size() >= 80) {
            printf("WARNING: There are %lu unfired signals\n", baton->processedQueue->size());
        }

        // Unlock processedQueue while we go back to waiting for messages
        uv_mutex_unlock(baton->processedQueueLock);

        // Signal the async that there are signals to fire
        uv_async_send(baton->processedAsync);

        // Clean up
        delete m;
    }
}

/*
Starts up all of our threads.
Args should contain a callback function.
*/
Handle<Value> Start(const Arguments& args) {

    // All V8 functions need a scope
    HandleScope scope;

    // Initialize HS read synchronization
    queue<canMessage*>* hsReadQueue = new queue<canMessage*>();
    uv_mutex_t* hsReadQueueLock = new uv_mutex_t;
    uv_cond_t* hsReadQueueNotEmpty = new uv_cond_t;
    uv_mutex_init(hsReadQueueLock);
    uv_cond_init(hsReadQueueNotEmpty);

    // Initialize processed synchronization
    queue<canSignal*>* processedQueue = new queue<canSignal*>();
    uv_mutex_t* processedQueueLock = new uv_mutex_t;
    uv_mutex_init(processedQueueLock);

    // Initialize processedAsync baton
    canCallbackBaton* processedAsyncBaton = new canCallbackBaton;
    processedAsyncBaton->processedQueue = processedQueue;
    processedAsyncBaton->processedQueueLock = processedQueueLock;
    processedAsyncBaton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

    // Initialize processedAsync
    uv_async_t* processedAsync = new uv_async_t;
    processedAsync->data = (void*) processedAsyncBaton;

    // Initialize HS read baton
    canReadBaton* hsCanReadBaton = new canReadBaton;
    hsCanReadBaton->readQueue = hsReadQueue;
    hsCanReadBaton->readQueueLock = hsReadQueueLock;
    hsCanReadBaton->readQueueNotEmpty = hsReadQueueNotEmpty;

    // Initialize HS process baton
    canProcessBaton* hsCanProcessBaton = new canProcessBaton;
    hsCanProcessBaton->readQueue = hsReadQueue;
    hsCanProcessBaton->readQueueLock = hsReadQueueLock;
    hsCanProcessBaton->readQueueNotEmpty = hsReadQueueNotEmpty;
    hsCanProcessBaton->processedQueue = processedQueue;
    hsCanProcessBaton->processedQueueLock = processedQueueLock;
    hsCanProcessBaton->processedAsync = processedAsync;

    // Initialize HS read work request
    uv_work_t* hsReadReq = new uv_work_t();
    hsReadReq->data = (void*) hsCanReadBaton;

    // Initialize HS process work request
    uv_work_t* hsProcessReq = new uv_work_t();
    hsProcessReq->data = (void*) hsCanProcessBaton;

    // Start all our threads
    uv_loop_t* loop = uv_default_loop();
    uv_async_init(loop, processedAsync, ExecuteCallbacks);
    uv_queue_work(loop, hsReadReq, ReadMessages, NULL);
    uv_queue_work(loop, hsProcessReq, ProcessMessages, NULL);

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
