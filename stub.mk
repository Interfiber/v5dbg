# Handles generating the stubbed versions of functions when enabled via a config variable

define stub_generate_file()
	src_file = $(1)
	@echo "Generate stub: $(src_file)"
endef

