#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${ROOT_DIR}/BuildHeadedGif"
ARTIFACT_DIR="${ROOT_DIR}/Artifacts/e2e_gif"
FRAMES_DIR="${ARTIFACT_DIR}/frames"
LOG_FILE="${ARTIFACT_DIR}/e2e.log"
GIF_FILE="${ARTIFACT_DIR}/memorygame_e2e.gif"
TEST_FILTER="${1:-Board_Cat_Regression.CompletingLevel1AdvancesToLevel2}"

mkdir -p "${FRAMES_DIR}"
rm -f "${FRAMES_DIR}"/*.png "${LOG_FILE}" "${GIF_FILE}" "${ARTIFACT_DIR}"/*.mp4 "${ARTIFACT_DIR}"/*.ass "${ARTIFACT_DIR}"/*.png

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DMEMORYGAME_BUILD_TESTS=ON -DMEMORYGAME_HEADED_E2E=ON
cmake --build "${BUILD_DIR}" --target memorygame_e2e

MEMORYGAME_CAPTURE_FRAMES_DIR="${FRAMES_DIR}" \
"${BUILD_DIR}/bin/memorygame_e2e" --gtest_filter="${TEST_FILTER}" | tee "${LOG_FILE}"

python3 "${ROOT_DIR}/Tests/Tools/generate_e2e_gif.py" \
    --frames-dir "${FRAMES_DIR}" \
    --log-file "${LOG_FILE}" \
    --output-gif "${GIF_FILE}"

echo "GIF written to ${GIF_FILE}"