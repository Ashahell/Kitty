ENGINE_FLAGS := -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror
RELEASE_FLAGS := -O2
DEBUG_FLAGS := -O0 -g -DKITTY_DEBUG
INC_RT := -Isrc/include -Isrc/platform/rt
INC_RENDER := -Isrc/include -Isrc/platform/rt -Isrc/render
INC_AUDIO := -Isrc/include -Isrc/platform/rt -Isrc/audio
INC_GURU := -Isrc/include -Isrc/platform/rt -Isrc/guru
INC_BENCH := -Isrc/include -Isrc/platform/rt -Isrc/benchmark -Isrc/render
INC_LINUX := -Isrc/include -Isrc/platform/rt -Isrc/platform/linux
INC_APP := -Isrc/include -Isrc/platform/rt -Isrc/platform/linux -Isrc/app -Isrc/render -Isrc/audio -Isrc/guru -Isrc/benchmark
INC_TEST := -Isrc/include -Isrc/platform/rt -Isrc/platform/linux -Isrc/render -Isrc/audio -Isrc/guru -Isrc/benchmark
LIBS := -lvulkan -lSDL2 -lm

ENGINE_SRC := $(shell find src -name '*.cpp' -not -path 'src/platform/aros/*' | sort)
ENGINE_REL := $(patsubst src/%.cpp,%.cpp,$(ENGINE_SRC))
REL_OBJ := $(ENGINE_REL:.cpp=.o)
REL_OBJ_DEBUG := $(ENGINE_REL:.cpp=.o)
LINUX_OBJ := $(addprefix build/linux/obj/,$(REL_OBJ))
DEBUG_OBJ := $(addprefix build/linux-debug/obj/,$(REL_OBJ_DEBUG))

VK4A ?= $(HOME)/Work/projects/Vulkan4Aros
AROS_SYSROOT ?= $(AROS_SDK_INCLUDE:%/include=%)
AROS_CXX_FLAGS := $(AROS_TARGET_CFLAGS) --sysroot=$(AROS_SYSROOT) -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -fno-pic -fno-pie -fno-stack-protector -fno-strict-aliasing -mno-ms-bitfields -O2
AROS_CXX_DEBUG := -g -DKITTY_DEBUG
ARO_CC_FLAGS := $(AROS_TARGET_CFLAGS) --sysroot=$(AROS_SYSROOT) -std=c11 -Dasm=__asm__ -ffp-contract=off -Wall -Wextra -Werror -fno-pic -fno-pie -fno-stack-protector -fno-strict-aliasing -mno-ms-bitfields -O2
ARO_ALL_REL := $(filter-out platform/linux/%,$(ENGINE_REL)) platform/aros/plat.cpp
ARO_REL := $(ARO_ALL_REL:.cpp=.o)
ARO_OBJ := $(addprefix build/aros/obj/,$(ARO_REL)) build/aros/obj/platform/aros/aros_wsi.o
ARO_DEBUG_OBJ := $(addprefix build/aros-debug/obj/,$(ARO_REL)) build/aros-debug/obj/platform/aros/aros_wsi.o
# devices/ahi.h is missing from the v1 SDK but present in the core-pc include
# tree; -idirafter keeps every SDK header authoritative and only fills the gap.
ARO_CORE_INC := $(VK4A)/src/abi/v1/core-pc-x86_64/bin/pc-x86_64/AROS/Developer/include
INC_AROS := -Isrc/include -Isrc/platform/rt -Isrc/platform/aros -idirafter $(ARO_CORE_INC)
INC_WSI_AROS := -Isrc/include -I$(VK4A)/src/vulkan/loader
INC_APP_AROS := -Isrc/include -Isrc/platform/rt -Isrc/platform/aros -Isrc/app -Isrc/render -Isrc/audio -Isrc/guru -Isrc/benchmark

TOOLS := build/tools/kitty-lint build/tools/kitty-shaders
TESTS := build/tests/t-modplay build/tests/t-vkresult build/tests/t-tier build/tests/t-selftest build/tests/t-pacing build/tests/t-guru build/tests/t-rtfmt build/tests/t-trap
SHADERS := $(patsubst shaders/%.vert,build/spv/%.spv,$(patsubst shaders/%.frag,build/spv/%.spv,$(wildcard shaders/*.vert shaders/*.frag)))

TEST_ENGINE := build/tests/obj/vk_ctx.o build/tests/obj/tier.o build/tests/obj/selftest.o \
               build/tests/obj/modplay.o build/tests/obj/audio.o build/tests/obj/guru.o \
               build/tests/obj/plat.o build/tests/obj/rt_fmt.o
UBSAN_ENGINE := build/tests-ubsan/obj/modplay.o build/tests-ubsan/obj/audio.o \
                build/tests-ubsan/obj/guru.o build/tests-ubsan/obj/plat.o build/tests-ubsan/obj/rt_fmt.o

.PHONY: all linux linux-debug aros aros-debug aros-pkg aros-precheck shaders tools tests test test-ubsan clean

all: linux tests tools

linux: shaders build/linux/kitty

linux-debug: shaders build/linux-debug/kitty

aros: build/aros/kitty

aros-debug: build/aros-debug/kitty

build/linux/kitty: $(LINUX_OBJ) | build/linux
	g++ $(LINUX_OBJ) -o $@ $(LIBS) -Wl,--as-needed

build/linux-debug/kitty: $(DEBUG_OBJ) | build/linux-debug
	g++ $(DEBUG_OBJ) -o $@ $(LIBS) -Wl,--as-needed

build/linux/obj/render/%.o: src/render/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_RENDER) -MMD -MP -c $< -o $@

build/linux/obj/audio/%.o: src/audio/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_AUDIO) -MMD -MP -c $< -o $@

build/linux/obj/guru/%.o: src/guru/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_GURU) -MMD -MP -c $< -o $@

build/linux/obj/benchmark/%.o: src/benchmark/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_BENCH) -MMD -MP -c $< -o $@

build/linux/obj/app/%.o: src/app/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_APP) -MMD -MP -c $< -o $@

build/linux/obj/platform/linux/%.o: src/platform/linux/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_LINUX) -MMD -MP -c $< -o $@

build/linux/obj/platform/rt/%.o: src/platform/rt/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) -fno-stack-protector $(INC_RT) -MMD -MP -c $< -o $@

build/linux-debug/obj/render/%.o: src/render/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(DEBUG_FLAGS) $(INC_RENDER) -MMD -MP -c $< -o $@

build/linux-debug/obj/audio/%.o: src/audio/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(DEBUG_FLAGS) $(INC_AUDIO) -MMD -MP -c $< -o $@

build/linux-debug/obj/guru/%.o: src/guru/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(DEBUG_FLAGS) $(INC_GURU) -MMD -MP -c $< -o $@

build/linux-debug/obj/benchmark/%.o: src/benchmark/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(DEBUG_FLAGS) $(INC_BENCH) -MMD -MP -c $< -o $@

build/linux-debug/obj/app/%.o: src/app/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(DEBUG_FLAGS) $(INC_APP) -MMD -MP -c $< -o $@

build/linux-debug/obj/platform/linux/%.o: src/platform/linux/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(DEBUG_FLAGS) $(INC_LINUX) -MMD -MP -c $< -o $@

build/linux-debug/obj/platform/rt/%.o: src/platform/rt/%.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(DEBUG_FLAGS) -fno-stack-protector $(INC_RT) -MMD -MP -c $< -o $@

# --- AROS cross-build (D31): toolchain only from the environment ---
aros-precheck: | build/tools/kitty-lint
	@sdk=`realpath "$(AROS_SDK_INCLUDE)" 2>/dev/null`; want=`realpath -m $(VK4A)/src/abi/v1/sdk/Developer/include`; \
	 if [ -z "$(AROS_SDK_INCLUDE)" ] || [ "$(AROS_ABI_EXPECT_MAJOR)" != 1 ] || [ -z "$$sdk" ] || [ "$$sdk" != "$$want" ]; then \
	 printf 'stale SDK: %s\n' '$(AROS_SDK_INCLUDE)' 1>&2; exit 1; \
	 fi
	@build/tools/kitty-lint src || exit 1
	@s=src/platform/aros; ok=1; \
	 if [ -d "$$s" ]; then for spec in \
	   'WaitTOF[[:space:]]*\(	WaitTOF' \
	   'Delay[[:space:]]*\(	Delay' \
	   'RemLibrary[[:space:]]*\(	RemLibrary' \
	   '\bAvail\b	Avail' \
	   'CloseLibrary[[:space:]]*\([^)]*vk_lib	CloseLibrary'; do \
	 pat=$${spec%%	*}; nm=$${spec##*	}; \
	 if grep -rqE "$$pat" "$$s"; then echo "banned API $$nm" 1>&2; ok=0; fi; \
	 done; fi; \
	 [ "$$ok" = 1 ] || exit 1

build/aros/obj/render/%.o: src/render/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(INC_RENDER) -MMD -MP -c $< -o $@

build/aros/obj/audio/%.o: src/audio/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(INC_AUDIO) -MMD -MP -c $< -o $@

build/aros/obj/guru/%.o: src/guru/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(INC_GURU) -MMD -MP -c $< -o $@

build/aros/obj/benchmark/%.o: src/benchmark/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(INC_BENCH) -MMD -MP -c $< -o $@

build/aros/obj/app/%.o: src/app/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(INC_APP_AROS) -MMD -MP -c $< -o $@

build/aros/obj/platform/aros/%.o: src/platform/aros/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(INC_AROS) -MMD -MP -c $< -o $@

build/aros/obj/platform/aros/aros_wsi.o: src/platform/aros/aros_wsi.c | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CC) $(ARO_CC_FLAGS) $(INC_WSI_AROS) -MMD -MP -c $< -o $@

build/aros/obj/platform/rt/%.o: src/platform/rt/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(INC_RT) -MMD -MP -c $< -o $@

build/aros-debug/obj/render/%.o: src/render/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(AROS_CXX_DEBUG) $(INC_RENDER) -MMD -MP -c $< -o $@

build/aros-debug/obj/audio/%.o: src/audio/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(AROS_CXX_DEBUG) $(INC_AUDIO) -MMD -MP -c $< -o $@

build/aros-debug/obj/guru/%.o: src/guru/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(AROS_CXX_DEBUG) $(INC_GURU) -MMD -MP -c $< -o $@

build/aros-debug/obj/benchmark/%.o: src/benchmark/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(AROS_CXX_DEBUG) $(INC_BENCH) -MMD -MP -c $< -o $@

build/aros-debug/obj/app/%.o: src/app/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(AROS_CXX_DEBUG) $(INC_APP_AROS) -MMD -MP -c $< -o $@

build/aros-debug/obj/platform/aros/%.o: src/platform/aros/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(AROS_CXX_DEBUG) $(INC_AROS) -MMD -MP -c $< -o $@

build/aros-debug/obj/platform/aros/aros_wsi.o: src/platform/aros/aros_wsi.c | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CC) $(ARO_CC_FLAGS) -g $(INC_WSI_AROS) -MMD -MP -c $< -o $@

build/aros-debug/obj/platform/rt/%.o: src/platform/rt/%.cpp | aros-precheck
	@mkdir -p $(dir $@)
	$(AROS_CXX) $(AROS_CXX_FLAGS) $(AROS_CXX_DEBUG) $(INC_RT) -MMD -MP -c $< -o $@

build/aros/kitty: $(ARO_OBJ) | aros-precheck build/aros
	@PATH="$(AROS_COLLECT_AROS_DIR):$$PATH" $(AROS_CXX) $(AROS_TARGET_CFLAGS) --sysroot=$(AROS_SYSROOT) $(ARO_OBJ) -o $@
	$(aros-gate)

build/aros-debug/kitty: $(ARO_DEBUG_OBJ) | aros-precheck build/aros-debug
	@PATH="$(AROS_COLLECT_AROS_DIR):$$PATH" $(AROS_CXX) $(AROS_TARGET_CFLAGS) --sysroot=$(AROS_SYSROOT) $(ARO_DEBUG_OBJ) -o $@
	$(aros-gate)

aros-pkg: build/aros/kitty build/aros-debug/kitty $(SHADERS) | aros-precheck build/aros-pkg build/aros-pkg/spv
	@cp -f build/aros/kitty build/aros-pkg/kitty
	@cp -f build/aros-debug/kitty build/aros-pkg/kitty-debug
	@cp -f tests/fixtures/test.mod build/aros-pkg/test.mod
	@cp -f build/spv/*.spv build/aros-pkg/spv/
	@if [ -e tests/aros/m0b-run.aros ]; then cp -f tests/aros/*.aros build/aros-pkg/; else echo "aros-pkg: guest scripts are missing (W4 adds tests/aros/ in a later phase; package ships what it has)"; fi
	@( cd build/aros-pkg && find . -type f ! -name MANIFEST.sha256 -print0 | sort -z | xargs -0 sha256sum > MANIFEST.sha256 )

aros-gate = @tt=`$(AROS_READELF) -h $@ | awk '/^  Type:/{print $$2}'`; \
	 oo=`$(AROS_READELF) -h $@ | awk -F: '/OS\/ABI/{gsub(/ /,"",$$2);print $$2}'`; \
	 aa=`$(AROS_READELF) -h $@ | grep 'ABI Version' | awk '{print $$NF}'`; \
	 uu=`$(AROS_READELF) -s -W $@ | awk '$$5=="GLOBAL" && $$7=="UND" && $$8!=""' | wc -l`; \
	 rr=`$(AROS_OBJDUMP) -d $@ | grep -cE 'movq?[[:space:]]+%rax,[[:space:]]*%r12'; :`; \
	 echo "aros-gates: type=$$tt osabi=$$oo abi=$$aa und=$$uu r12=$$rr"; \
	 if [ "$$rr" != 0 ]; then echo "GATE FAIL: $$rr stale-r12 call sites" 1>&2; exit 1; fi; \
	 if [ "$$tt" != REL ] || [ "$$oo" != AROS ] || [ "$$aa" != 1 ] || [ "$$uu" != 0 ]; then exit 1; fi

build/tests/obj/vk_ctx.o: src/render/vk_ctx.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_RENDER) -c $< -o $@

build/tests/obj/tier.o: src/render/tier.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_RENDER) -c $< -o $@

build/tests/obj/selftest.o: src/render/selftest.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_RENDER) -c $< -o $@

build/tests/obj/modplay.o: src/audio/modplay.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_AUDIO) -c $< -o $@

build/tests/obj/audio.o: src/audio/audio.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_AUDIO) -c $< -o $@

build/tests/obj/guru.o: src/guru/guru.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_GURU) -c $< -o $@

build/tests/obj/bench.o: src/benchmark/bench.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_BENCH) -c $< -o $@

build/tests/obj/plat.o: src/platform/linux/plat.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) $(INC_LINUX) -c $< -o $@

build/tests/obj/rt_fmt.o: src/platform/rt/rt_fmt.cpp
	@mkdir -p $(dir $@)
	g++ $(ENGINE_FLAGS) $(RELEASE_FLAGS) -fno-stack-protector $(INC_RT) -c $< -o $@

build/tests/t-%.o: tests/t-%.cpp
	@mkdir -p $(dir $@)
	g++ -std=c++20 -O2 -g -Wall -Wextra -Werror $(INC_TEST) -c $< -o $@

build/tests/t-vkresult: build/tests/t-vkresult.o build/tests/obj/vk_ctx.o
	g++ $^ -o $@ -lvulkan -Wl,--as-needed

build/tests/t-tier: build/tests/t-tier.o build/tests/obj/tier.o
	g++ $^ -o $@

build/tests/t-selftest: build/tests/t-selftest.o build/tests/obj/selftest.o
	g++ $^ -o $@

build/tests/t-pacing: build/tests/t-pacing.o
	g++ $^ -o $@

build/tests/t-guru: build/tests/t-guru.o build/tests/obj/guru.o build/tests/obj/plat.o build/tests/obj/rt_fmt.o
	g++ $^ -o $@ $(LIBS) -Wl,--as-needed

build/tests/t-rtfmt: build/tests/t-rtfmt.o build/tests/obj/rt_fmt.o
	g++ $^ -o $@

build/tests/t-trap: build/tests/t-trap.o build/tests/obj/plat.o build/tests/obj/rt_fmt.o
	g++ $^ -o $@ $(LIBS) -Wl,--as-needed

build/tests/t-modplay: build/tests/t-modplay.o build/tests/obj/modplay.o build/tests/obj/audio.o \
                       build/tests/obj/guru.o build/tests/obj/plat.o \
                       build/tests/obj/rt_fmt.o
	g++ $^ -o $@ $(LIBS) -Wl,--as-needed

tests: $(TESTS)

test: $(TESTS)
	@f=0; \
	for t in t-modplay t-vkresult t-tier t-selftest t-pacing t-guru t-rtfmt; do \
	  if [ "$$t" = t-modplay ]; then build/tests/$$t tests/fixtures/test.mod > build/tests/$$t.log 2>&1; else build/tests/$$t > build/tests/$$t.log 2>&1; fi; \
	  rc=$$?; echo "$$t rc=$$rc"; [ $$rc = 0 ] || f=1; \
	done; exit $$f

tools: $(TOOLS)

build/tools/kitty-lint: tools/kitty-lint.cpp
	@mkdir -p $(dir $@)
	g++ -std=c++20 -O2 -Wall -Wextra -Werror tools/kitty-lint.cpp -o $@

build/tools/kitty-shaders: tools/kitty-shaders.cpp
	@mkdir -p $(dir $@)
	g++ -std=c++20 -O2 -Wall -Wextra -Werror tools/kitty-shaders.cpp -o $@

shaders: build/tools/kitty-shaders
	@mkdir -p build/spv
	build/tools/kitty-shaders shaders build/spv

$(SHADERS): shaders
	@true

test-ubsan:
	@mkdir -p build/tests-ubsan/obj
	g++ -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -O1 -g \
	    -fsanitize=undefined -fno-sanitize-recover $(INC_AUDIO) -c src/audio/modplay.cpp -o build/tests-ubsan/obj/modplay.o
	g++ -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -O1 -g \
	    -fsanitize=undefined -fno-sanitize-recover $(INC_AUDIO) -c src/audio/audio.cpp -o build/tests-ubsan/obj/audio.o
	g++ -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -O1 -g \
	    -fsanitize=undefined -fno-sanitize-recover $(INC_GURU) -c src/guru/guru.cpp -o build/tests-ubsan/obj/guru.o
	g++ -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -O1 -g \
	    -fsanitize=undefined -fno-sanitize-recover $(INC_LINUX) -c src/platform/linux/plat.cpp -o build/tests-ubsan/obj/plat.o
	g++ -std=c++20 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra -Werror -O1 -g \
	    -fsanitize=undefined -fno-sanitize-recover -fno-stack-protector $(INC_RT) -c src/platform/rt/rt_fmt.cpp -o build/tests-ubsan/obj/rt_fmt.o
	g++ -std=c++20 -O1 -g -fsanitize=undefined -fno-sanitize-recover $(INC_TEST) -c tests/t-modplay.cpp -o build/tests-ubsan/t-modplay.o
	g++ build/tests-ubsan/t-modplay.o $(UBSAN_ENGINE) -o build/tests-ubsan/t-modplay \
	    -fsanitize=undefined -fno-sanitize-recover $(LIBS) -Wl,--as-needed
	build/tests-ubsan/t-modplay tests/fixtures/test.mod

-include $(LINUX_OBJ:.o=.d) $(DEBUG_OBJ:.o=.d) $(ARO_OBJ:.o=.d) $(ARO_DEBUG_OBJ:.o=.d)

build/linux build/linux-debug build/tools build/tests build/tests-ubsan build/spv build/aros build/aros-debug build/aros-pkg build/aros-pkg/spv:
	@mkdir -p $@

clean:
	rm -rf build/linux build/linux-debug build/tools build/tests build/tests-ubsan build/spv build/aros build/aros-debug build/aros-pkg
