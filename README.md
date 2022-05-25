![example workflow](https://github.com/maxwolf8852/NeuroSnip/actions/workflows/ci.yml/badge.svg)

# NeuroSnip
Windows snipping tool written on C++, which allows you to recognize and translate the selected text into Russian.

![alt text](https://github.com/maxwolf8852/NeuroSnip/blob/main/1.PNG?raw=true)

1. [Install](https://github.com/maxwolf8852/NeuroSnip#install)
    1. [Snipper](https://github.com/maxwolf8852/NeuroSnip#snipper)
    2. [Translator](https://github.com/maxwolf8852/NeuroSnip#translator)
    3. [Releases](https://github.com/maxwolf8852/NeuroSnip#prebuild-binaries)


# Install

## Snipper

Snipper is c++ project that allows:
+ extracting image from part of desktop
+ copying this image to buffer
+ finding text regions of interest
+ recognizing words
+ showing copyable text on GUI with confidence coloring

Installation steps:
+ Download or compile tesseract and opencv (use vcpkg for easiest installing)
+ generate grpc cxx and h files with your protoc
    + protoc -I ./ --grpc_out=. --plugin=protoc-gen-grpc=<path>\grpc_cpp_plugin.exe ./cli_g.proto
    + protoc -I ./ --cpp_out=. ./cli_g.proto
+ compile program with Qt

## Translator

```shell
cd translator/
pip3 install -r requirements.txt
python3 main.py
```

## Prebuild binaries

Coming soon!

