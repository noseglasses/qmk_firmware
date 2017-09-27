BOOTMAGIC_ENABLE=no
COMMAND_ENABLE=no
SLEEP_LED_ENABLE=no
FORCE_NKRO ?= yes
DEBUG_ENABLE = no
CONSOLE_ENABLE = yes
TAP_DANCE_ENABLE = no
PAPAGENO_ENABLE = yes
KEYLOGGER_ENABLE ?= no
UCIS_ENABLE = no
MOUSEKEY_ENABLE = no

AUTOLOG_ENABLE ?= no

ifeq (${FORCE_NKRO},yes)
OPT_DEFS += -DFORCE_NKRO
endif

ifeq (${AUTOLOG_ENABLE},yes)
KEYLOGGER_ENABLE = yes
OPT_DEFS += -DAUTOLOG_ENABLE
endif

ifeq (${KEYLOGGER_ENABLE},yes)
OPT_DEFS += -DKEYLOGGER_ENABLE
CONSOLE_ENABLE = yes
endif

OPT_DEFS += -DUSER_PRINT

# Context switching is not required when
# using Papageno with qmk. Thus we disabled it.
#
OPT_DEFS += -DPAPAGENO_DISABLE_CONTEXT_SWITCHING

# Uncomment the following line to only preprocess all files.
# This can be helpful for detecting preprocessor macro related
# errors by checking the pre-processed code.
#
# OPT_DEFS += -E

# Debugging switches
#
# OPT_DEFS += -DDEBUG_PAPAGENO
# OPT_DEFS += -DPAPAGENO_HAVE_ASSERTIONS
# OPT_DEFS += -DPAPAGENO_PRINT_SELF_ENABLED=0

# The default behavior is to first check it an 
# input is defined through the keypos of a key.
# If not then we check the assigned keycode.
# 
# By defining PPG_QMK_REVERSE_KEYPOS_TO_KEYCODE_PRECEDENCE
# this order can be reversed.
#
OPT_DEFS += -DPPG_QMK_REVERSE_KEYPOS_TO_KEYCODE_PRECEDENCE

KEYMAP_VERSION = $(shell \
 if [ -d "${KEYMAP_PATH}/.git" ]; then \
  cd "${KEYMAP_PATH}" && git describe --abbrev=6 --dirty --always --tags --match 'v*' 2>/dev/null; \
 else echo QMK; fi)

KEYMAP_BRANCH = $(shell \
 if [ -d "${KEYMAP_PATH}/.git" ]; then \
  cd "${KEYMAP_PATH}"; \
 fi; \
 git rev-parse --abbrev-ref HEAD 2>/dev/null)

OPT_DEFS += -DKEYMAP_VERSION=\"$(KEYMAP_VERSION)\\\#$(KEYMAP_BRANCH)\"

ifndef QUANTUM_DIR
	include ../../../../Makefile
endif
