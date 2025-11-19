# V5dbg build configuration
# Makefiles are shit... 11/18/25

# Enable v5dbg? When disabled .stub.h files are included to overwrite macros
ENABLE_DEBUGGER := false

######## Apply configuration, DO NOT MODIFY ########

EXTRA_CXXFLAGS += $(if $(filter $(ENABLE_DEBUGGER),true),"",-DV5DBG_DISABLE)