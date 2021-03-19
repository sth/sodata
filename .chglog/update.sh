#!/bin/bash
set -eu

NEXT="${1:-master}"

cd "$(dirname $0)/.."
git-chglog --next-tag "$NEXT" > .chglog/CHANGELOG.raw.md
git diff .chglog/CHANGELOG.raw.md

echo
echo "Edit main changelog with:"
echo "   vim -o CHANGELOG.md .chglog/CHANGELOG.raw.md"
