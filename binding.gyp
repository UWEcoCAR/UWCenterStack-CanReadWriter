{
    "targets": [
        {
            "target_name": "canReadWriter",
            "conditions": [
                ["OS=='linux'", {
                    "sources": [ "canReadWriter.cpp" ],
                    "cflags_cc": [ "-std=gnu++11" ],
                    "libraries": [ "/usr/lib/libcanlib.so" ],
                    "include_dirs": [ "/usr/include" ]  
                }],
                ["OS=='mac' or OS=='win'", {
                    "sources": [ "canReadWriterMacWin.cpp" ],
                }], 
            ]
            
        }
    ]
}
