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
compile: replace-secrets replace-index-html ## Compiles source code using esp profile
	arduino-cli compile --profile esp

.PHONY: compile-and-revert-replaces
compile-and-revert-replaces: compile revert-index-html revert-secrets ## Compiles source code using esp profile
	@echo "Done!"

.PHONY: replace-secrets
replace-secrets: ## Replaces secrets in the source code
	@echo "Replacing secrets in the source code"
	@sed -i 's/SSID/$(shell cat .passwd | head -n 1)/g' esp.ino
	@sed -i 's/PASSWD/$(shell cat .passwd | tail -n 1)/g' esp.ino

.PHONY: revert-secrets
revert-secrets: ## Reverts secrets in the source code
	@echo "Reverting secrets in the source code"
	@sed -i 's/$(shell cat .passwd | head -n 1)/SSID/g' esp.ino
	@sed -i 's/$(shell cat .passwd | tail -n 1)/PASSWD/g' esp.ino

.PHONY: replace-index-html
replace-index-html: ## Adds index.html content in the source code
	@cp esp.ino esp.ino.bak
	@sed -i '/INDEXHTML/ {
		r ./web/index.html
		d
	}' esp.ino

.PHONY: revert-index-html
revert-index-html: ## Removes index.html content in the source code
	if [ -f esp.ino.bak ]; then
		@mv esp.ino.bak esp.ino
	fi

all: compile-and-revert-replaces

.PHONY: compile-clean
compile-clean: ## Compiles and cleans source code using esp profile
	arduino-cli compile --clean -v --profile esp

.PHONY: upload
upload: ## Uploads source code using esp profile
	arduino-cli upload -p $(PORT) --profile esp

.PHONY: monitor
monitor: ## Opens a communication port with a board
	arduino-cli monitor -p $(PORT) -c BaudRate=115200 --profile esp

.PHONY: dev-web-server
dev-web-server: ## Starts a development web server
	cd web && python3 -m http.server

.PHONY: install-websocket
install-websocket: ## Installs websocket library
	pip3 install websockets

.PHONY: dev-websocket-server
dev-websocket-server: ## Starts a development websocket server
	python3 web/websocket-server.py
