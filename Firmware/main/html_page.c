// html_page.c
#include "html_page.h"

const char* html_page =
    "<!DOCTYPE html><html><head><title>Hydroponic Control Web Server</title></head><body>"
    "<h1>Hydroponic Control Web Server</h1>"

    "<p><a href=\"/led/on\"><button>LED ON</button></a></p>"
    "<p><a href=\"/led/off\"><button>LED OFF</button></a></p>"

    "<form action=\"/submit\" method=\"GET\">"
    "<label for=\"name\">Enter text:</label>"
    "<input type=\"text\" id=\"name\" name=\"name\">"
    "<input type=\"submit\" value=\"Send\">"
    "</form>"

    "<h2>Heater P Value: %d</h2>"

    "<h2>Heater I Value: %d</h2>"

    "<h2>Heater D Value: %d</h2>"

    "</body></html>";

