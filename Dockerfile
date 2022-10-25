FROM ubuntu:22.04

#need to change if ports in config difference
EXPOSE 8080:8080 8010:8010

#Install main toolchain
RUN apt-get update && \
    apt-get -y install snapd && \
    apt-get -y install gcc-11 && \
    apt-get -y install python3-pip && \
    apt-get -y install ninja-build && \
    apt-get -y install cmake && \
    apt-get -y install sysstat && \
    pip3 install conan

RUN mkdir server && cd server
COPY ./ ./server

#Pull dependencies
RUN cd server/conan && conan install -s compiler.libcxx=libstdc++11 conanfile.txt --build=missing

#Builiding project
RUN cd server && rm -rf build && mkdir build && cmake -B/server/build -S/server -G Ninja -DCMAKE_BUILD_TYPE=Release && cd build && ninja

#Run server on config ip:ports
CMD cd server/build/bin && ./test_work_server
