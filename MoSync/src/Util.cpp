#include <maapi.h>
#include <matime.h>
#include <mastring.h>
#include <maxtoa.h>

#include "Platform.h"
#include "Util.h"

String& getString(MAHandle stringResource) {
    int length = maGetDataSize(stringResource);
    String* output;
    if (length) {
        char buffer[length + 1];
        maReadData(stringResource, buffer, 0, length);
        buffer[length] = '\x00';
        output = new String(buffer);
    } else {
        output = new String();
    }
    return *output;
}

char* greekSize(char* buffer, int value) {
    int a = mod(value);
    if (a <= 9999) {
        itoa(value, buffer, 10);
    } else {
        a /= 1024;
        char* p = "KMGTPEZY";
        while (a > 9999) {
            a /= 1024;
            p++;
        }
        if (value < 0) {
            a = -a;
        }
        sprintf(buffer, "%d%c", a, *p);
    }
    return buffer;
}

char* memInfo(char* buffer, int free, int total) {
	char b1[8], b2[8];
    sprintf(buffer, "%sB/%sB", greekSize(b1, free), greekSize(b2, total));
    return buffer;
}

char* justTime() {
    int localTime = maLocalTime();
    if (DAYLIGHT_SAVING_TIME) {
        // Workaround for MoSync issue 646
        localTime += 60 * 60;
    }
    char* date = sprint_time(localTime);
    date += strlen(date) - 13;
    date[8] = '\x00';
    return date;
}
