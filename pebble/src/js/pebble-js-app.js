var base_url = 'http://nivekuil.pythonanywhere.com/';

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.open(type, url);
  xhr.onreadystatechange = function () {
      if (xhr.readyState==4) {
          if (xhr.status!=404) {
              console.log("response: " + xhr.responseText);
              callback(xhr.responseText);
          }
      }
  };
  xhr.send();
};

function askForHelp() {
  // Construct URL
  var url = base_url + "text?name=test_app&phone_number=12345&lat=123&lon=456";

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
    askForHelp();
  }
);

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('Configuration window returned: ' + e.response);
});

console.log("JS loaded, but not ready yet!");
