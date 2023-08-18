# TinyVisualizer

This is a set of mesh processing and visualization functions with python interface.
To install on ubuntu, use the following commands:

```sh
git clone https://github.com/gaoxifeng/TinyVisualizer
mkdir TinyVisualizer-build
cd TinyVisualizer-build
cmake ../TinyVisualizer -DPYTHON_BINDING=Python3
sudo make install -j6
```

To install on windows, use the cmake GUI interface, and in advanced setting, choose:
```sh
PYTHON_BINDING=Python3
```
Then build the INSTALL.vcxproj project via msvc. To install with python interface, you need a version of Python3 interpreter and the [SWIG](https://www.swig.org/download.html) library.