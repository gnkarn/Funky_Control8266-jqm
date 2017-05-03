// myjs-body 
        var urltext = "ws://" + window.location.hostname + ":81";
    document.getElementById('webSocketCell').value = urltext;
    document.getElementById("webSocketCell").style.backgroundColor = "red"; 
    
    WSConnect() ;


    /*//connection.onmessage = function(evt) {
        if (evt.data.substring(0, 1) == "H") {
            document.getElementById('slider1').value = evt.data.substring(evt.data.indexOf("H") + 1, evt.data.indexOf(",S"));
            document.getElementById('slider2-range').value = evt.data.substring(evt.data.indexOf("S") + 1, evt.data.indexOf(",V"));
            document.getElementById('slider3-range').value = evt.data.substring(evt.data.indexOf("V") + 1, evt.data.indexOf(",W"));
            document.getElementById('slider4-range').value = evt.data.substring(evt.data.indexOf("W") + 1, evt.data.length);

            $(document.getElementById('slider1')).slider("refresh");
            $(document.getElementById('slider2-range')).slider("refresh");
            $(document.getElementById('slider3-range')).slider("refresh");
            $(document.getElementById('slider4-range')).slider("refresh");
        } else {
            document.getElementById('msgESP').value = evt.data;
        }

    };
*/