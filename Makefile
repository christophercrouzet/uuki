PROJECT_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# ------------------------------------------------------------------------------

ifndef outdir
	OUT_DIR := build
else
	OUT_DIR := $(outdir)
endif

ifndef config
	CONFIG := debug
else ifneq "$(filter-out debug release all,$(config))" ""
	$(error the 'config' option is not valid)
else ifneq "$(filter all,$(config))" ""
	CONFIG := debug release
else
	CONFIG := $(config)
endif

ifdef cc
	CC := $(cc)
endif

# ------------------------------------------------------------------------------

FILES :=

# ------------------------------------------------------------------------------

# $(1): build directory.
# $(2): rule.
define w_forward_rule_impl =
$(MAKE) -C $(1) -s $(2)
endef

# Forward a rule to the generated Makefiles.
# $(1): rule.
define w_forward_rule =
$(foreach _x,$(BUILD_DIRS), $(call                                             \
	w_forward_rule_impl,$(_x),$(1)))
endef

# Recursively find files.
# $(1): parent directory.
# $(2): filters.
define w_find_files =
$(foreach _x,$(wildcard $(1:=/*)),$(call \
	w_find_src_files,$(_x),$(2)) $(filter $(2),$(_x)))
endef

# ------------------------------------------------------------------------------

# Create a Makefile rule.
# $(1): configuration.
define w_create_makefile_rule =
$(OUT_DIR)/$(1)/Makefile:
	@ mkdir -p $(OUT_DIR)/$(1)
	@ cd $(OUT_DIR)/$(1)                                                       \
	    && export CC="$(cc)"                                                   \
	    && cmake                                                               \
	        -DCMAKE_BUILD_TYPE=$(1)                                            \
	        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON                                 \
	        $(PROJECT_DIR)

BUILD_DIRS += $(OUT_DIR)/$(1)
MAKE_FILES += $(OUT_DIR)/$(1)/Makefile
endef

$(foreach _config,$(CONFIG),$(eval $(call                                      \
	w_create_makefile_rule,$(_config))))

# ------------------------------------------------------------------------------

# Create a package rule.
# $(1): name.
define w_create_pkg_rule =
$(1): $(MAKE_FILES)
	@ $(call w_forward_rule,$(1))

.PHONY: $(1)
endef

PKGS := $(notdir $(wildcard src/uuki/*))

# Create the package rules.
$(foreach _x,$(PKGS),$(eval $(call                                             \
    w_create_pkg_rule,$(_x))))

FILES += $(call w_find_src_files,src/uuki,%.h %.c)

# ------------------------------------------------------------------------------

API_H_FILES := $(wildcard src/uuki/api/*.h)
PUBLIC_H_FILES := $(patsubst src/uuki/api/%,include/uuki/%,$(API_H_FILES))

$(PUBLIC_H_FILES): include/uuki/%: src/uuki/api/%
	@ mkdir -p $(dir $@)
	@ echo "#include \"../../$<\"" > $@

includes: $(PUBLIC_H_FILES)

.PHONY: includes

# ------------------------------------------------------------------------------

uuki: includes $(MAKE_FILES)
	@ $(call w_forward_rule,uuki)

.PHONY: uuki

# ------------------------------------------------------------------------------

# Create a test rule (implementation).
# $(1): file.
# $(2): name.
define w_create_test_rule_impl =
$(2): $(MAKE_FILES)
	@ $(call w_forward_rule,$(2))

FILES += $(1)

.PHONY: $(2)
endef

# Create a test rule.
# $(1): file.
define w_create_test_rule =
$(call w_create_test_rule_impl,                                                \
    $(1),                                                                      \
    test-$(subst /,-,$(patsubst tests/%.c,%,$(1))))
endef

# Create the test rules.
$(foreach _x,$(call w_find_files,tests,%.c),$(eval $(call                      \
    w_create_test_rule,$(_x))))

tests: $(MAKE_FILES)
	@ $(call w_forward_rule,tests)

test: tests
	@ $(call w_forward_rule,ARGS="--output-on-failure" test)

.PHONY: tests test

# ------------------------------------------------------------------------------

# Run the formatter on a file.
# $(1): file.
define w_format =
clang-format -style=file $(1) | diff --color -u $(1) -;
endef

format:
	@ $(foreach _file,$(FILES),$(call                                          \
	    w_format,$(_file)))

tidy: $(MAKE_FILES)
	@ clang-tidy $(FILES)                                                      \
	    -p $(firstword $(BUILD_DIRS))/compile_commands.json                    \
	    -- -Isrc

.PHONY: format tidy

# ------------------------------------------------------------------------------

coverity: clean
	@ cov-build --dir cov-int make tests -j 4
	@ tar czvf uuki.tgz cov-int

.PHONY: coverity

# ------------------------------------------------------------------------------

clean:
	@ rm -rf $(OUT_DIR) cov-int uuki.tgz

.PHONY: clean

# ------------------------------------------------------------------------------

all: uuki

.PHONY: all

.DEFAULT_GOAL := all
