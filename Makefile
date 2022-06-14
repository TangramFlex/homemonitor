CONTAINER_ID = $(shell docker ps -q --filter "name=ie2207")
IMAGE_ID = $(shell docker images ie2207 -q)
CXX             ?= g++
CXXFLAGS        ?= -Werror -Wall -Wmissing-declarations -Wpointer-arith \
					 -Wwrite-strings -Wcast-qual -Wcast-align -Wformat-security \
					 -Wformat-nonliteral -Wmissing-format-attribute -Winline \
					 -pedantic-errors -fstack-protector-all -D_FORTIFY_SOURCE=2 \
					 -fPIC -std=c++11 -pthread

INCOMING        ?= incoming
OUTGOING  ?= outgoing
TRANSFORM_NAME ?= incoming2outgoing
TRANSFORM_DIR ?= code-gen/out/transform

GENERICAPI_DIR   = code-gen/out/$(INCOMING)/pkg_deps/genericapi

INCLUDES         = -I$(GENERICAPI_DIR)/include \
							-Icode-gen/out/$(INCOMING)/include \
							-Icode-gen/out/$(OUTGOING)/include \
							-I$(TRANSFORM_DIR)/include

LIBS             = code-gen/out/$(INCOMING)/build/libs/libtangramgeneric.a \
					   code-gen/out/$(OUTGOING)/build/libs/libtangramgeneric.a \
					   $(GENERICAPI_DIR)/build/libs/libtangramgenericapi.a

SOURCES_CXX      = $(wildcard gen/*.cpp)
all: $(SOURCES_CXX)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -shared -o libtransformmsg.so transform_msg_wrapper.cpp $(TRANSFORM_DIR)/gen/$(TRANSFORM_NAME).cpp $(TRANSFORM_DIR)/gen/my_transform.cpp $(LIBPATHS) $(LIBS)

clean:
	@rm -f libtransformmsg.so

############ DOCKER&COMPOSE TARGETS ############
.PHONY: build-docker
build-docker: 
	@docker build -t ie2207 .

.PHONY: shell-docker
shell-docker: build-docker
	@docker run -it --rm ie2207 python3 -i exercise.py

.PHONY: run-docker
run-docker: build-docker
	@docker run -d --rm --name ie2207 -t ie2207

.PHONY: clean-docker-image
clean-docker-image: stop-docker
	@docker rmi $(IMAGE_ID)

.PHONY: stop-docker
stop-docker:
	@docker stop $(CONTAINER_ID)