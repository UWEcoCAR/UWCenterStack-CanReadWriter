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

#define IS_SIGNED true
#define IS_NOT_SIGNED false
#define IS_EXTENDED true
#define IS_NOT_EXTENDED false

using namespace v8;
using namespace std;

// Struct definition plus constructor for struct
struct signalDef {
  public:
    boolean isExtended;
    string name;
    boolean isSigned;
    int startBit;
    int length;
    double scale;
    int offset;
    string unit;
s
    signalDef(boolean isExtended, string name, boolean isSigned, int startBit, int length, double scale, int offset, string unit) :
    isExtended(isExtended),
    name(name),
    isSigned(isSigned);
    startBit(startBit),
    length(length),
    scale(scale),
    offset(offset),
    unit(unit)  { }
};

struct messageDef {
  long id;
  unsigned long message;
  int startBit;
  int length;

  messageDef(long id, unsigned long messageDefault, int startBit, int length) :
    id(id),
    message(messageDefault),
    startBit(startBit),
    length(length) { }
};

// Define readSignalMap data structure
// Keys are ints and values are signalDef types
typedef unordered_multimap<int, signalDef> readSignalMap;

// Define writeMessageMap data structure
// Keys are strings and values are messageDef types
typedef unordered_map<string, messageDef> writeMessageMap;

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
    readSignalMap signalDefinitions;

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
struct canProcessReadBaton {
    readSignalMap signalDefinitions;

    // read side synchronization
    queue<canMessage*>* readQueue;
    uv_mutex_t* readQueueLock;
    uv_cond_t* readQueueNotEmpty;

    // processed side synchronization
    queue<canSignal*>* processedReadQueue;
    uv_mutex_t* processedReadQueueLock;
    uv_async_t* processedReadAsync;
};

// Data to pass to ExecuteCallbacks
struct canCallbackBaton {
    // callback function
    Persistent<Function> callback;

    // synchronization
    queue<canSignal*>* processedReadQueue;
    uv_mutex_t* processedReadQueueLock;
};

// Data to pass to WriteMessages
struct canProcessWriteBaton {
    writeMessageMap messageDefinitions;

    // synchronization from javascript
    queue<canMessage*>* writeQueue;
    uv_mutex_t* writeQueueLock;
    uv_cond_t* writeQueueNotEmpty;

    // processed side synchronization
    queue<canMessage*>* processedWriteQueue;
    uv_mutex_t* processedWriteQueueLock;
    uv_cond_t* processedWriteQueueNotEmpty;
};

struct canWriteBaton {
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
    queue<canMessage*>* processedWriteQueue;
    uv_mutex_t* processedWriteQueueLock;
    uv_cond_t* processedWriteQueueNotEmpty;
};

Persistent<Object> context;  

// Global write queue and synchronization
queue<canSignal*>* lsWriteQueue;
uv_mutex_t* lsWriteQueueLock;
uv_cond_t* lsWriteQueueNotEmpty;


// Creates a readSignalMap of ints and vectors
readSignalMap createHsReadSignalMap() {

  readSignalMap m = {

    {1954, signalDef(IS_NOT_EXTENDED, "batteryCurrent", IS_NOT_SIGNED, 48, 16, 0.025, -1000, "amps")},
    {1954, signalDef(IS_NOT_EXTENDED, "batteryVoltage", IS_NOT_SIGNED, 36, 12, 0.25, 0, "volts")},
    {1954, signalDef(IS_NOT_EXTENDED, "batteryTemp", IS_NOT_SIGNED, 28, 8, 0.5, -40, "Deg C")},
    {1954, signalDef(IS_NOT_EXTENDED, "batterySoc", IS_NOT_SIGNED, 20, 8, 0.5, 0, "%")},
    {1954, signalDef(IS_NOT_EXTENDED, "engineTemp", IS_NOT_SIGNED, 12, 8, 1, -40, "Deg C")},


    {1955, signalDef(IS_NOT_EXTENDED, "engineTorque", IS_NOT_SIGNED, 4, 12, 0.5, -848, "Nm")},
    {1955, signalDef(IS_NOT_EXTENDED, "engineRpm", IS_NOT_SIGNED, 16, 16, 0.25, 0, "rpm")},
    {1955, signalDef(IS_NOT_EXTENDED, "vehicleSpeed", IS_NOT_SIGNED, 33, 15, 0.015625, 0, "km / h")},
    {1955, signalDef(IS_NOT_EXTENDED, "motorTemp", IS_NOT_SIGNED, 48, 16, 0.1, 0, "degC")},

    {1956, signalDef(IS_NOT_EXTENDED, "transRatio", IS_NOT_SIGNED, 8, 8, 0.03125, 0, "")},
    {1956, signalDef(IS_NOT_EXTENDED, "transGear", IS_NOT_SIGNED, 19, 4, 1, 0, "")},
    {1956, signalDef(IS_NOT_EXTENDED, "vehicleBrake", IS_NOT_SIGNED, 23, 1, 1, 0, "")},
    {1956, signalDef(IS_NOT_EXTENDED, "vehicleAccel", IS_NOT_SIGNED, 24, 8, 0.392156862745098, 0, "%")},
    {1956, signalDef(IS_NOT_EXTENDED, "motorTorque", IS_NOT_SIGNED, 32, 16, 0.1, 0, "Nm")},
    {1956, signalDef(IS_NOT_EXTENDED, "motorRpm", IS_NOT_SIGNED, 48, 16, 1, 0, "rpm")},

    {1957, signalDef(IS_NOT_EXTENDED, "chargerCurrent", IS_NOT_SIGNED, 32, 16, 0.01, 0, "A")},
    {1957, signalDef(IS_NOT_EXTENDED, "chargerVoltage", IS_NOT_SIGNED, 48, 16, 0.1, 0, "V")},
  };

  return m;
}

// Creates a readSignalMap of ints and vectors
readSignalMap createLsReadSignalMap() {

  readSignalMap m = {
    {0x102AA000, signalDef(IS_EXTENDED, "gpsLatitude", IS_SIGNED, 32, 30, 1, 0, "deg")},
    {0x102AA000, signalDef(IS_EXTENDED, "gpsLongitude", IS_SIGNED, 0, 31, 1, 0, "deg")},
  };

  return m;
}

writeMessageMap createLsWriteMessageMap() {
  writeMessageMap m = {

    {"diagnosticMode", messageDef(0x101, (unsigned long) 0x000000003E01FE07, -1, 8)},

    {"toggleAc", messageDef(0x251, (unsigned long) 0x000000010104AE07, -1, 8)},

    {"toggleAutoTemp", messageDef(0x251, (unsigned long) 0x000000080804AE07, -1, 8)},

    {"toggleRecirculate", messageDef(0x251, (unsigned long) 0x000000040404AE07, -1, 8)},

    {"toggleRearDefrost", messageDef(0x251, (unsigned long) 0x000000101004AE07, -1, 8)},

    {"toggleDefrost", messageDef(0x251, (unsigned long) 0x000101000004AE07, -1, 8)},

    {"toggleTopVent", messageDef(0x251, (unsigned long) 0x000000404004AE07, -1, 8)},

    {"toggleFloorVent", messageDef(0x251, (unsigned long) 0x000000808004AE07, -1, 8)},

    {"ventFanSpeed", messageDef(0x251, (unsigned long) 0x000000000802AE07, 56, 8)},

    {"driverTemp", messageDef(0x251, (unsigned long) 0x000000000102AE07, 32, 8)},

    {"passengerTemp", messageDef(0x251, (unsigned long) 0x000000000202AE07, 32, 8)}
  };

  return m;
}

// Takes a value and string of a message
// Returns a canMessage struct with an updated byte value to be written
canMessage* WriteParse(writeMessageMap m, string name, unsigned long value) {
  canMessage* c = new canMessage;
  int startBit = m.at(name).startBit;
  c->id = m.at(name).id;
  c->length = m.at(name).length;
  unsigned long message = m.at(name).message;
  if (startBit != -1) {
    message = (message + (value << startBit));
  }
  for (int i = 0; i < c->length; i++) {
    c->data[i] = (unsigned char)message;
    message = message >> 8;
  }
  // cout <<message<<endl;
  return c;
}

// Takes an id and byte array and prints out the corresponding signal definitions
vector<canSignal*> ReadParse(readSignalMap m, unsigned long id, unsigned char message[], unsigned int length) {
  unsigned long mask;
  double signal;
  unsigned long data = 0;
  vector<canSignal*> signals;

  // Convert message bytes into a single number (using Big-Endien layout).
  for (int i = 0; i < length; i++) {
    data += ((unsigned long) message[i] << ((length - 1 - i) * 8));
  }

  // Parse out each of the signals
  auto range = m.equal_range(id);
  for (auto it = range.first; it != range.second; ++it) {
    signalDef ourSignal = it->second;

    // Mask out our signal
    mask = ((1 << ourSignal.length) - 1) << ourSignal.startBit;
    long tempSignal = (data & mask) >> ourSignal.startBit;

    // If the signal is signed and negative, move the signed bit to the end (e.g. with length 6, 0b0...00101010 becomes 0b1...11101010)
    if (ourSignal.isSigned && tempSignal >> (ourSignal.length - 1) != 0) {
      tempSignal = tempSignal | -1 << ourSignal.length;
    }

    // Scale and offset signal
    signal = (double) tempSignal;
    signal *= ourSignal.scale;
    signal += ourSignal.offset;

    // Create canSignal
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
    uv_mutex_lock(baton->processedReadQueueLock);

    // Run until it is empty
    while (!baton->processedReadQueue->empty()) {

        // Dequeue a signal
        canSignal* s = baton->processedReadQueue->front();
        baton->processedReadQueue->pop();

        // Let others access the queue while we callback
        uv_mutex_unlock(baton->processedReadQueueLock);

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
        uv_mutex_lock(baton->processedReadQueueLock);
    }

    // We are all finished with the queue, so let others fill it up
    uv_mutex_unlock(baton->processedReadQueueLock);
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
void ProcessReadMessages(uv_work_t* req) {

    // Retrieve baton
    canProcessReadBaton* baton = (canProcessReadBaton*) req->data;

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

        vector<canSignal*> signals = ReadParse(baton->signalDefinitions, m->id, m->data, m->length);

        // Lock processedQueue
        uv_mutex_lock(baton->processedReadQueueLock);

        for (auto it = signals.begin(); it != signals.end(); ++it) {
            baton->processedReadQueue->push(*it);
        }
        if (baton->processedReadQueue->size() >= 80) {
            printf("WARNING: There are %lu unfired signals\n", baton->processedReadQueue->size());
        }

        // Unlock processedQueue while we go back to waiting for messages
        uv_mutex_unlock(baton->processedReadQueueLock);

        // Signal the async that there are signals to fire
        uv_async_send(baton->processedReadAsync);

        // Clean up
        delete m;
    }
}

/*
  Constantly processes messages from writeQueue (never exits).
  req->data should be a canProcessWriteBaton.
  Does not need to run in the V8 thread.
*/
void ProcessWriteMessages(uv_work_t* req) {

    // Retrieve baton
    canProcessWriteBaton* baton = (canProcessWriteBaton*) req->data;

    while (1) {
        // Lock writeQueue
        uv_mutex_lock(baton->writeQueueLock);

        // Wait for a message to come in
        while (baton->writeQueue->empty()) {
            uv_cond_wait(baton->writeQueueNotEmpty, baton->writeQueueLock);
        }

        // Pop the message information off the queue
        canSignal* signal = baton->writeQueue->pop();

        // Unlock queue while we send the message
        uv_mutex_unlock(baton->writeQueueLock);
	
        // Process Message
        canMessage *m = WriteParse(baton->messageDefinitions, signal->name, signal->value) {

        // Lock processedQueue
        uv_mutex_lock(baton->processedWriteQueueLock);
	     
        // Add message to processed queue
        baton->processedWriteQueue->push(m);

        if (baton->processedWriteQueue->size() > 80) {
            printf("WARNING: There are %lu unprocessed messages\n", baton->processedWriteQueue->size());
        }
	
        // Unlock processedWriteQueue while we go back to waiting for messages
        // from JavaScript
        uv_mutex_unlock(baton->processedWriteQueueLock);

        // Let others know there is something to send
        uv_cond_signal(baton->processedWriteQueueNotEmpty);
    }
}

/*
Constantly sends messages from processedWriteQueue.
req->data should be a canReadBaton.
Does not need to run in the V8 thread.
*/
void SendWriteMessages(uv_work_t* req) {
    
    // Retrieve baton
    canWriteBaton* baton = (canWriteBaton*) req->data;
    canHandle handle = canOpenChannel(baton->channel, baton->canFlags);
    if (handle < 0) {
      printf("ERROR: canOpenChannel %d failed: %d\n", baton->channel, handle);
      return;
    }
    canSetBusParams(handle, baton->baudRate, baton->tseg1, baton->tseg2, baton->sjw, baton->samplePoints, baton->syncMode);
    canBusOn(handle);

    while (1) {

        // Lock processedWriteQueue
        uv_mutex_lock(baton->processedWriteQueueLock);

        // Wait for a message to come in
        while (baton->processedWriteQueue->empty()) {
          uv_cond_wait(baton->processedWriteQueueNotEmpty, baton->processedWriteQueueLock);
        }

        // Pop the message off the queue
        canMessage* m = baton->processedWriteQueue->front();
        baton->processedWriteQueue->pop();

        // Unlock queue while we send the message
        uv_mutex_unlock(baton->processedWriteQueueLock);

        // send the message
        canWrite(handle, m->id, m->data, m->length, canMSG_STD);

        // Clean up
        delete m;
    }
}

Handle<Value> Write(const Arguments& args) {

    // All V8 functions need a scope
    HandleScope scope;

    if (args.length() < 2) {
        return ThrowException(Exception::TypeError(String::New("You must pass two arguments")))       
    }

    canSignal* signal = new canSignal;

    String::Utf8Value param0(args[0]->ToString());
    signal->name = std::string(*param0);    
    signal->value = args[1]->ToInteger()->Value();

    // Lock lsWriteQueue
    uv_mutex_lock(lsWriteQueueLock);

    lsWriteQueue->push(signal);

    uv_mutex_unlock(lsWriteQueueLock);   

    uv_cond_signal(lsWriteQueueNotEmpty); 

    return Undefined();

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

    // Initialize LS read synchronization
    queue<canMessage*>* lsReadQueue = new queue<canMessage*>();
    uv_mutex_t* lsReadQueueLock = new uv_mutex_t;
    uv_cond_t* lsReadQueueNotEmpty = new uv_cond_t;
    uv_mutex_init(lsReadQueueLock);
    uv_cond_init(lsReadQueueNotEmpty);

    // Initialize read processed synchronization
    queue<canSignal*>* processedReadQueue = new queue<canSignal*>();
    uv_mutex_t* processedReadQueueLock = new uv_mutex_t;
    uv_mutex_init(processedReadQueueLock);

    // Initialize processedReadAsync baton
    canReadCallbackBaton* processedAsyncBaton = new canReadCallbackBaton;
    processedAsyncBaton->processedReadQueue = processedReadQueue;
    processedAsyncBaton->processedReadQueueLock = processedReadQueueLock;
    processedAsyncBaton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

    // Initialize processedReadAsync
    uv_async_t* processedReadAsync = new uv_async_t;
    processedReadAsync->data = (void*) processedReadAsyncBaton;

    // Initialize processed write synchronization
    queue<canMessage*>* lsProcessedWriteQueue = new queue<canMessage*>();
    uv_mutex_t* lsProcessedWriteQueueLock = new uv_mutex_t;
    uv_cond_t* lsProcessedWriteQueueNotEmpty = new uv_cond_t;
    uv_mutex_init(lsProcessedWriteQueueLock);
    uv_cond_init(lsProcessedWriteQueueNotEmpty);

    // Initialize gloabl write synchronization
    lsWriteQueue = new queue<canSignal*>();
    lsWriteQueueLock = new uv_mutex_t;
    lsWriteQueueNotEmpty = new uv_cond_t;
    uv_mutex_init(lsWriteQueueLock);
    uv_cond_init(lsWriteQueueNotEmpty);

    // Initialize HS read baton
    canReadBaton* hsCanReadBaton = new canReadBaton;
    hsCanReadBaton->signalDefinitions = createHsReadSignalMap();
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

    // Initialize LS read baton
    canReadBaton* lsCanReadBaton = new canReadBaton;
    lsCanReadBaton->signalDefinitions = createLsReadSignalMap();
    lsCanReadBaton->channel = LS_CHANNEL;
    lsCanReadBaton->baudRate = LS_BAUD;
    lsCanReadBaton->tseg1 = LS_TSEG1;
    lsCanReadBaton->tseg2 = LS_TSEG2;
    lsCanReadBaton->sjw = LS_SJW;
    lsCanReadBaton->samplePoints = LS_SAMPLE_POINTS;
    lsCanReadBaton->syncMode = LS_SYNC_MODE;
    lsCanReadBaton->canFlags = LS_FLAGS;
    lsCanReadBaton->readQueue = lsReadQueue;
    lsCanReadBaton->readQueueLock = lsReadQueueLock;
    lsCanReadBaton->readQueueNotEmpty = lsReadQueueNotEmpty;

    // Initialize HS read process baton
    canProcessBaton* canHsProcessReadBaton = new canProcessBaton;
    canHsProcessReadBaton->signalDefinitions = createHsReadSignalMap();
    canHsProcessReadBaton->readQueue = hsReadQueue;
    canHsProcessReadBaton->readQueueLock = hsReadQueueLock;
    canHsProcessReadBaton->readQueueNotEmpty = hsReadQueueNotEmpty;
    canHsProcessReadBaton->processedQueue = processedReadQueue;
    canHsProcessReadBaton->processedQueueLock = processedReadQueueLock;
    canHsProcessReadBaton->processedAsync = processedReadAsync;

    // Initialize LS read process baton
    canProcessBaton* canLsProcessReadBaton = new canProcessBaton;
    canLsProcessReadBaton->signalDefinitions = createLsReadSignalMap();
    canLsProcessReadBaton->readQueue = lsReadQueue;
    canLsProcessReadBaton->readQueueLock = lsReadQueueLock;
    canLsProcessReadBaton->readQueueNotEmpty = lsReadQueueNotEmpty;
    canLsProcessReadBaton->processedQueue = processedReadQueue;
    canLsProcessReadBaton->processedQueueLock = processedReadQueueLock;
    canLsProcessReadBaton->processedAsync = processedReadAsync;

    // Initialize HS read work request
    uv_work_t* hsReadReq = new uv_work_t();
    hsReadReq->data = (void*) hsCanReadBaton;

    // Initialize LS read work request
    uv_work_t* lsReadReq = new uv_work_t();
    lsReadReq->data = (void*) lsCanReadBaton;

    // Initialize HS read process work request
    uv_work_t* hsProcessReadReq = new uv_work_t();
    hsProcessReadReq->data = (void*) canHsProcessReadBaton;

    // Initialize LS read process work request
    uv_work_t* lsProcessReadReq = new uv_work_t();
    lsProcessReadReq->data = (void*) canLsProcessReadBaton;

    // Initialize LS write process baton
    canProcessWriteBaton* lsCanProcessWriteBaton = new canProcessWriteBaton;
    lsCanProcessWriteBaton->messageDefinitions = createHsWriteSignalMap();
    lsCanProcessWriteBaton->writeQueue = lsWriteQueue;
    lsCanProcessWriteBaton->writeQueueLock = lsWriteQueueLock;
    lsCanProcessWriteBaton->writeQueueNotEmpty = lsWriteQueueNotEmpty;
    lsCanProcessWriteBaton->processedWriteQueue = lsProcessedWriteQueue;
    lsCanProcessWriteBaton->processedWriteQueueLock = lsProcessedWriteQueueLock;
    lsCanProcessWriteBaton->processedWriteQueueNotEmpty = lsProcessedWriteQueueNotEmpty;

    // Initialize LS write baton
    canWriteBaton* lsCanWriteBaton = new canWriteBaton;
    lsCanWriteBaton->channel = LS_CHANNEL;
    lsCanWriteBaton->baudRate = LS_BAUD;
    lsCanWriteBaton->tseg1 = LS_TSEG1;
    lsCanWriteBaton->tseg2 = LS_TSEG2;
    lsCanWriteBaton->sjw = LS_SJW;
    lsCanWriteBaton->samplePoints = LS_SAMPLE_POINTS;
    lsCanWriteBaton->syncMode = LS_SYNC_MODE;
    lsCanWriteBaton->canFlags = LS_FLAGS;
    lsCanWriteBaton->processedWriteQueue = lsProcessedWriteQueue;
    lsCanWriteBaton->processedWriteQueueLock = lsProcessedWriteQueueLock;
    lsCanWriteBaton->processedWriteQueueNotEmpty = lsProcessedWriteQueueNotEmpty;

    // Initialize LS process work request
    uv_work_t* lsProcessWriteReq = new uv_work_t();
    lsProcessWriteReq->data = (void*) lsCanProcessWriteBaton;
    
    // Initialize LS write work request
    uv_work_t* lsWriteReq = new uv_work_t();
    lsWriteReq->data = (void*) lsCanWriteBaton;
     
    // Start all our threads
    uv_loop_t* loop = uv_default_loop();
    uv_async_init(loop, processedReadAsync, ExecuteCallbacks);
    uv_queue_work(loop, hsReadReq, ReadMessages, NULL);
    uv_queue_work(loop, lsReadReq, ReadMessages, NULL);
    uv_queue_work(loop, hsProcessReadReq, ProcessMessages, NULL);
    uv_queue_work(loop, lsProcessReadReq, ProcessMessages, NULL);
    uv_queue_work(loop, lsProcessWriteReq, ProcessWriteMessages, NULL);
    uv_queue_work(loop, lsWriteReq, SendWriteMessages, NULL);

    return Undefined();
}

/*
Initializes module. Adds functions to module.
*/
void RegisterModule(Handle<Object> target) {
    context = Persistent<Object>::New(target);
    target->Set(String::NewSymbol("start"),
        FunctionTemplate::New(Start)->GetFunction());
    target->Set(String::NewSymbol("write"),
        FunctionTemplate::New(Write)->GetFunction());
}

NODE_MODULE(canReadWriter, RegisterModule);
