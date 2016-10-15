/*jslint node:true, vars:true, bitwise:true, unparam:true */
/*jshint unused:true */
 
 
// File handlers.
var fs = require('fs');
var filepath = "/home/root/dekoboko_log.txt";
fs.appendFile(filepath, "\nNEW RUN\n\n", function(err) {
if(err) {
    return console.log(err);
}
//    console.log("The file was saved!");
});
 
// Serial ports on Edison board.
var SerialPort = require("serialport").SerialPort;  
var port = "/dev/ttyMFD1";  
var serialPort = new SerialPort(port, {  
  baudrate: 9600  
}, false);
 
// LED Bar
var LEDBar = require("jsupm_my9221");
var myLEDBar = new LEDBar.MY9221(8,9);
 
// Accelerometer
var digitalAccelerometer = require("jsupm_mma7660");
var myDigitalAccelerometer = new digitalAccelerometer.MMA7660(digitalAccelerometer.MMA7660_I2C_BUS, digitalAccelerometer.MMA7660_DEFAULT_I2C_ADDR);
// place device in standby mode so we can write registers
myDigitalAccelerometer.setModeStandby();
// enable 64 samples per second
myDigitalAccelerometer.setSampleRate(digitalAccelerometer.MMA7660.AUTOSLEEP_64);
// place device into active mode
myDigitalAccelerometer.setModeActive();
 
var x, y, z;
x = digitalAccelerometer.new_intp();
y = digitalAccelerometer.new_intp();
z = digitalAccelerometer.new_intp();
 
var ax, ay, az;
ax = digitalAccelerometer.new_floatp();
ay = digitalAccelerometer.new_floatp();
az = digitalAccelerometer.new_floatp();
 
// Averaging etc
var smoothing = new Array(5);
var smoothing_counter = 0;
 
var avg;
var max;
var min;
 
var outputStr;
 
 
serialPort.open(function (error) {  
  if (error) {  
    console.log('Failed to open: '+error);
  } else {
    serialPort.on('data', function(data) {  
        var dataStr = '' + data;
//        console.log('raw data: ' + dataStr);
        if(dataStr.substring(0, 6).localeCompare("$GPGGA") === 0){
            var rawText = dataStr.substring(18, 27) + ', ' + dataStr.substring(30, 40);
//            console.log(rawText);
            var long = parseInt(rawText.split(", ")[0].slice(0,2)) + parseFloat(rawText.split(", ")[0].slice(2))/60;
            var lat = parseInt(rawText.split(", ")[1].slice(0,3)) + parseFloat(rawText.split(", ")[1].slice(3))/60;
            
            var parsedData = long + ", " + lat + ", " + acceleration_ledbar + "\n";
            var d = new Date();
            var n = d.getTime();
            console.log(n + ", " + parsedData);
            
            fs.appendFile(filepath, parsedData, function(err) {
            if(err) {
                return console.log(err);
            }
//                console.log("The file was saved!");
            });
            
        }
    });
      
    var loopdelay = 1;
    var loop = setInterval(function()
    {
 
        myDigitalAccelerometer.getAcceleration(ax, ay, az);
        outputStr = "Acceleration: x = " + roundNum(digitalAccelerometer.floatp_value(ax), 6) + "g y = " + roundNum(digitalAccelerometer.floatp_value(ay), 6) + "g z = " + roundNum(digitalAccelerometer.floatp_value(az), 6) + "g";
        //console.log(outputStr); // start showing LED strip with just the first one lit
 
        // round acceleration to 2 digits, invert
        acceleration = roundNum(digitalAccelerometer.floatp_value(az),2) * -1;
 
        smoothing[smoothing_counter] = acceleration + 1.0;
 
        smoothing_counter++;
        if (smoothing_counter >= smoothing.length) {
            smoothing_counter = 0;
 
            var sum = 0;
            for( var i = 0; i < smoothing.length; i++ ){
                sum += parseInt( smoothing[i], 10 );
            }
            avg = sum/smoothing.length;
            max = Math.max.apply(Math, smoothing);
            min = Math.min.apply(Math, smoothing);
 
    //        console.log("Average acceleration: " + avg);
      //      console.log("Maximum acceleration: " + max);
        //    console.log("Minimum acceleration: " + min);
          //  console.log("");
        }
 
        // map acceleration from [0..1.5g] to [0..10]
        acceleration_ledbar = roundNum(map(Math.abs(max),0,1.0,0,10),0);
        // console.log(acceleration_ledbar);
        // Visualize on LED bar
        myLEDBar.setBarLevel(acceleration_ledbar || 0, true);
 
    }, loopdelay);  
  }  
});
 
 
 
function show_LED(level, direction)
{
 // If it's less than 10
 // light up the LED now
 // call show_LED again in 50 ms
 if (level <= 10)
 {
  myLEDBar.setBarLevel(level, directionBool);
  setTimeout(show_LED, 50, ++level, directionBool); 
 }
 // Switch LED lighting directions between lighting cycles
 else
  directionBool = !directionBool;
}
 
 
 
 
function roundNum(num, decimalPlaces)
{
 var extraNum = (1 / (Math.pow(10, decimalPlaces) * 1000));
 return (Math.round((num + extraNum) 
  * (Math.pow(10, decimalPlaces))) / Math.pow(10, decimalPlaces));
}
 
 
function map( x,  in_min,  in_max,  out_min,  out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
 
 
// When exiting: clear LED strip lights, clear interval, print exit message
process.on('SIGINT', function()
{
 myLEDBar.setBarLevel(0, true);
 clearInterval(myInterval);
 console.log("Exiting...");
 process.exit(0);
    
    
    
    clearInterval(myInterval);
 
 // clean up memory
 digitalAccelerometer.delete_intp(x);
 digitalAccelerometer.delete_intp(y);
 digitalAccelerometer.delete_intp(z);
 
 digitalAccelerometer.delete_floatp(ax);
 digitalAccelerometer.delete_floatp(ay);
 digitalAccelerometer.delete_floatp(az);
 
 myDigitalAccelerometer.setModeStandby();
 
 console.log("Exiting...");
 process.exit(0);
});