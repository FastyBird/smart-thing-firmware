$(function() {
    var url = new URL('discover', new URL(window.location));

    $.ajax({

        url: url.href,
        type: "GET",

        // Tell jQuery not to process data or worry about content-type
        // You *must* include these options!
        cache: false,
        contentType: false,
        processData: false,

        success: function(data, text) {
            // Success action
        }

    });
});