var base_url = 'https://watchdog.mybluemix.net/';

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
	console.log("state != 404: " + xhr.status);
      }
    }
  };
  xhr.send();
};

function askForHelp() {
  var do_request = function() {
    var url = base_url + "text" +
	"?name=" + config_name +
	"&number=" + config_contact +
	"&lat=" + config_lat +
	"&lon=" + config_lon;
    console.log("open url " + url);

    xhrRequest(url, 'GET',
      function(responseText) {
        console.log("success, got " + responseText);

        var dictionary = {
          "KEY_STATE": 1, // success
        };

        Pebble.sendAppMessage(dictionary,
          function(e) {
            console.log("State info sent to Pebble successfully!");
          },
          function(e) {
            console.log("Error sending state info to Pebble!");
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
    timeout: 10000,
  };

  navigator.geolocation.getCurrentPosition(locationSuccess,
					   locationError,
					   locationOptions);

}

Pebble.addEventListener('showConfiguration', function(e) {
  console.log("show config, open " + base_url + 'config');
  Pebble.openURL(base_url + 'config');
});

Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    askForHelp();
  }
);

Pebble.addEventListener('webviewclosed', function(e) {
  var config = JSON.parse(e.response);
  config_name = config.name;
  config_contact = config.contact;

  console.log("Response:" + e.response);

  var dictionary = {
    "KEY_NUM_PASSES": parseInt(config.num_passes),
  };

  Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log("Configuration sent to Pebble successfully!");
    },
    function(e) {
      console.log("Error sending configuration to Pebble!");
    }
  );
});
