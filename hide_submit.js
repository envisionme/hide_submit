// $Id$

// Hide button and siblings
function hide_submit_button(obj, message) {
    $(obj).siblings('input:submit').hide(); 
    $(obj).hide(); 
    $(message).insertAfter(obj);
}

// Disable button and siblings
function disable_submit_button(obj) {
    $(obj).parents("form").submit()
    $(obj).siblings('input:submit').attr("disabled", "disabled");
    $(obj).attr("disabled", "disabled");

    // Workaround for form-comment 
    // inject missing "op"
    $("#edit-hide-submit-comment-fix").attr("name", "op");
    $("#edit-hide-submit-comment-fix").attr("value", $(obj).attr("value"));            
}

$(document).ready(function() {

    var settings = Drupal.settings.hide_submit;

    if (settings.dbg) {
        // For debugging, this addtion to the script will paint included and excluded buttons
        $('input:submit').css({border:'6px red solid'}); 
        $(settings.selector).css({border:'6px green solid'});
    }
    
    // Hide buttons and inject message
    if (settings.mode == 'hide') {

        jQuery("<img>").attr("src",settings.image);
        
        $(settings.selector).click(function() {
            hide_submit_button(this, settings.message);
        })  

        // Submit when ENTER is pressed
        if (settings.keypress) {
            $(settings.selector).keypress(function() {
                $(this).parents("form").submit();
                hide_submit_button(this, settings.message);
            })  
        }
    } 
    else { // mode == 'disable' 
        $(settings.selector).click(function() {
            disable_submit_button(this);
        });
        
        // Submit when ENTER is pressed
        if (settings.keypress) {
            $(settings.selector).keypress(function() {
                disable_submit_button(this);
            });
        }
    }
})