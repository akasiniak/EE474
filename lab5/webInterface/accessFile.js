// This program is taken from the following tutorial.
// https://code.tutsplus.com/tutorials/learning-server-side-javascript-with-nodejs--net-10044

var sys = require ('sys'),
url = require('url'),
http = require('http'),
qs = require('querystring');
var fs = require("fs");
var path = require("path");
http.createServer(function (req, res) {
    if(req.method=='POST') {
            var body='';
            req.on('data', function (data) {
                body +=data;
            });
            req.on('end',function(){
                var POST =  qs.parse(body);
                fs.writeFile(POST.File, POST.Command, function(err) {
                    if (err) {
                        return console.log(err);
                    }
                });
                console.log(POST);
            });
    }
    else if(req.method=='GET') {
        var uri = url.parse(req.url).pathname;
        var filename = path.join(process.cwd(), uri);
        path.exists(filename, function(exists) {
            if(!exists) {
                res.writeHead(404, {"Content-Type": "text/plain"});
                res.write("404 Not Found\n");
                res.end();
                return;
            }
             
            fs.readFile(filename, "binary", function(err, file) {
                if(err) {
                    res.writeHead(500, {"Content-Type": "text/plain"});
                    res.write(err + "\n");
                    res.end();
                    return;
                }
                 
                res.writeHead(200);
                res.write(file, "binary");
                res.end();
            });
        });
    }
}).listen(8081);
 
sys.puts("Server running at http://localhost:8081/");
