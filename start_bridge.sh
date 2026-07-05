#!/bin/bash

# 1. Asegurar que el PATH incluya las rutas comunes de Node en macOS (Homebrew/Intel o Apple Silicon)
export PATH=/usr/local/bin:/opt/homebrew/bin:/usr/bin:/bin:$PATH

# Nota: Si instalaste Node vía NVM, descomenta las siguientes líneas en su lugar:
export NVM_DIR="/Users/santiago/Library/Application Support/Herd/config/nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm

load-nvmrc() {
  local nvmrc_path
  nvmrc_path="$(nvm_find_nvmrc)"

  if [ -n "$nvmrc_path" ]; then
    local nvmrc_node_version
    nvmrc_node_version=$(nvm version "$(cat "${nvmrc_path}")")

    if [ "$nvmrc_node_version" = "N/A" ]; then
      nvm install
    elif [ "$nvmrc_node_version" != "$(nvm version)" ]; then
      nvm use
    fi
  elif [ -n "$(PWD=$OLDPWD nvm_find_nvmrc)" ] && [ "$(nvm version)" != "$(nvm version default)" ]; then
    echo "Reverting to nvm default version"
    nvm use default
  fi
}

# 2. Navegar al repositorio y ejecutar el bridge
cd /Users/santiago/Repos/xplane-bridge
load-nvmrc
node dist/bridge.js