FROM gcc:9.4.0 as build

ARG INCOMING_MESSAGE_SET_NAME=oldstate
ARG OUTGOING_MESSAGE_SET_NAME=newstate
ARG TRANSFORM_NAME=Old2NewState

WORKDIR /app

RUN apt update && apt install -y g++ make python3 python3-dev python3-pip pkg-config curl \
    libtool-bin autopoint autoconf automake cmake bison gettext \
    zlib1g-dev libsasl2-dev libzstd-dev libssl-dev liblz4-dev cmake gdb

COPY code-gen/ ./code-gen/
COPY Makefile ./
COPY transform_msg_wrapper.cpp ./

RUN cd code-gen/out/$INCOMING_MESSAGE_SET_NAME && make clean && make ffi=y
RUN cd code-gen/out/$OUTGOING_MESSAGE_SET_NAME && make clean && make ffi=y
RUN make INCOMING=$INCOMING_MESSAGE_SET_NAME OUTGOING=$OUTGOING_MESSAGE_SET_NAME TRANSFORM_NAME=$TRANSFORM_NAME

RUN pip3 install numpy==1.20.1
COPY exercise.py ./

CMD [ "python3", "exercise.py" ]