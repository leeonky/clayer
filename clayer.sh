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

function play_video() {
	if [ "$subtitle" == "" ]; then
		"$project_path/decoder/decoder" "$media_file" -v $video
	else
		encoding=$(file -b --mime-encoding "$subtitle")
		if [ $encoding != 'utf-8' ]; then
			encoding=${encoding/%le/}
			encoding=${encoding/%be/}
			new_file=$(mktemp /tmp/sub.srt.XXXXXX)
			iconv -f $encoding -t utf-8 "$subtitle" > "$new_file"
			sed -i '1s/^\xEF\xBB\xBF//' "$new_file"
			subtitle="$new_file"
		fi
		"$project_path/decoder/decoder" "$media_file" -v $video | "$project_path/subtitle/subtitle" -f "$project_path/wqy-zenhei.ttc" "$subtitle"
	fi
}

set -x
(
	play_video &
	"$project_path/decoder/decoder" "$media_file" -a $audio | "$project_path/resampler/resampler" -f pack:flt32:maxbit32 -l stereo | "$project_path/speaker/speaker" -d 1
) | "$project_path/screen/screen" $position $size $video_flag

