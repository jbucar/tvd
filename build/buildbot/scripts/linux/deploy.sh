#!/bin/bash

# params:
#	tool
#	buildername
#	branch
#	revision
BUILDERNAME=$1
REVISION=$2
ORIGIN=$3
BUILDREPO=$4

echo "Current status:"
echo "BUILDERNAME=${BUILDERNAME}"
echo "REVISION=${REVISION}"
echo "ORIGIN=${ORIGIN}"
echo "BUILDREPO=${BUILDREPO}"

# do we have a version already tagged?
GITTAG=`git describe --tags`
if [ $? -eq "0" ]; then
	REVISION=${GITTAG}
fi

BRANCHDIR=${BUILDREPO}/${BUILDERNAME}/
echo "BRANCHDIR=${BRANCHDIR}"

DESTDIR=${BRANCHDIR}/${REVISION}/
echo "DESTDIR=${DESTDIR}"

mkdir -p ${DESTDIR}
cp -r -v ${ORIGIN}/lifia ${DESTDIR}
echo "build: ${REVISION}" > ${DESTDIR}/build

# clear symlink:
rm ${BRANCHDIR}/LATEST

# limit builds to the latest 10
cd ${BRANCHDIR}
REST=$(( `ls | wc -l` - 10 ))
if [ "${REST}" -gt "0" ]; then
	for i in `ls -t | tail -n ${REST}`; do
		echo "Removing old build: ${i}"
	        # rm -rf ${i}
	done
else
	echo "Nothing old to remove."
fi

# set new symlink
ln -s ${DESTDIR} ${BRANCHDIR}/LATEST

