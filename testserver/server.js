const fs = require("fs"),
	url = require("url"),
	path = require("path"),
	sys = require("util"),
	https = require('https'),
	tls = require('tls');

/*
run  server_certs/create_certs.sh to generate custom certificates
*/
var options = {
	key: fs.readFileSync(path.join(__dirname, "..", "server_certs", "privatekey.pem")).toString(),
	cert: fs.readFileSync(path.join(__dirname, "..", "server_certs", "certificate.pem")).toString()
};

// loading server
var server = https.createServer(options, function (req, res) {
	console.log("Request: " + req.url);
	const filePath = path.join(__dirname, "..", "build", req.url,);
	fs.readFile(filePath, function (err, data) {
		if (err) {
			console.error(err);
			res.writeHead(404);
			res.end(JSON.stringify(err));
			return;
		}
		console.log("Sending firmware...");
		res.writeHead(200, {
			"Content-Type": "application/octet-stream"
		});
		res.write(data, "binary");
		res.end();
		console.log("done!");
	});
}).listen(8070);