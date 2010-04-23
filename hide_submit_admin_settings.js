// $id$
function hs_custom_image_toggle( use_custom_link ) {
    img="#edit-hide-submit-custom-image-link-wrapper";
    if ( use_custom_link ) { 
        $(img).show(); 
    }
    else {
        $(img).hide(); 
    }
}

$(document).ready(function() {
    $("#edit-hide-submit-toggle-custom-image").each(function() {
        hs_custom_image_toggle ($(this).attr("checked"));
        $(this).click(function() {
            hs_custom_image_toggle ( $(this).attr("checked") );
        });
    });
});