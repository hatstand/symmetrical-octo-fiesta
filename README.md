# Cheating at Words With Friends

## Building the Android app:

1. [Install Bazel](https://bazel.build/)

1. `bazel build --cpu=armeabi-v7a java/com/purplehatstands/wwf/cheating:cheating`

1. Install the apk with `adb install bazel-bin/java/com/purplehatstands/wwf/cheating/cheating.apk`


## Using the app

Take a screenshot of an in-progress game of Words with Friends and share it with the app and wait a few seconds.

There are some magic numbers in the code for where the letter grid is located on the screen that probably only work
if your device has the same resolution as mine (a Galaxy S7 Edge).
