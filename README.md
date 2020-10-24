# Building
```
cd build
cmake .
make
```

# Running
Setting username and listening at 127.0.0.1:8080
```
./pchat -n username -l 127.0.0.1:8080
```


# Limitatons/TODOs
* IPv4-only for simplicity
* SIGTERM is not cross-platform but helps to make correct shutdown on Linux

