How to build:

Make sure you have cordova (v5.1.1) installed:

	npm install cordova@5.1.1

Make sure you have android-sdk-linux installed. And the following variables are correctly set.

	export ANDROID_SDK_ROOT=<Location of the android-sdk-linux directory>
	export PATH=$PATH:$ANDROID_SDK_ROOT/tools

To generate android apk:

	cordova build android

To test on the emulator:

	cordova emulate android

To test on a physical device:

	cordova run --device
