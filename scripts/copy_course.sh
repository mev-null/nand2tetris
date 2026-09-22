#!/bin/sh
# Copy the course's projects/NN directories into this repository's projects/NN-<name> layout.
# usage: scripts/copy_course.sh /tmp/n2t-projects/projects
# cp -n never overwrites, so authored files stay untouched and re-running is safe.
set -eu

src=${1:?usage: $0 COURSE_PROJECTS_DIR}
root=$(cd "$(dirname "$0")/.." && pwd)

dest_of() {
  case $1 in
    01) echo 01-boolean-logic ;;
    02) echo 02-boolean-arithmetic ;;
    03) echo 03-memory ;;
    04) echo 04-machine-language ;;
    05) echo 05-computer-architecture ;;
    06) echo 06-assembler ;;
    07 | 08) echo 07-08-vm-translator ;;
    09) echo 09-high-level-language ;;
    # both chapters ship a different Square/, so each keeps its own directory
    10 | 11) echo "10-11-compiler/$1" ;;
    12) echo 12-operating-system ;;
    # 00 and 13 hold no course projects
    *) echo "" ;;
  esac
}

for dir in "$src"/*/; do
  chapter=$(basename "$dir")
  dest=$(dest_of "$chapter")
  [ -n "$dest" ] || continue
  mkdir -p "$root/projects/$dest"
  # macOS cp -n exits 1 when it skips a file that already exists, which is the point here
  cp -Rn "$dir". "$root/projects/$dest/" || true
done
