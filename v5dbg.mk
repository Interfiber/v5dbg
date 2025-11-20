# V5dbg build configuration

# About: When 'true' debugger functionality is enabled, when 'false' its disabled
# Cont: Enables the including of .stub.h files to replace debugger functionality
# Note: Enable this for a final competition build!
ENABLE_DEBUGGER := true

######## Apply configuration, DO NOT MODIFY ########

EXTRA_CXXFLAGS += $(if $(filter $(ENABLE_DEBUGGER),true),,-DV5DBG_DISABLE)

stub:
	@echo "Calling stub compiler..."
	@python3 stub.py