echo "Starting smoketest"
echo " "

time gwtestdirector -v 1 -c /home/builder/gw/environment.conf -l /home/builder/gw/src/testcases/postbuild/smoketest/smoketest.py
SMKRESULT=$?

echo " "
if [ ${SMKRESULT} == "0" ]; then
	echo "Smoketest successful."
	exit 0
else
	echo "Smoketest failed! (${SMKRESULT})"
	exit ${SMKRESULT}
fi

