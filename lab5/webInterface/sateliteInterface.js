var sat_data_file = 'commandOut';
var sat_command_file = 'commandIn';
var $ = function(n) {return document.getElementById(n);};


window.onload = function() {
    var currentDate = new Date();
    $("date").innerHTML = currentDate.getMonth() + "/" + currentDate.getDay()
                        + "/" + currentDate.getFullYear();
    $("changethrusters").onclick = submitThruster;
    $("displaybtn").onclick = toggleDisplay;
    $("startbtn").onclick = startSat;
    $("stopbtn").onclick = stopSat;
    $("databtn").onclick = requestData;
    $("vechiclebtn").onclick = vechileCommand;
    $("photonbtn").onclick = firePhoton;
    $("torpidobtn").onclick = fireTorpido;
    setInterval(function() {readTextFile(sat_data_file)}, 200);
};




function interpretData(x) {
    var splitData = x.trim().split("\n");
    for (var i = 0; i < splitData.length; i++) {
        var data = splitData[i].split(":");
        data[0] = data[0].replace(/ /g, "").toLowerCase();
        if ($(data[0])) {
            if(data[0] == "response") {
                enableCommands();
            }
            $(data[0]).innerHTML = data[1].trim();
        } else {
            console.log("Failed to Read: " + data[0]);
        }
        //console.log(data[1].trim());
    }
}

function readTextFile(file)
{
    var rawFile = new XMLHttpRequest();
    rawFile.open("GET", file, true);
    rawFile.onreadystatechange = function () {
        if (rawFile.readyState === 4) {
            if (rawFile.status === 200 || rawFile.status == 0) {
                var allText = rawFile.responseText;
                //console.log(allText);
                if (allText.length > 0) {
                    //enableCommands();
                    interpretData(allText);
                }
            }
        }
    };
    rawFile.send(null);
}

function writeTextFile(file, command) {
    var request = new XMLHttpRequest();
    var params = "MODE=POST&File=" + file + "&Command=" + command;
    request.open("POST", "http://192.168.7.2:8081/commandIn", true);
    request.setRequestHeader("Content-type", "text/plain");
    request.onreadystatechange = function() {
        if(request.readyState == 4 && request.status == 200) {
            console.log(request.responseText);
        }
    };
    console.log("Sent Request: " + command);
    request.send(params);
}

function disableCommands() {
    var inputElements = document.getElementsByClassName('command');
    console.log(inputElements);
    console.log(inputElements.length);
    for (var i = 0; i < inputElements.length; i++) {
        inputElements[i].disabled = true;
    }
    setTimeout(enableCommands, 5000);
}

function enableCommands() {
    var inputElements = document.getElementsByClassName('command');
    console.log(inputElements);
    for (var i = 0; i < inputElements.length; i++) {
        inputElements[i].disabled = false;
    }
}

function submitThruster() {
    console.log("submitThruster called");
    disableCommands();
    var thrusterCommand = 0;
    if ($("leftthrust").checked) {
        thrusterCommand += 1;
    }
    if ($("rightthrust").checked) {
        thrusterCommand += 2;
    }
    if ($("upthrust").checked) {
        thrusterCommand += 4;
    }
    if ($("downthrust").checked) {
        thrusterCommand += 8;
    }
    var mag = $("magnitude").value;
    thrusterCommand += mag << 4;
    var dur = $("duration").value;
    thrusterCommand += dur << 8;
    writeTextFile(sat_command_file, "T" + thrusterCommand + "\0");
}

function toggleDisplay() {
    disableCommands();
    writeTextFile(sat_command_file, "D\0");
}

function startSat() {
    disableCommands();
    writeTextFile(sat_command_file, "S\0");
}

function stopSat() {
    disableCommands();
    writeTextFile(sat_command_file, "P\0");
}

function requestData() {
    disableCommands();
    writeTextFile(sat_command_file, "M" + $("requesteddata").value + "\0");
}

function vechileCommand() {
    disableCommands();
    writeTextFile(sat_command_file, "V" + $("vechilecommand").value + "\0");
}

function firePhoton() {
    disableCommands();
    writeTextFile(sat_command_file, "H\0");
}

function fireTorpido() {
    disableCommands();
    writeTextFile(sat_command_file, "O\0");
}
