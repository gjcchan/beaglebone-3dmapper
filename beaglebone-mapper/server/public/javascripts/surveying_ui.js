"use strict";
// Client-side interactions with the browser.

// Web sockets: automatically establish a socket with the server
var socket = io.connect();

// Make connection to server when web page is fully loaded.
$(document).ready(function() {
	
	window.setInterval(function() {
		socket.emit('updateTime', null);
		socket.emit('udpUpdateTemperature', null);
	}, 1000);
	$('#measureRoom').on('click',function(){
		socket.emit('udpUpdateDimension', '');
	});
	// Handle data coming back from the server
	socket.on('dimensionXReply', function(result) {
		$("#dimensionXId").val(result);
	});
	socket.on('dimensionYReply', function(result) {
		$("#dimensionYId").val(result);
	});
	socket.on('dimensionZReply', function(result) {
		$("#dimensionZId").val(result);
	});
	socket.on('temperatureReply', function(result) {
		$("#temperatureId").val(result);
	});
	socket.on('errorReply', function(result) {
		var msg = 'SERVER ERROR: ' + result;
		$("#error-text").text(msg);
		$("#error-box").show();
		var timer = setTimeout(function(){
			$("#error-box").hide();
			clearTimeout(timer);
		}, 10000);
	});

	socket.on('changeUptime', function(result) {
		$("#uptimeId").html(result);
	});
	
});
