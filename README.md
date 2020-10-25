# Building
```
cd build
cmake .
make
```
or `build.sh`

# Running
Setting username and listening at 127.0.0.1:8080
```
./pchat -n username -l 127.0.0.1:8080
```

# Limitatons
* IPv4-only for simplicity
* Very simple text-based protocol
* Minimalistic validations
* No watermarks and limiting writing to bufferevents
* SIGTERM is not cross-platform but helps to make correct shutdown on Linux


