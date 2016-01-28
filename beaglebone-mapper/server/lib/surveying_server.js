/*
 * Respond to commands over a websocket to do math
 */

var fs = require('fs');
var socketio = require('socket.io');
var io;
var dgram = require('dgram');

var PORT = 12345;
var HOST = '127.0.0.1';


exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');
	
	io.sockets.on('connection', function(socket) {

		handleCommand(socket);
	});
	
};

function handleCommand(socket) {
	socket.on('udpCommand', function(data) {
		console.log('udp command: ' + data);
		
		var buffer = new Buffer(data);
		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		    console.log('UDP message sent to ' + HOST +':'+ PORT);
		});
		
		client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
		    
		    client.close();

		});
		client.on("UDP Client: close", function() {
		    console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
		    console.log("error: ",err);
		});
	});
	socket.on('updateTime', function(fileName) {
		var absPath = "/proc/uptime";
		
		fs.exists(absPath, function(exists) {
			if (exists) {
				// Can use 2nd param: 'utf8', 
				fs.readFile(absPath, function(err, fileData) {
					if (err) {
						socket.emit('errorReply', "ERROR: Unable to read file " + absPath);
					} else {
						socket.emit('changeUptime', fileData.toString('utf8'));
					}
				});
			} else {
				socket.emit('errorReply', "ERROR: File " + absPath + " not found.");
			}
		});
	});
	socket.on('udpUpdateDimension', function(data) {
		var buffer = new Buffer("updateDimension");
		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		});
		
		client.on('listening', function () {
		    var address = client.address();
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    
		    var reply = message.toString('utf8');
		    var split = reply.split(" ");
		    socket.emit('dimensionXReply', split[0]);
		    socket.emit('dimensionYReply', split[1]);
		    socket.emit('dimensionZReply', split[2]);
		    client.close();
		});
		client.on("UDP Client: close", function() {
		    console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
		    console.log("error: ",err);
		});
	});
	socket.on('udpUpdateTemperature', function(data) {
		var buffer = new Buffer("updateTemperature");
		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		});
		
		client.on('listening', function () {
		    var address = client.address();
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    
		    var reply = message.toString('utf8');
		    socket.emit('temperatureReply', reply);
		    client.close();
		});
		client.on("UDP Client: close", function() {
		    console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
		    console.log("error: ",err);
		});
	});
};

