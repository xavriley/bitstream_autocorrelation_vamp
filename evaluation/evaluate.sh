#!/usr/bin/env bash

#VAMP_NAME="pyin"
#VAMP_FULL_NAME="vamp_pyin_pyin_smoothedpitchtrack"

VAMP_NAME="qlib"
VAMP_FULL_NAME="vamp_qlib_pitch_qlib_pitch_frequency"

#VAMP_PATH="../" 
mkdir -p results
mkdir -p annotation_$(echo $VAMP_NAME)
# run this if you are using parallel for the first time
# parallel --citation

# variant with GNU parallel
find audio_melody_normalized/mp3/*.mp3 | xargs basename | parallel -I% --jobs 32 --max-args 1 sonic-annotator -q -t ./params/$(echo $VAMP_NAME)/$(echo "%").n3 -w csv --csv-basedir ./annotation_$(echo $VAMP_NAME)/ --csv-omit-filename -r --csv-force ./audio_melody_normalized/mp3/%

# add -n to normalize files before running vamp
# not required here because the files are already normalized
# sonic-annotator -t ./params_qlib_pitch.n3 -w csv --csv-basedir ./annotation_qlib/ --csv-omit-filename -r --csv-force ./audio_melody_normalized/mp3

# for i in ./annotation_melody/*.csv
# do python3 ./melody_eval.py "$i" "./annotation_qlib/normalized_$(basename -s .csv "$i")_vamp_qlib_pitch_qlib_pitch_frequency.csv" -o "./results/$(basename -s .csv "$i").json"; done

# I couldn't figure out how to interpolate bash variables
# into this parallel command string so I gave up
case $VAMP_NAME in
	qlib)
		parallel 'python3 ./melody_eval.py "./annotation_melody/{/}" "./annotation_qlib/normalized_{/.}_vamp_qlib_pitch_qlib_pitch_frequency.csv" -o "./results/qlib_{/.}.json"' ::: ./annotation_melody/*.csv
		;;
	pyin)
		parallel 'python3 ./melody_eval.py "./annotation_melody/{/}" "./annotation_pyin/normalized_{/.}_vamp_pyin_pyin_smoothedpitchtrack.csv" -o "./results/pyin_{/.}.json"' ::: ./annotation_melody/*.csv
		;;
esac

# output scores for all files, sorted low to high by overall accuracy
find results/$(echo $VAMP_NAME)_*.json | xargs jq '{"file": input_filename, "result": ."Overall Accuracy"}' | jq -s -c 'sort_by(.result)[]'

# output aggregate stats for overall accuracy
find results/$(echo $VAMP_NAME)_*.json | xargs jq '{"file": input_filename, "result": ."Overall Accuracy"}' | jq -s -c 'sort_by(.result)[] | .result' | datamash --header-out max 1 min 1 mean 1 median 1
