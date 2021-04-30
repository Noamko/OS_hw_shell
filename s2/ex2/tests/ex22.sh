#!/bin/bash
# Noam koren 308192871

print_match() {
	#$1 = word
	#$2 = file_to_read
	#$3 = word_count
	grep -wi $1 $2 | while read -r line ; do
		if [ $(echo "$line" | wc -w) -ge $3 ] ; then
    		echo $line
		fi
	done
}

ex21(){
	#$1 = folder
	#$2 = extention
	#$3 = word
	#$4 = word_count
	ext="*."
	ext="${ext}$2"
	cd $1
	for file in $(ls -v 2> /dev/null $(echo "${ext}")) ; do
		print_match $3 $file $4
	done
	cd ..
}

DFS(){
	#$1 = folder
	#$2 = extension
	#$3 = word
	#$4 = word to find
	
	ex21 $1 $2 $3 $4
	cd $1
	for dir in $(ls -v 2> /dev/null) ; do
		if [ -d $dir ] ; then
			DFS $dir $2 $3 $4
		fi
	done
	cd ..
}

if [ ! -z $4 ] ; then
	DFS $1 $2 $3 $4
else 
	echo "Not enough parameters"
fi
