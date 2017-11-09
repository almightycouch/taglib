ERLANG_PATH ?= $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CXXFLAGS  	?= -O2 -Wall -Wextra -Wno-unused-parameter -I$(ERLANG_PATH)
LDFLAGS 	+= -ltag
MAKEFLAGS 	+= --silent

ifneq ($(OS),Windows_NT)
	CXXFLAGS += -fPIC
	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup
	endif
endif

all: priv/taglib_nif.so

priv/taglib_nif.so:
	@mkdir -p priv
	$(CXX) $(CXXFLAGS) -shared -w $(LDFLAGS) -o $@ c_src/taglib.cpp

clean:
	$(RM) -r priv/taglib_nif.so
