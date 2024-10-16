.PHONY: help
help:
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@perl -nle'print $& if m{^[a-zA-Z_-]+:.*?## .*$$}' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: install-arduino-cli
install-arduino-cli: ## Installs arduino-cli
	curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/home/pahagon/bin sh

.PHONY: install-arduino-linter
install-arduino-linter: ## Installs arduino-linter
	curl -fsSL https://raw.githubusercontent.com/arduino/arduino-lint/main/etc/install.sh | BINDIR=/home/pahagon/bin sh

.PHONY: install-fritzing
install-fritzing: ## Install fritzing
	sudo apt-get install fritzing
