#!/bin/bash
# Submit all Run3 2025 ZeroBias Data CRAB jobs
# Runs: C (v1, v2), D (v1), E (v1), F (v1, v2), G (v1)
#
# Usage:
#   bash submit_all_2025_data.sh           # submit all
#   bash submit_all_2025_data.sh C         # submit Run C only
#   bash submit_all_2025_data.sh C D       # submit Run C and D only

# ==============================================================================
# Setup
# ==============================================================================

set -e  # exit on error

CRAB_CONFIG="crab_run_2025_data.py"

# All jobs: "RUN DATASET_VERSION"
ALL_JOBS=(
#    "C v1"
#    "C v2"
#    "D v1"
#    "E v1"
    "F v1"
#    "F v2"
    "G v1"
)

# ==============================================================================
# Filter by selected runs (optional args)
# ==============================================================================

SELECTED_RUNS=("$@")   # e.g. bash submit_all.sh C D

should_submit() {
    local run=$1
    if [ ${#SELECTED_RUNS[@]} -eq 0 ]; then
        return 0   # no filter: submit all
    fi
    for r in "${SELECTED_RUNS[@]}"; do
        if [ "${r^^}" == "$run" ]; then
            return 0
        fi
    done
    return 1
}

# ==============================================================================
# CRAB environment check
# ==============================================================================

if ! command -v crab &> /dev/null; then
    echo "[ERROR] CRAB not found. Please source crab environment:"
    echo "  source /cvmfs/cms.cern.ch/crab3/crab.sh"
    exit 1
fi

if ! voms-proxy-info --exists &> /dev/null; then
    echo "[ERROR] No valid grid proxy. Please run:"
    echo "  voms-proxy-init --voms cms --valid 168:00"
    exit 1
fi

# ==============================================================================
# Submit
# ==============================================================================

echo "============================================================"
echo "  Run3 2025 ZeroBias Data -- CRAB Submission"
echo "============================================================"

SUBMITTED=0
SKIPPED=0

for job in "${ALL_JOBS[@]}"; do
    RUN=$(echo $job | awk '{print $1}')
    DVER=$(echo $job | awk '{print $2}')

    if ! should_submit "$RUN"; then
        echo "[SKIP] Run 2025${RUN}_${DVER}"
        ((SKIPPED++)) || true
        continue
    fi

    echo ""
    echo "[SUBMIT] Run 2025${RUN} dataset ${DVER}"
    RUN=$RUN DATASET_VERSION=$DVER python3 $CRAB_CONFIG

    if [ $? -eq 0 ]; then
        echo "[OK] Run 2025${RUN}_${DVER} submitted"
        ((SUBMITTED++)) || true
    else
        echo "[FAIL] Run 2025${RUN}_${DVER} submission failed"
    fi

    # Brief pause between submissions to avoid server overload
    sleep 3
done

echo ""
echo "============================================================"
echo "  Done: ${SUBMITTED} submitted, ${SKIPPED} skipped"
echo "============================================================"
