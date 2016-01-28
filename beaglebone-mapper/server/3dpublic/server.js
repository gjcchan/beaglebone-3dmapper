var app = require('http').createServer(handler);
var fs = require('fs');
var net = require('net');
var io = require('socket.io')(app);
var dgram = require('dgram');
var PORT = 33333;
var TCP_PORT = 8101
var HOST = '127.0.0.1';
var APP_IP = '127.0.0.1'; //"192.168.1.145";
var uptime;
var fdArray = new Array();
app.listen(8100);


function handler (req, res) {

    if (req.url == '/') {
      filePath = '/root/server/3dpublic/index.html';
    } else {
      filePath = '/root/server/3dpublic/' + req.url.substring(1);
    }
    
    //var absPath = './' + filePath;

  fs.readFile(filePath,
  function (err, data) {
    if (err) {
      res.writeHead(500);
      console.log(err);
      return res.end('Error loading index.html');
    }

    res.writeHead(200);
    res.end(data);
  });
}

// Console will print the message
console.log('Server running at http://127.0.0.1:8100/');
var time = Date.now();
io.on('connection', function (socket) {

  socket.on('go', function (data) {
    var client = dgram.createSocket("udp4");
    var message = new Buffer("go");
    console.log("request received...");
    client.send(message, 0, message.length, 12345, APP_IP, function(err) {
      if(err) {
        socket.emit('go', "error: Timed out");  
      }
    });
    client.on('message', function (message, remote) {
      if(message != '\n') {
        socket.emit('go', message.toString());   
      }
    });
  });


});
var server = net.createServer(function(conn) {
    console.log('server connected');

});
