#include <node.h>

extern "C" {
    #include <canlib.h>
}

#include <queue>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>

// C standard library
#include <cstdlib>
#include <ctime>

#define HS_CHANNEL 0
#define HS_BAUD 500000
#define HS_TSEG1 4
#define HS_TSEG2 3
#define HS_SJW 1
#define HS_SAMPLE_POINTS 1
#define HS_SYNC_MODE 0
#define HS_FLAGS 0

#define LS_CHANNEL 1
#define LS_BAUD 33333
#define LS_TSEG1 12
#define LS_TSEG2 3
#define LS_SJW 3
#define LS_SAMPLE_POINTS 1
#define LS_SYNC_MODE 0
#define LS_FLAGS 0

using namespace v8;
using namespace std;

// Struct definition plus constructor for struct
struct signalDef {
  public:
    string name;
    int startBit;
    int length;
    double scale;
    int offset;
    string unit;

    signalDef(string nameSig, int startInt, int lengthInt, double scaleInt, int offsetInt, string unit) : 
    name(nameSig), 
    startBit(startInt), 
    length(lengthInt), 
    scale(scaleInt), 
    offset(offsetInt), 
    unit(unit)  { }
};

// Define signalMap data structure
// Keys are ints and values are signalDef types
typedef unordered_multimap<int, signalDef> signalMap;

// A single signal processed from a message
struct canSignal {
    string name;
    double value;
    string unit;
};

// The data from a message received
struct canMessage {
    long id;
    unsigned char data[8];
    unsigned int length;
};

// Data to pass to ReadMessages
struct canReadBaton {
    signalMap signalDefinitions;

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
    signalMap signalDefinitions;

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


Persistent<Object> context;  

// Creates a signalMap of ints and vectors
signalMap createSignalMap() {

  signalMap m = {

    {1954, signalDef("batteryCurrent", 48, 16, 0.025, -1000, "amps")},
    {1954, signalDef("batteryVoltage", 36, 12, 0.25, 0, "volts")},
    {1954, signalDef("batteryTemp", 28, 8, 0.5, -40, "Deg C")},
    {1954, signalDef("batterySoc", 20, 8, 0.5, 0, "%")},
    {1954, signalDef("engineTemp", 12, 8, 1, -40, "Deg C")},

    
    {1955, signalDef("engineTorque", 4, 12, 0.5, -848, "Nm")},
    {1955, signalDef("engineRpm", 16, 16, 0.25, 0, "rpm")},
    {1955, signalDef("vehicleSpeed", 33, 15, 0.015625, 0, "km / h")},
    {1955, signalDef("motorTemp", 48, 16, 0.1, 0, "degC")},

    {1956, signalDef("transRatio", 8, 8, 0.03125, 0, "")},
    {1956, signalDef("transGear", 19, 4, 1, 0, "")},
    {1956, signalDef("vehicleBrake", 23, 1, 1, 0, "")},
    {1956, signalDef("vehicleAccel", 24, 8, 0.392156862745098, 0, "%")},
    {1956, signalDef("motorTorque", 32, 16, 0.1, 0, "Nm")},
    {1956, signalDef("motorRpm", 48, 16, 1, 0, "rpm")},

    {1957, signalDef("chargerCurrent", 32, 16, 0.01, 0, "A")},
    {1957, signalDef("chargerVoltage",48, 16, 0.1, 0, "V")},
  };

  return m;
}

// Takes an id and byte array and prints out the corresponding signal definitions
vector<canSignal*> Parse(signalMap m, unsigned long id, unsigned char message[], unsigned int byteLength) { 
  int length = byteLength;
  unsigned long mask;
  double signal;
  unsigned long data = 0;
  vector<canSignal*> signals;
  for (int i = 0; i < length; i++) {
    data += ((unsigned long)message[i] << ((length-1-i)*8));
  }
  auto range = m.equal_range(id);
  for (auto it = range.first; it != range.second; ++it) {
    signalDef ourSignal = it->second;
    unsigned long ones = ((1 << ourSignal.length) - 1);
    mask = ones << ourSignal.startBit;
    signal = (data & mask) >> ourSignal.startBit;
    signal *= ourSignal.scale;
    signal += ourSignal.offset;
    
    canSignal* cSig = new canSignal;
    cSig->name = ourSignal.name;
    cSig->value = signal;
    cSig->unit = ourSignal.unit;
    signals.push_back(cSig);
  }
  return signals;
}

/*
Fires the callback function for each signal in the processedQueue.
This function should be signaled via the async when a signal is added to the processedQueue.
This function must run in the V8 thread
*/
void ExecuteCallbacks(uv_async_t* handle, int status /*UNUSED*/) {

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
        TryCatch tryCatch;
        baton->callback->Call(context, argc, argv);
        if (tryCatch.HasCaught()) {
            node::FatalException(tryCatch);
        }

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

    canHandle handle = canOpenChannel(baton->channel, baton->canFlags);
    if (handle < 0) {
      printf("ERROR: canOpenChannel %d failed: %d\n", baton->channel, handle);
      return;
    }
    canSetBusParams(handle, baton->baudRate, baton->tseg1, baton->tseg2, baton->sjw, baton->samplePoints, baton->syncMode);
    canBusOn(handle);

    while (1) {

        // Create message
        unsigned int flags;
        unsigned long timestamp;
        canMessage* m = new canMessage;
        canReadWait(handle, &m->id, m->data, &m->length, &flags, &timestamp, 0xFFFFFFFF);

        if (baton->signalDefinitions.count(m->id) == 0) {
            continue;
        }

        // Add message to readQueue
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

        vector<canSignal*> signals = Parse(baton->signalDefinitions, m->id, m->data, m->length);

        // Lock processedQueue
        uv_mutex_lock(baton->processedQueueLock);
        
        for (auto it = signals.begin(); it != signals.end(); ++it) {
            baton->processedQueue->push(*it);
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

    // Initialize signalDefintions
    signalMap signalDefinitions = createSignalMap();

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
    hsCanReadBaton->signalDefinitions = signalDefinitions;
    hsCanReadBaton->channel = HS_CHANNEL;
    hsCanReadBaton->baudRate = HS_BAUD;
    hsCanReadBaton->tseg1 = HS_TSEG1;
    hsCanReadBaton->tseg2 = HS_TSEG2;
    hsCanReadBaton->sjw = HS_SJW;
    hsCanReadBaton->samplePoints = HS_SAMPLE_POINTS;
    hsCanReadBaton->syncMode = HS_SYNC_MODE;
    hsCanReadBaton->canFlags = HS_FLAGS;
    hsCanReadBaton->readQueue = hsReadQueue;
    hsCanReadBaton->readQueueLock = hsReadQueueLock;
    hsCanReadBaton->readQueueNotEmpty = hsReadQueueNotEmpty;

    // Initialize HS process baton
    canProcessBaton* hsCanProcessBaton = new canProcessBaton;
    hsCanProcessBaton->signalDefinitions = signalDefinitions;
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
    context = Persistent<Object>::New(target);
    target->Set(String::NewSymbol("start"),
        FunctionTemplate::New(Start)->GetFunction());
}

NODE_MODULE(canReadWriter, RegisterModule);
