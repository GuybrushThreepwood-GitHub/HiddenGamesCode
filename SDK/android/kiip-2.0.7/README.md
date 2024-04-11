# Kiip SDK - Android

Library project including Kiip services client jar. Supported platforms: Android 2.2+ (Tested 2.3.4)


## Contents

### KiipSDK

Kiip folder which carries the needed res files (under src>main>res) and the KiipSDK.jar.


## Required Libraries

* android-support-v4 (For applications that support < android-11)


## Samples

Sample projects can be found at http://github.com/kiip/android-samples


## Instructions

### Gradle

1. Copy `KiipSDK` into the root of your project
1. Add `KiipSDK` as a dependency to your project:

	dependencies {
		compile project(':KiipSDK')
	}


### Manual Integration

1. Copy `KiipSDK/libs/KiipSDK.jar` into your `libs/` directory
1. Copy android-support-v4.jar from your android installation folder into your `libs/` directory
1. Add Kiip resources from `KiipSDK/src/main/res/*` to your project


## Support

For integration and reference, see our docs at http://docs.kiip.me
