#!/bin/bash
# clayer file [v=index] [a=index] [sub=subtitle_file]

project_path="$(dirname "$0")"

media_file=$1

"$project_path/decoder/decoder" "$media_file" | "$project_path/screen/screen" &
"$project_path/decoder/decoder" "$media_file" -a 0 | "$project_path/resampler/resampler" -f pack:flt32:maxbit32 -lmax5.1 | "$project_path/speaker/speaker"
