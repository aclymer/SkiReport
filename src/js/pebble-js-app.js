/* global window: false */
/* global locationWatcher: true */
var apiKey = "4a556d7e765cbb79f42ea242f4dc2";
var d = new Date();
var hr = d.getHours();
var three_hr_index = Math.floor(parseFloat(hr) / 3);
var degree = decodeURI(encodeURI("°"));
var units = [" ","   ", degree + " "];
var retries = 5;
var timeout = 1;
var sendList = [];
var tuplet = {};
var dKey = "details";
tuplet[dKey] = three_hr_index;
sendList.push(tuplet);
tuplet = {};

var locationOptions = {"timeout": 15000, "maximumAge": 60000 }; 

function fetchNearby(latitude, longitude) {
	var req = new XMLHttpRequest();
	req.open('GET',"http://api.worldweatheronline.com/free/v2/search.ashx?q="+latitude+","+longitude+"&format=json&num_of_results=10&wct=Ski&key=" + apiKey, false);
	req.onreadystatechange = function(e) {
		if (req.readyState == 4) {
			if(req.status == 200) {
				localStorage.setItem("nearbyList", req.responseText);
				var result = (JSON.parse(req.responseText));
				//console.log(req.responseText);
				if (result)
				{
					result = result.search_api.result;
					var length = 0;
					length = result.length;
					console.log(length + " locations recieved...");
					var list = [];
					var Keys = ["area_name", "index", "num_of_areas", "details", "loaded"];
					var tuplet = {};
					tuplet[Keys[2]] = length;
					list.push(tuplet);
					tuplet = {};
					
					for (var i = 0; i < length; i++) {
						tuplet = {};
						tuplet[Keys[1]] = i;
						list.push(tuplet);
						tuplet = {};
						tuplet[Keys[0]] = result[i].areaName[0].value;
						list.push(tuplet);
						fetchSkiReport(i);
					}						
					sendList = sendList.concat(list);
				}
			}
			else
			{
				timeout *= 10;
				var err_key = "area_name";
				var err_tuplet = {};
				var err_list = [];
				err_tuplet[err_key] = req.statusText;
				err_list.push(err_tuplet);
				err_tuplet = {};
				err_key = "weather_desc";
				err_tuplet[err_key] = "Retrying in " + timeout/1000 + "s";
				err_list.push(err_tuplet);
				err_tuplet = {};
				sendList = sendList.concat(err_list);
				console.log("!!! " + req.statusText);
				setTimeout(function(e){ fetchNearby(latitude, longitude); }, timeout);
				timeout += timeout;
			}
		}
	};
	req.send();
}

function fetchSkiReport(index) {
	var req = new XMLHttpRequest();
	var locData = JSON.parse(localStorage.nearbyList);
	
	if (locData)
		locData = locData.search_api.result[index];
	else
		locationWatcher = window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	
	if (locData)
	{
		//console.log("Fetching Ski Report for " + locData.areaName[0].value);
		req.open('GET', "http://api.worldweatheronline.com/free/v2/ski.ashx?q="+locData.latitude+","+locData.longitude+"&format=json&includelocation=yes&key=" + apiKey, false);
		req.onreadystatechange = function(e){
			if (req.readyState == 4) {
				if(req.status == 200) {

					//console.log("Storing: " + req.responseText);
					localStorage.setItem(JSON.parse(req.responseText).data.nearest_area[0].areaName[0].value, req.responseText);					
				}
				else
					console.log(req.status + " - " + req.statusText);
			}
		};
		req.send(null);
	}
}

function fetchStoredSkiReport(index) {
	
	var locData = JSON.parse(localStorage.nearbyList);	
	var list = [];	
	//console.log("Fetching stored report for " + locData.search_api.result[index].areaName[0].value);
	var result = localStorage.getItem(locData.search_api.result[index].areaName[0].value);
	var skiReport = JSON.parse(result).data.weather[0];

	if (skiReport)
	{
		var weather_desc = skiReport.hourly[three_hr_index].bottom[0].weatherDesc[0].value;
		var minTemp = skiReport.hourly[three_hr_index].top[0].tempF;
		var maxTemp = skiReport.hourly[three_hr_index].bottom[0].tempF;
		if (units[0] === "cm") {
			minTemp = skiReport.hourly[three_hr_index].top[0].tempC;
			maxTemp = skiReport.hourly[three_hr_index].bottom[0].tempC;
		}
		var wind_speed = skiReport.hourly[three_hr_index].bottom[0].windspeedMiles;
		if (units[1] === "kph") {
			wind_speed = skiReport.hourly[three_hr_index].bottom[0].windspeedKmph;
		}
		var wind_dir = skiReport.hourly[three_hr_index].bottom[0].winddir16Point;
		var chance_of_snow = skiReport.chanceofsnow;
		var area_snowfall = skiReport.totalSnowfall_cm;
		if (units[0] === "in") {
			area_snowfall = parseFloat(area_snowfall / 2.54).toFixed(2);
		}
		var test_weather = /snow/g;
		var tuplet = {};
		var Keys = ["weather_desc","area_snowfall","area_temps","area_wind","update_time","index"];

		tuplet[Keys[5]] = index;
		list.push(tuplet);
		tuplet = {};

		tuplet[Keys[4]] = "Updated @ " + three_hr_index * 3 + ":00";
		list.push(tuplet);
		tuplet = {};

		tuplet[Keys[3]] = "Wind: " + wind_dir + " " + wind_speed + " " + units[1];
		list.push(tuplet);
		tuplet = {};

		tuplet[Keys[2]] = "Temp(" + units[2] + "): " + minTemp + "-" + maxTemp;
		list.push(tuplet);
		tuplet = {};

		tuplet[Keys[1]] = "Snow(24h): " + area_snowfall + " " + units[0];
		list.push(tuplet);
		tuplet = {};

		if (test_weather.test(weather_desc) === true) {
			tuplet[Keys[0]] = weather_desc;
		}
		else {
			tuplet[Keys[0]] = "Snow: " + chance_of_snow + "% chance";
		}
		list.push(tuplet);
		tuplet = {};

		sendList = sendList.concat(list);
	}
}

function SendList(ACK)
{
	if (ACK === 1)
	{
		retries = 10;
		sendList.shift();
		timeout *= 0.98;
	}
	else if (ACK === 0)
	{
		console.log("Message Dropped, resending...");
		retries--;
		timeout *= 1.02;
	}
	else
		console.log(ACK + ": Sending...");
		
	
	if (timeout < 1)
		timeout = 1;
	
	//console.log("Timeout: " + timeout + " Retries: " + retries);
	
	if (sendList.length > 0 && retries > 0)
	{
		setTimeout(function(){Pebble.sendAppMessage(sendList[0], messageAck(1), null/*messageAck(0)*/);}, timeout);

		if (retries === 0)
		{
			Pebble.showSimpleNotificationOnPebble("Comm Error!","Check your BT connection.");
		}
	}
	else if (sendList.length === 0 && retries > 0)
	{
		console.log("List empty, finished sending.");
		Pebble.sendAppMessage({"loaded":1});
	}
}

function messageAck(ACK)
{
	if (ACK === 0)
	{
		retries--;
		SendList(0);
	}
	return;
}

function locationSuccess(pos) {
	console.warn("Location Found!");
	//area_lat = pos.coords.latitude;
	//area_long = pos.coords.longitude;
	fetchNearby(pos.coords.latitude, pos.coords.longitude);
}

function locationError(err) {
	console.warn("location error (" + err.code + "): " + err.message);
	
	if (err.code == 3) {
		Pebble.showSimpleNotificationOnPebble("GPS Loc:","Haven't moved!");
	}
	
	else if (err.code == 1) {
		Pebble.showSimpleNotificationOnPebble("GPS Error:", "DENIED");
	}
	
	else {
		Pebble.showSimpleNotificationOnPebble("GPS Error:", "Loc Unavailable");
	}
}

Pebble.addEventListener("ready",
						function(e) {
							if (e.ready)
								locationWatcher = window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
						});

Pebble.addEventListener("appmessage",
						function(e) {
							//console.log("AppMessage recieved...\n" + JSON.stringify(e.payload));
							if (e.payload)
							{
								if (e.payload.index !== undefined)
								{
									//console.log("Index: " + e.payload.index);
									retries = 3;
									SendList(e.payload.index);
								}
								
								if (e.payload.units !== undefined)
								{
									console.log("Units: " + e.payload.units);
									if (e.payload.units === 1 && units[0] !== "cm")
										units = ["cm","kph", degree + "C"];
									else if (e.payload.units === 0 && units[0] !== "in")
										units = ["in","mph", degree + "F"];
									else
										return;
									for (var i = 0; i < JSON.parse(localStorage.getItem("nearbyList")).search_api.result.length; i++)
										fetchStoredSkiReport(i);
										
									SendList(-1);
								}
								
								if (e.payload.details !== undefined)
								{
									three_hr_index = e.payload.details;
									
									for (var j = 0; j < JSON.parse(localStorage.getItem("nearbyList")).search_api.result.length; j++)
										fetchStoredSkiReport(j);
									
									SendList(-1);
								}
							}
						});

Pebble.addEventListener("webviewclosed",
						function(e) {
							//console.log("webview closed");
							//console.log(e.type);
							//console.log(e.response);
						});

