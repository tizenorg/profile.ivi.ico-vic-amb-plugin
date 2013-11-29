BEGIN {
	FS = " "
	prevtime = -1
	measurementerror = 0.2
	errorflg = 0
}
{
	if (prevtime != -1) {
		currenttime = $1
		interspacetime = currenttime - prevtime
		if (interspacetime < (standardvalue - (standardvalue * measurementerror)) || interspacetime > (standardvalue + (standardvalue * measurementerror))) {
			print interspacetime
			errorflg = 1
		}
	}
	prevtime = $1
}
END {
	if (errorflg == 0) {
		print "OK!"
	}
	else {
		print "NG"
	}
}
