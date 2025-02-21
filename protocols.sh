#!/bin/bash

XML_PATH="/usr/share/wlr-protocols/unstable/wlr-layer-shell-unstable-v1.xml"

echo "Generating protocol files from: ${XML_PATH}"

wayland-scanner private-code \
    "${XML_PATH}" \
    wlr-layer-shell-unstable-v1-protocol.c

wayland-scanner client-header \
    "${XML_PATH}" \
    wlr-layer-shell-unstable-v1-client-protocol.h
