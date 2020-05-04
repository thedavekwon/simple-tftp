# Simple TFTP Server/Client with C++
```
git clone https://github.com/thedavekwon/simple-tftp
cd simple-tftp

mkdir build
cd build
cmake ..
make

# Server Usage
./simple-tftp-server port

# Client Usage
# mode 0: send, 1: get
./simple-tftp-server host port filename mode
```