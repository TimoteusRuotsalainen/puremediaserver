#!/bin/bash
# make_thumbs v0.0.1
# Script que genera thumbnails de todas las películas en el directorio Media
# Copyright Santi Noreña 2012
# puremediaserver at gmail dot com
# GPL License

export MEDIA_DIR=/home/santi/PMS/svn/media/video
export E_XCD=86

cd $MEDIA_DIR ||{
   echo "Cannot change to necessary directory." $MEDIA_DIR
   exit $E_XCD;
}


for folder in $(find -maxdepth 1 -type d); do
		rm $folder/thumbs/*
	done

for file in $(find . -maxdepth 2 -mindepth 2 -type f ); do 
		avconv -i $file -f image2 -frames:v 1 -s 64x46 -vcodec mjpeg $file.jpg
	done

for folder in $(find -maxdepth 1 -type d); do
		mv $folder/*.jpg $folder/thumbs
	done

exit 0
