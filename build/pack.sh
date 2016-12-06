#!/bin/bash

DEPOT=$1 	# path 
TOOL=$2		# tool name only
PLATFORM=$3  	# PC | CS | ST
DESTDIR=$4	# dir to store packed tool
function showHelp(){
	echo "tvd-lifia pack script"
	echo "	Usage: pack.sh DEPOT TOOL PLATFORM DESTDIR"
	echo " "
	echo "	Options: "
	echo "		DEPOT    = Path to middleware to be packaged."
	echo "		TOOL     = Tool name"
	echo "		PLATFORM = ( PC | CS | ST )"
	echo "		DESTDIR  = Path where pack will be stored."
	echo " "
	echo "	or use pack.sh --help to get this info."
	echo " "
}

function appendPath(){
	CURRPATH=$1
	RETURNLIST=""
	for j in ${LIST[*]}; do
		RETURNLIST="${RETURNLIST} ${CURRPATH}/${j}"
	done
}

function findMatchingFiles(){
	#$1 dir
	#$2 pattern
	#$3 match mode flag
	if [ $3 == "DIR" ]; then
		RETURNLIST=`find $1 -iwholename "*$2*" -type d | tr '\n' ' '`
	else 
		if [ $3 == "SENS" ]; then
			RETURNLIST=`find $1 -wholename "*$2*" -type f | tr '\n' ' '`
		else 
			RETURNLIST=`find $1 -iwholename "*$2*" -type f | tr '\n' ' '`
		fi
	fi
}

function toolFiles(){
	#$1 = TOOL name
	#$2 = PLATFORM_name
	TOOLDIR=tool/$1
	findMatchingFiles ${TOOLDIR} "platform_$2" "FILES"
	t_platformFiles=${RETURNLIST}
	
	findMatchingFiles ${TOOLDIR} "sources_$2" "FILES"
	t_sourcesFiles=${RETURNLIST}
	
	findMatchingFiles ${TOOLDIR} "CPack*$2.cmake" "FILES"
	t_cpackFiles=${RETURNLIST}
	
	findMatchingFiles ${TOOLDIR} "platform/$2" "DIR"
	t_srcFiles=${RETURNLIST}
	
	RETURNLIST="${t_platformFiles} ${t_sourcesFiles} ${t_cpackFiles} ${t_srcFiles}"
}

function platformFiles(){
	#$1 tool
	#$2 platform
	#$3 match sensitively only
	if [ $# -eq 3 ]; then
		matchMode=$3
	else
		matchMode="FILES"
	fi
	
	findMatchingFiles config "_$2" "${matchMode}"
	configFILES=${RETURNLIST}
	
	toolFiles $1 $2 
	toolFILES=${RETURNLIST}
	
	RETURNLIST="${configFILES} ${toolFILES}"
}

#check if all params are ok.
if [ $1 == "--help" ] || [ $# -ne 4 ] || [ ! -d ${DEPOT} ] || [ ! -d ${DEPOT}/tool/${TOOL} ] || [ ! -d ${DESTDIR} ]; then
	showHelp
	exit 0
fi

case ${PLATFORM} in
	PC | CS | ST ) echo " -- Platform to pack: ${PLATFORM} ";;
	*) showHelp ; exit 0 ;;
esac

# initial list for files to be removed with some fixed dirs
DELFILES="install/"

# filter libs in LIBS array from list in build.conf file inside the tool 
LIBS=`ls ${DEPOT}/lib`
CONF=${DEPOT}/tool/${TOOL}/build.conf 

for i in ${LIBS[*]}; do
	cat ${CONF} | grep -x ${i} > /dev/null
	if [ $? != "0" ]; then
		NOLIB="${NOLIB} lib/${i}"
	fi
done

# filter tools
LIST=`ls ${DEPOT}/tool | grep -v ${TOOL} | tr '\n' ' '`
appendPath tool
NOTOOL=${RETURNLIST}

# filter custom build stuff (buildbot, pack.sh, etc)
LIST=`ls -A ${DEPOT}/build | grep -v 'build.py' | grep -v 'CMakeLists.txt' | tr '\n' ' '`
appendPath build 
NOBUILD=${RETURNLIST}

# filter platform
# platformFiles CS 
platformFiles ${TOOL} CS 
CSFILES=${RETURNLIST}

# platformFiles ST 
platformFiles ${TOOL} ST 
STFILES=${RETURNLIST}

# platformFiles PC
platformFiles ${TOOL} LINUX "SENS"
PCFILES=${RETURNLIST}

platformFiles ${TOOL} WINDOWS  
PCFILES="${PCFILES} ${RETURNLIST}"

platformFiles ${TOOL} VS 
PCFILES="${PCFILES} ${RETURNLIST}"

platformFiles ${TOOL} MINGW 
PCFILES="${PCFILES} ${RETURNLIST}"

platformFiles ${TOOL} PC 
PCFILES="${PCFILES} ${RETURNLIST}"

if [ ${PLATFORM} == "PC" ]; then
	NOPLAT="${CSFILES} ${STFILES}"
fi

if [ ${PLATFORM} == "CS" ]; then
	NOPLAT="${STFILES} ${PCFILES}"
fi

if [ ${PLATFORM} == "ST" ]; then
	NOPLAT="${CSFILES} ${PCFILES}"
fi

DELFILES="${DELFILES} ${NOLIB} ${NOTOOL} ${NOPLAT} ${NOBUILD}"

# do some log before packing
echo "Files not included in pack:"
echo ${DELFILES}

TMPDEPOT=`mktemp -d`
DATE=`date +%Y%m%d-%H%M`

# clean all files not needed for target tool+platform and pack it
WORKDIR=${TMPDEPOT}/${TOOL}
mkdir -p ${WORKDIR}
cp -adPr ${DEPOT}/* ${WORKDIR}/
cd ${WORKDIR}
rm -rf ${DELFILES} 

cd ${TMPDEPOT}
PTOOL="${DESTDIR}/${TOOL}-${PLATFORM}-${DATE}.tar.bz2"
tar -c ${TOOL} | bzip2 > ${PTOOL}
echo "Packed tool: ${PTOOL}"

cd ${TMPDEPOT}/..
rm -rf ${TMPDEPOT} 

