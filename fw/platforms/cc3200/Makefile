APP ?= mongoose-iot
REPO_PATH ?= ../../..
SJS_CC3200_PATH = $(REPO_PATH)/fw/platforms/cc3200
SDK_VERSION ?= $(shell cat $(SJS_CC3200_PATH)/sdk.version)

REPO_ABS_PATH = $(realpath $(REPO_PATH))
APP_PATH ?= $(subst $(REPO_ABS_PATH),/src,$(CURDIR))

.PHONY: all clean

MAKEFLAGS += w

# this hack is needed to make `-$(MAKEFLAGS)` always work (notice the dash).
# Otherwise, $(MAKEFLAGS) does not contain the flag `w` when `make` runs
# directly, but it does contain this flag when it runs as a submake.
#
# see:
# - http://www.gnu.org/software/make/manual/html_node/Options_002fRecursion.html
# - http://www.gnu.org/software/make/manual/html_node/_002dw-Option.html
MAKEFLAGS += w

all clean:
	@docker run --rm -i -v $(REPO_ABS_PATH):/src $(DOCKER_EXTRA) $(SDK_VERSION) \
	  //bin/bash -c "\
	    if [ -d /src/v7 ] ; then make -C /src/v7 v7.c ; fi && \
	    if [ -d /src/krypton ] ; then make -C /src/krypton krypton.c ; fi && \
	    if [ -d /src/mongoose ] ; then make -C /src/mongoose mongoose.c mongoose.h ; fi && \
	    cd $(APP_PATH) && \
	    make -f Makefile.build APP=$(APP) REPO_PATH=/src $@ -$(MAKEFLAGS) \
	  "

debug:
	docker run --rm -it --privileged -v $(SRC_DIR):/src $(SDK) \
		/bin/bash -c "cd /src/fw/platforms/cc3200 && tools/gdb.sh"
