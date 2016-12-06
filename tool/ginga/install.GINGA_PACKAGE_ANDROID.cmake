set(ANDROID_ACTIVITY_EXTRA_CONFIG "
            <intent-filter>
                <action android:name=\"android.intent.action.VIEW\" />

                <category android:name=\"android.intent.category.DEFAULT\" />
                <category android:name=\"android.intent.category.BROWSABLE\" />

                <data android:scheme=\"file\" />
                <data android:scheme=\"http\" />
                <data android:scheme=\"https\" />
                <data android:scheme=\"ftp\" />
                <data android:scheme=\"ftps\" />
                <data android:scheme=\"rtp\" />
                <data android:scheme=\"rtsp\" />
                <data android:scheme=\"mms\" />
                <data android:scheme=\"dav\" />
                <data android:scheme=\"davs\" />
                <data android:scheme=\"ssh\" />
                <data android:scheme=\"sftp\" />
                <data android:scheme=\"smb\" />

                <data android:mimeType=\"*/ncl\" />
		<data android:mimeType=\"*/*ncl\" />
		<data android:mimeType=\"*/*NCL\" />
                <data android:mimeType=\"application/x-ginga-NCL\" />
            </intent-filter>
            <intent-filter>
                <action android:name=\"android.intent.action.VIEW\" />

                <category android:name=\"android.intent.category.DEFAULT\" />
                <category android:name=\"android.intent.category.BROWSABLE\" />

                <data android:scheme=\"file\" />
                <data android:scheme=\"http\" />
                <data android:scheme=\"https\" />
                <data android:scheme=\"ftp\" />
                <data android:scheme=\"ftps\" />
                <data android:scheme=\"rtp\" />
                <data android:scheme=\"rtsp\" />
                <data android:scheme=\"mms\" />
                <data android:scheme=\"dav\" />
                <data android:scheme=\"davs\" />
                <data android:scheme=\"ssh\" />
                <data android:scheme=\"sftp\" />
                <data android:scheme=\"smb\" />

                <data android:host=\"*\" />
		<data android:mimeType=\"*/*\" />
                <data android:pathPattern=\".*\\\\.ncl\" />
                <data android:pathPattern=\".*\\\\.NCL\" />
            </intent-filter>
")
