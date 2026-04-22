#!/usr/bin/env bash

set -xeu

if [ "$#" -ne 1 ]; then
    echo "ERROR: 'expected only test executable" >&2
    exit 1
fi

ROOT_DIR="$(realpath "$0/../..")"

EXECUTABLE="$1"

FILE_NAME="$(basename "$EXECUTABLE")"

VALGRIND_ROOT_DIR="${ROOT_DIR}/valgrind"

if ! [ -d "$VALGRIND_ROOT_DIR" ]; then
    mkdir -p "$VALGRIND_ROOT_DIR"

    echo "*" >"${VALGRIND_ROOT_DIR}/.gitignore"
fi

VALGRIND_DIR="${VALGRIND_ROOT_DIR}/${FILE_NAME}"

if ! [ -d "$VALGRIND_DIR" ]; then
    mkdir -p "$VALGRIND_DIR"
fi

VALGRIND_OUTPUT="${VALGRIND_DIR}/output.xml"

ERROR=""

valgrind \
    --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes \
    --xml=yes \
    --xml-file="$VALGRIND_OUTPUT" "$EXECUTABLE" || ERROR="1"

valgrind-ci "$VALGRIND_OUTPUT" \
    --output-dir "${VALGRIND_DIR}/report/" \
    --source-dir="${ROOT_DIR}" \
    --summary

valgrind-ci "$VALGRIND_OUTPUT" --summary --abort-on-errors

if [ -n "$ERROR" ]; then
    exit "$ERROR"
fi
