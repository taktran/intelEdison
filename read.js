var theThingsAPI = require('thethingsio-api');

var KEY = 'light';

//create Client
var client = theThingsAPI.createClient();

//read latest write
var req1 = client.thingReadLatest(KEY);

//event fired when the response arrives
req1.on('response',function(res){
    console.log('Read Latest\n',res.statusCode, res.payload.toString(),'\n\n');
});
req1.end();
