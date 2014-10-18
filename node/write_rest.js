var groveSensor = require('jsupm_grove');
var theThingsAPI = require('thethingsio-api');

//Initialize GrooveLight on Analog Pin #1
var light = new groveSensor.GroveLight(1);


var KEY = 'light';
var interval = 3000;//ms

//create Client
var client = theThingsAPI.createClient();


//The object to write.
var object = {
    "values":
        [
            {
                "key": KEY,
                "value": "100",
                "units": "lm",
                "type": "temporal"
                }
        ]
}
//write the object

setInterval(function(){
  object.values[0].value = light.value();
  var req3 = client.thingWrite(object);
  req3.on('response',function(res){
      console.log('Write\n',res.statusCode,res.payload.toString() ,'\n\n');
  });
  req3.end();
  console.log("send",object);
},interval);
