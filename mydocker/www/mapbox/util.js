/**
 * Created by robin on 7/6/16.
 */

function query(lat, lng) {
    clearMap();
    var coord = [];
    var coord2 = [];
    var coord3 = [];
    var myUrl = 'http://docker3.se-labor.de:' + port + '/' + request + '/v1/driving/'
        + lng +',' + lat +'?' + measurement + '=' + weight
        + '&convexhull=true&concavehull=' + concave + '&threshold='
        + threshold;
    $.when($.ajax({
        url: myUrl,
        dataType: 'json',
        async: true,
        success: function(mydata) {
            if(mydata.hasOwnProperty('convexhull')) {
                $.each(mydata['convexhull'], function (index, element) {
                    coord.push([element.lon, element.lat]);
                });
            }
            if(mydata.hasOwnProperty('concavehull')) {
                $.each(mydata['concavehull'], function (index, element) {
                    coord2.push([element.lon, element.lat]);
                });
            }
            if(mydata.hasOwnProperty('isochrone')) {
                $.each(mydata['isochrone'], function (index, element) {
                    coord3.push([[element.p2.lon, element.p2.lat],
                        [element.p1.lon, element.p1.lat]]);
                });
            }

        }
    })).done(function (a1) {
        if(convex) {
            convexhull.geometry.coordinates = [[coord]];
            map.getSource('convexhull').setData(convexhull);
        }
        if(concave) {
            concavehull.geometry.coordinates = [[coord2]];
            map.getSource('concavehull').setData(concavehull);
        }
        if(iso) {
            isochrone.geometry.coordinates = coord3;
            map.getSource('isochrone').setData(isochrone);
        }
        marker.geometry.coordinates = [lng, lat];
        map.getSource('marker').setData(marker);
    });
}

function clearMap() {
    isochrone.geometry.coordinates = [];
    map.getSource('isochrone').setData(isochrone);
    convexhull.geometry.coordinates = [];
    map.getSource('convexhull').setData(convexhull);
    concavehull.geometry.coordinates = [];
    map.getSource('concavehull').setData(concavehull);
}
