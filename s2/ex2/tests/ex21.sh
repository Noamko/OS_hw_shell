#!/bin/bash
# Noam Koren 308192871
ext="*."
if [ ! -z "$3" ] ; then
	ext="${ext}$2"
	cd $1
	for file in $(ls $(echo "${ext}"))
	do
		grep -wi $3 $file
	done
else
	echo "Not enough parameters"
fi
