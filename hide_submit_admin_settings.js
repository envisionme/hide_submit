function hs_toggle( use_default ) {
    img="#edit-hide-submit-image-link-wrapper";
    dfl="#hide-submit-preview-default-wrapper";
    user="#hide-submit-preview-user-wrapper";
    
    if ( use_default ) { 
        $(img).hide(); 
        $(user).hide();
        $(dfl).show();
    }
    else {
        $(img).show(); 
        $(user).show();
        $(dfl).hide();
    }
}
$(document).ready(function() {
    $("#edit-hide-submit-toggle-image").each(function() {
        hs_toggle ($(this).attr("checked"));
        $(this).change(function() {
            hs_toggle ( $(this).attr("checked") );
        });
    });
});