# Evaluation

This aims to evaluate pitch tracking vamp plugins against the MedleyDB dataset.

The metrics are drawn from the `mir_eval` project described here: https://craffel.github.io/mir_eval/#module-mir_eval.melody

## Sample output

For a single audio file:

```json
StrandOfOaks_Spacestation_STEM_04.RESYN.csv vs. normalized_StrandOfOaks_Spacestation_STEM_04.RESYN_vamp_qlib_pitch_qlib_pitch_frequency.csv
	     Voicing Recall : 0.8954698979769491
	Voicing False Alarm : 0.013743218806509945
	 Raw Pitch Accuracy : 0.5876249173021345
	Raw Chroma Accuracy : 0.6104669382638671
	   Overall Accuracy : 0.8499387043406849
```

Statistics across all 65 files:

```
# aggregates from the Overall Accuracy results
max(field-1)	min(field-1)		mean(field-1)		median(field-1)
0.9543703959816	0.32786521835513	0.74307556471596	0.76909967089798
```

## Credits

This uses a cut down version of the CC0 licenced MedleyDB 1 dataset - described as follows:

> MDB-melody-synth contains 65 songs from the MedleyDB dataset (http://medleydb.weebly.com/) in which the melody track 
> has been resynthesized to obtain a perfect melody f0 annotation using the analysis/synthesis method described in the 
> following publication:
> 
> J. Salamon, R. M. Bittner, J. Bonada, J. J. Bosch, E. Gómez, and J. P. Bello. "An analysis/synthesis framework for 
> automatic f0 annotation of multitrack datasets". In 18th Int. Soc. for Music Info. Retrieval Conf., Suzhou, China, 
> Oct. 2017.
> https://zenodo.org/record/1481168/

You can download the original dataset from here: https://zenodo.org/record/1481168/

The metrics are produced by the `mir_eval` library (music information retrieval):

> mir_eval is a Python library which provides a transparent, standaridized, and straightforward way to evaluate Music Information Retrieval systems.
> 
> Raffel, B. McFee, E. J. Humphrey, J. Salamon, O. Nieto, D. Liang, and D. P. W. Ellis, “mir_eval: A Transparent Implementation of Common MIR Metrics”, Proceedings of the 15th International Conference on Music Information Retrieval, 2014.
> https://craffel.github.io/mir_eval/

## Dependencies

TODO: these are doing simple things so they could all be reimplemented in Python

* `jq` - https://stedolan.github.io/jq/download/
* `python3`
* `datamash` - https://www.gnu.org/software/datamash/ (`brew install datamash` on OSX)
* `sonic-annotator` - https://code.soundsoftware.ac.uk/projects/sonic-annotator/files and add to your `PATH`

## Steps

* Ensure that the qlib vamp plugin (from the parent directory) is built and added to your `VAMP_PATH` (`~/Library/Audio/Plug-Ins/Vamp` on OSX)
* `pip3 install mir_eval`
* `cd` into this folder
* Run `./evaluate.sh`

## Notes

The MedleyDB dataset has the following frequency ranges:

- lowest: `46.259328Hz`
- highest: `1210.180328Hz`
