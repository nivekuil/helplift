document.body.onload = function() {

  document.getElementById("save").onclick = function() {
    config = {
      'name': document.getElementById("name").value,
      'contact': document.getElementById("contact").value,
      'num_passes': document.getElementById("slider").value / 2
    };
    console.log(config);
    location.href = "pebblejs://close#" +
      encodeURIComponent(JSON.stringify(config));
  };
};
