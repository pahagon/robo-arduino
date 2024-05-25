.ONESHELL:
APP_NAME=kit-robot-webserver-esp
APP_VSN ?= `cat 'VERSION' | cut -d '"' -f2`
BUILD ?= `git rev-parse --short HEAD`
PORT=/dev/ttyUSB0

.PHONY: help
help:
	@echo "$(APP_NAME):$(APP_VSN)-$(BUILD)"
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@perl -nle'print $& if m{^[a-zA-Z_-]+:.*?## .*$$}' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: board-list
board-list: ## Lists all avalible boards.
	arduino-cli board list

.PHONY: compile
compile: ## Compiles source code using esp profile
	arduino-cli compile --profile esp

.PHONY: compile-clean
compile-clean: ## Compiles and cleans source code using esp profile
	arduino-cli compile --clean -v --profile esp

.PHONY: upload
upload: ## Uploads source code using esp profile
	arduino-cli upload -p $(PORT) --profile esp

.PHONY: monitor
monitor: ## Opens a communication port with a board
	arduino-cli monitor -p $(PORT)
