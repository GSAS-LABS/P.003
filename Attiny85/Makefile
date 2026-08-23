.PHONY: install update build fuses upload

PIO := pio

install:
	@echo "==> Installing PlatformIO..."
	@if ! command -v brew >/dev/null 2>&1; then \
		echo "ERROR: Homebrew is not installed."; \
		echo "Install Homebrew first: https://brew.sh"; \
		exit 1; \
	fi

	@if ! command -v $(PIO) >/dev/null 2>&1; then \
		echo "==> PlatformIO not found. Installing..."; \
		brew install platformio; \
	else \
		echo "==> PlatformIO already installed."; \
	fi

	@echo "==> PlatformIO version:"
	@$(PIO) --version

update:
	@echo "==> Updating PlatformIO..."
	@$(PIO) upgrade
	@echo "==> PlatformIO version:"
	@$(PIO) --version

build:
	@echo "==> Building firmware..."
	@$(PIO) run

fuses:
	@echo "==> Programming ATtiny85 fuses..."
	@$(PIO) run -t fuses

upload:
	@echo "==> Uploading firmware..."
	@$(PIO) run -t upload
