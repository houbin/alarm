#
# config my own makefile. houbin
# 

# 常用配置
SHELL=/bin/sh
CC = gcc
CXX = g++
MAKE = make

# 定义目录路径
MAKE_DIR = .
INCLUDE_DIR = ${MAKE_DIR}/include
OBJ_DIR = ${MAKE_DIR}/obj
SRC_DIR = ${MAKE_DIR}/src
UTIL_DIR = ${MAKE_DIR}/util
LIB_DIR = ${MAKE_DIR}/lib
DEBUG_DIR = ${MAKE_DIR}/debug
RELEASE_DIR = ${MAKE_DIR}/release

CFLAGS = -Wall -c -DNDEBUG
FLAG_DEBUG = -g 
INCLUDE = -I${INCLUDE_DIR} -I${SRC_DIR} -I/usr/include/boost148
LIB = -lhiredis -llog4cxx -ljson -lboost_system -lboost_regex -L${OBJ_DIR} -L/usr/local/lib -levent

# 可执行程序名称
EXEC = voice_server

# 依赖文件查找路径
vpath %.h ${INCLUDE_DIR}
vpath %.cc ${SRC_DIR}:${UTIL_DIR}
vpath %.d ${OBJ_DIR}
vpath %.o ${OBJ_DIR}

# 指定SRC OBJ DEP
# 不带目录的basename
SRC_TMP := $(wildcard ${SRC_DIR}/*.cc) $(wildcard ${UTIL_DIR}/*.cc)
CXX_SRCS := $(notdir $(SRC_TMP))
CXX_OBJS := $(patsubst %.cc,%.o,${CXX_SRCS})
CXX_DEPS := $(patsubst %.cc,%.d,${CXX_SRCS})

# debug or release
DEBUG := 1
ifeq ($(DEBUG), 1)
	EXEC_DIR := $(DEBUG_DIR)
	CFLAGS := ${CFLAGS} ${FLAG_DEBUG}
else
	EXEC_DIR := ${RELEASE_DIR}
endif

# 最终可执行程序的目录和名称
EXEC := ${EXEC_DIR}/${EXEC}

# 终极目标
all : ${EXEC}

${EXEC} : ${CXX_OBJS}
	@echo 'start building $(notdir $@)'
	${CXX} $(addprefix ${OBJ_DIR}/,$(notdir $^)) ${LIB} -o $@

# 依赖关系
%.o : %.cc %.d
	@echo 'start building $(notdir $@)'
	${CXX} ${CFLAGS} $< ${INCLUDE} -o ${OBJ_DIR}/$@

${OBJ_DIR}/%.d : %.cc
	@echo 'start building $(notdir $@)'
	${CXX} $< ${INCLUDE} -MM -MD -o $@

# 将所有.d文件引入
# 作用是为了更新
-include $(addprefix ${OBJ_DIR}/,${CXX_DEPS})

# clean
clean :
	@${RM} ${OBJ_DIR}/*
	@${RM} $(EXEC)
	@clear

debug :
	@echo ${CXX_OBJS}
	@echo ${CXX_DEPS}

.PHONY : all clean debug

