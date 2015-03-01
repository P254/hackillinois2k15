var id;

var y1demo = 40.11476593028796;
var y2demo = 40.114794116912314;
var y3demo = 40.11483045551279;
var x1demo = -88.22826711564456;
var x2demo = -88.2284647014247;
var x3demo = -88.22847872192051;
var lat_prv = 0.0;
var lat_cur = 0.0;
var lat_des = 0.0;
var lon_prv = 0.0;
var lon_cur = 0.0;
var lon_des = 0.0;
var offset_angle = 0.0;
var distance = 0.0;
var prevDistance = 0.0;
var returnString = "";

var saveCurrentLocation = false;
var navigating = false;
var demoMode = false;

var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 0, 
  timeout: 2000
};

function locationSuccess(pos) {
	console.log('Location changed u hack');
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
  
  if(!demoMode)
  {
    lat_prv = lat_cur;
    lon_prv = lon_cur;
    //pull new values for lat and lon
    lat_cur = pos.coords.latitude;//latitude = y
    lon_cur = pos.coords.longitude;//longitude = x
    //if first time
    if(lat_prv===0 || lon_prv===0)
    {
      lat_prv = lat_cur;
      lon_prv = lon_cur;
    }
  }
  else
  {
    lat_prv = y1demo;
    lat_cur = y2demo;
    lat_des = y3demo;
    lon_prv = x1demo;
    lon_cur = x2demo;
    lon_des = x3demo;
  }  
  
  //if save loc
  if(saveCurrentLocation)
  {
    lat_des = lat_cur;
    lon_des = lon_cur;
    saveCurrentLocation = false;
    saveDestinationToFile();
  }
  
  console.log("destination:" + lat_des + ", " + lon_des);
  
  if(navigating)
  {
    if(lat_cur != lat_prv || lon_cur != lon_prv)
    {
      prevDistance = distance;
      var angle_direction = Math.atan((lon_cur-lon_prv)/(lat_cur-lat_prv));
      var dX = lon_des-lon_cur;
      var dY = lat_des-lat_cur;
      var angle_comp = (Math.PI/2 - Math.abs(angle_direction));
      distance = Math.sqrt(dX*dX + dY*dY);
      var R = dY * Math.tan(angle_direction);
      var r = R - dX;
      offset_angle = Math.asin((r/distance)*Math.sin(angle_comp));
      console.log(offset_angle);
      offset_angle = (offset_angle*180)/Math.PI;
      if(distance > prevDistance && distance-prevDistance < -0.05*distance) offset_angle += 180;
    }
  
    console.log('theta=' + offset_angle);
    console.log('distance=' + distance);
    returnString = offset_angle+"&"+distance;
    
    Pebble.sendAppMessage({
      'angle': offset_angle
    }, function(err) {
      console.log("Success");
    }, function(err) {
      console.log("failure");
    });
  }
}

function saveDestinationToFile()
{
  localStorage.setItem(0, lat_des);
  localStorage.setItem(1, lon_des);
}

function loadDestinationFromFile()
{
  lat_des = localStorage.getItem(0);
  lon_des = localStorage.getItem(1);
}

function clearDestination()
{
  lat_prv = 0;
  lat_cur = 0;
  lat_des = 0;
  lon_prv = 0;
  lon_cur = 0;
  lon_des = 0;
  offset_angle = 0;
  saveDestinationToFile();
  console.log("Cleared location info.");
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

Pebble.addEventListener('appmessage',
  function(e) {
    var strf = JSON.stringify(e.payload);
    var value = strf.substring(strf.indexOf(":")+2,strf.length-2);
    console.log(value);
   
    if(value == "CLEAR") clearDestination();
    else if(value == "SAVE") saveCurrentLocation = true;
    else if(value == "STARTNAV") navigating = true;
    else if(value == "STARTUP") loadDestinationFromFile();
  }
);
