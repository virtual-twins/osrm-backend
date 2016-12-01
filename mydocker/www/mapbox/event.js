/**
 * Created by robin on 5/12/16.
 */


function onSubmitClick() {

    var reg = /^\d+$/;
    var value = weight = document.getElementById("distanceInput").value;
    if(!reg.test(value) || value == null) {
        alert("Distance is not valid: " + value);
        return;
    }


    //var reg = /^\d+$/;
    var value2 = threshold = document.getElementById("tresholdInput").value;
    //if(!reg.test(value2) || value2 == null) {
    //    alert("Treshold is not valid: " + value2);
    //    return;
    //}

    query(lat, lng);
}

function onDetailChanged() {
    $('input[name="detail"]:checked').each(function () {
        var checkedValue = $(this).attr("value");
        switch(checkedValue) {
            case "iso":
                iso = true;
                break;
            case "convex":
                convex = true;
                break;
            case "concave":
                concave = true;
                break;
            default:
                break;
        }
    });
    $('input[name="detail"]:not(:checked)').each(function () {
        var checkedValue = $(this).attr("value");
        switch(checkedValue) {
            case "iso":
                iso = false;
                break;
            case "convex":
                convex = false;
                break;
            case "concave":
                concave = false;
                break;
            default:
                break;
        }
    });
}

function onMeasurementChange() {
    switch($('input[name="measurement"]:checked').val()) {
        case "time":
            measurement = "duration";
            weight = document.getElementById("distanceInput").value;
            document.getElementById("distanceInput").value = weight;
	    request ="isochrone";	
            break;
        case "distance":
            measurement = "distance";
            weight = document.getElementById("distanceInput").value;
            document.getElementById("distanceInput").value = weight;
	    request = "isodistance";	 
            break;
        default:
            break;
    }
}

function onProfileChange() {
    switch($('input[name="vehicle"]:checked').val()) {
        case "bicycle":
            port = 5183;
            break;
        case "car":
            port = 5182;
            break;
        default:
            break;
    }
}
