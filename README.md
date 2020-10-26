# Building
```
cd build
cmake .
make
```
or `build.sh`

# Running
Setting three clients with user2 connecting to user1 and user3 connecting to user1/user2,
```
build/pchat -n user1 -l 127.0.0.1:8080

build/pchat -n user2 -l 127.0.0.1:8081 -c 127.0.0.1:8080

build/pchat -n user3 -l 127.0.0.1:8082 -c 127.0.0.1:8080 -c 127.0.0.1:8081
```

# Limitatons
* IPv4-only for simplicity
* Very simple text-based protocol
* Minimalistic validations
* No watermarks and limiting writing to bufferevents
* SIGTERM is not cross-platform but helps to make correct shutdown on Linux


