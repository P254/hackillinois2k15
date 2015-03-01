var id;

var lat_prv = 40.11476593028796;
var lat_cur = 40.114794116912314;
var lat_des = 40.11483045551279;//-100;
var lon_prv = -88.22826711564456;
var lon_cur = -88.2284647014247;
var lon_des = -88.22847872192051;//100
var offset_angle = 0.0;

var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 0, 
  timeout: 5000
};

function locationSuccess(pos) {
	console.log('Location changed u hack');
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);

  lat_prv = lat_cur;
  lon_prv = lon_cur;
  //pull new values for lat and lon
  lat_cur = pos.coords.latitude;//latitude = y
  lon_cur = pos.coords.longitude;//longitude = x
  if(lat_cur != lat_prv || lon_cur != lon_prv)
  {
    var angle_direction = Math.atan((lon_cur-lon_prv)/(lat_cur-lat_prv));
    var dX = lon_des-lon_cur;
    var dY = lat_des-lat_cur;
    var angle_comp = (Math.PI/2 - Math.abs(angle_direction));
    var distance = Math.sqrt(dX*dX + dY*dY);
    var R = dY * Math.tan(angle_direction);
    var r = R - dX;
    offset_angle = Math.asin((r/distance)*Math.sin(angle_comp));
    console.log(offset_angle);
    offset_angle = (offset_angle*180)/Math.PI;
  }

  console.log('theta=' + offset_angle);
  
  Pebble.sendAppMessage({
    'angle': offset_angle
  }, function(err) {
		console.log("Success");
	}, function(err) {
		console.log("failure");
	});
}


function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}

Pebble.addEventListener('ready',
  function(e) {
    // Request current position
    id = navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
  }
);
