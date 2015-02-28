var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 10000, 
  timeout: 10000
};

function locationSuccess(pos) {
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
	Pebble.sendAppMessage({'lat': pos.coords.latitude.toString(),'lon': pos.coords.longitude.toString()}, function(err)
	{
		console.log("Success");
	},function (err)
	{
		console.log("failure");
	});
}


function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}

Pebble.addEventListener('ready',
  function(e) {
    // Request current position
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  }
);