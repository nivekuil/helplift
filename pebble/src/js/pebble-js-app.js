var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.open(type, url);
  xhr.onreadystatechange = function () {
      if (xhr.readyState==4) {
          if (xhr.status!=404) {
              console.log("response: " + xhr.responseText);
              callback(xhr.responseText);
          } else {
              console.log("status changing to " + xhr.status);
              console.log("response: " + xhr.responseText);

          }
      } else {
          console.log("state changing to " + xhr.readyState);
      }
  };
  xhr.send();
};

function askForHelp() {
  // Construct URL
  var url = "http://nivekuil.pythonanywhere.com/text";

  console.log("loading " + url + "...")

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET',
    function(responseText) {
      // responseText contains a JSON object with weather info
      console.log("success, got " + responseText)

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
