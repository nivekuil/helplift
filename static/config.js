document.getElementById("save").onClick(function() {
  config = {
    'name': document.getElementById("name").value,
    'contact': document.getElementById("contact").value
  };
  location.href = "pebblejs://close#" +
    encodeURIComponent(JSON.stringify(config));
});
