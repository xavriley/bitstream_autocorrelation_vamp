#!/usr/bin/env bash

#VAMP_PATH="../" 
mkdir -p results
mkdir -p annotation_qlib
# variant with GNU parallel
# find audio_melody_normalized/mp3/*.mp3 | xargs basename | parallel -I% --jobs 32 --max-args 1 sonic-annotator -q -t ./params_qlib_pitch.n3 -w csv --csv-basedir ./annotation_qlib/ --csv-omit-filename -r --csv-force ./audio_melody_normalized/mp3/%

# add -n to normalize files before running vamp
# not required here because the files are already normalized
sonic-annotator -t ./params_qlib_pitch.n3 -w csv --csv-basedir ./annotation_qlib/ --csv-omit-filename -r --csv-force ./audio_melody_normalized/mp3

# for i in ./annotation_melody/*.csv
# do python3 ./melody_eval.py "$i" "./annotation_qlib/normalized_$(basename -s .csv "$i")_vamp_qlib_pitch_qlib_pitch_frequency.csv" -o "./results/$(basename -s .csv "$i").json"; done

parallel 'python3 ./melody_eval.py "./annotation_melody/{/}" "./annotation_qlib/normalized_{/.}_vamp_qlib_pitch_qlib_pitch_frequency.csv" -o "./results/{/}.json"' ::: ./annotation_melody/*.csv

find results/*.json | xargs jq '{"file": input_filename, "result": ."Overall Accuracy"}' | jq -s -c 'sort_by(.result)[]'

find results/*.json | xargs jq '{"file": input_filename, "result": ."Overall Accuracy"}' | jq -s -c 'sort_by(.result)[] | .result' | datamash --header-out max 1 min 1 mean 1 median 1
