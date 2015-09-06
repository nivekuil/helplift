var base_url = 'https://watchdog.mybluemix.com/';

var config_name = "Kevin";
var config_contact = "6266026651";
var config_lat = "0";
var config_lon = "0";

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.open(type, url);
  xhr.onreadystatechange = function () {
    if (xhr.readyState==4) {
      if (xhr.status!=404) {
        console.log("response: " + xhr.responseText);
        callback(xhr.responseText);
      } else {
	console.log("states != 404: " + xhr.status);
      }
    } else {
      console.log("readyState != 4: " + xhr.readyState);
    }
  };
  xhr.send();
};

function askForHelp() {
  var do_request = function() {
    // Construct URL
    var url = base_url + "text" +
	"?name=" + config_name +
	"&number=" + config_contact +
	"&lat=" + config_lat +
	"&lon=" + config_lon;
    console.log("open url " + url);

    // Send request to OpenWeatherMap
    xhrRequest(url, 'GET',
      function(responseText) {
        // responseText contains a JSON object with weather info
        console.log("success, got " + responseText);

        //var json = JSON.parse(responseText);

        // Assemble dictionary using our keys
        var dictionary = {
          "STATE": 1, // success
        };

        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
          function(e) {
            console.log("Weather info sent to Pebble successfully!");
          },
          function(e) {
            console.log("Error sending weather info to Pebble!");
          }
        );
      }
    );
  };


  var locationSuccess = function(pos) {
    console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
    config_lat = pos.coords.latitude;
    config_lon = pos.coords.longitude;
    do_request();
  };

  var locationError = function (err) {
    console.log('location error (' + err.code + '): ' + err.message);
    config_lat = 0;
    config_lon = 0;
    do_request();
  };

  var locationOptions = {
    enableHighAccuracy: true,
    maximumAge: 10000,
    timeout: 10000
  };

  navigator.geolocation.getCurrentPosition(locationSuccess,
					   locationError,
					   locationOptions);

}

Pebble.addEventListener('showConfiguration', function(e) {
  console.log("show config, open " + base_url + 'config');
  // Show config page
  Pebble.openURL(base_url + 'config');
});

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("name: " + config_name);
    console.log("contact: " + config_contact);
    askForHelp();
  }
);

Pebble.addEventListener('webviewclosed', function(e) {
  var config = JSON.parse(e.response);
  config_name = config.name;
  config_contact = config.contact;
});
