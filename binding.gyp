{
    "targets": [
        {
            "target_name": "canReadWriter",
            "sources": [ "canReadWriter.cpp" ],
      		"cflags_cc": [ "-std=gnu++11" ],
            "libraries": [ "/usr/lib/libcanlib.so" ],
            "include_dirs": [ "/usr/include" ]
        }
    ]
}
