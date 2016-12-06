echo "Starting to send upgrade:"
echo " "
cat ~/gw/src/testcases/postbuild/upgrade/resources/upgradeconfig.txt  | grep "="
echo " "

time gwtestdirector -v 1 -c /home/builder/gw/environment.conf -l /home/builder/gw/src/testcases/postbuild/upgrade/upgrade.py 
UPGRESULT=$?

echo " "
if [ ${UPGRESULT} == "0" ]; then
	echo "Upgrade successful."
	exit 0
else
	echo "Failed to upgrade STB! (${UPGRESULT})"
	exit ${UPGRESULT}
fi

