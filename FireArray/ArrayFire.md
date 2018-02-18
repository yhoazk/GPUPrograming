# [Array Fire](http://arrayfire.org/docs/index.htm)


## Install:

`yum install glfw cmake`


Then download and execute the installer: http://arrayfire.com/download/

```
./arrayfire_*_Linux_x86_64.sh --exclude-subdir --prefix=/usr/local
```


### Testing the installation:

```
cp -r /usr/local/share/ArrayFire/examples .
cd examples
mkdir build
cd build
cmake ..
make
```
