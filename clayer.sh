#!/bin/bash
# clayer file [v=index] [a=index] [sub=subtitle_file] [p=position(0,0)] [s=size(1920x1080)] [vf=video_flag(full|opengl|borderless)]

project_path="$(dirname "$0")"
media_file=$1

shift 1
for args in "$@"
do
        eval "$args"
done

video=${v:-0}
audio=${a:-0}
subtitle=${sub:-}

position=${p:-}
if [ "$position" != "" ]; then
	position="-p $position"
fi

size=${s:-}
if [ "$size" != "" ]; then
	size="-s $size"
fi

video_flag=${vf:-}
if [ "$video_flag" != "" ]; then
	video_flag="-f $video_flag"
fi

if [ "$subtitle" != "" ]; then
	encoding=$(file -b --mime-encoding "$subtitle")
	if [ $encoding != 'utf-8' ]; then
		encoding=${encoding/%le/}
		encoding=${encoding/%be/}
		new_file=$(mktemp /tmp/sub.srt.XXXXXX)
		iconv -f $encoding -t utf-8 "$subtitle" > "$new_file"
		#sed -i '1s/^\xEF\xBB\xBF//' "$new_file"
		subtitle="$new_file"
	fi
fi

function output_video() {
	if [ "$subtitle" == "" ]; then
		"$project_path/decoder/cl_decoder" "$media_file" -v $video
	else
		"$project_path/decoder/cl_decoder" "$media_file" -v $video | "$project_path/subtitle/cl_subtitle" -f "$project_path/wqy-zenhei.ttc" "$subtitle"
	fi
}

function play_audio() {
	 "$project_path/decoder/cl_decoder" "$media_file" -a $audio | "$project_path/resampler/cl_resampler" -f pack:flt32:maxbit32 -l stereo | "$project_path/speaker/cl_speaker" -d 1
}

(
	(play_audio | grep -v --line-buffered 'CLOCK') &
	"$project_path/cl_terminal"
) | "$project_path/controller/cl_controller"

exit

set -x
(
(
	output_video &
	#play_audio
) | "$project_path/screen/cl_screen" $position $size $video_flag &
"$project_path/cl_terminal"
) | "$project_path/controller/cl_controller"

