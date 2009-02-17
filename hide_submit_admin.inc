<?php
// $Id$

function hide_submit_admin_delete_image(&$form_state, $filename)
{
    $filepath = HIDE_SUBMIT_IMG_DIRECTORY.'/'.$filename;
    
    if (!file_exists(HIDE_SUBMIT_IMG_DIRECTORY.'/'.$filename)) {
       drupal_goto('admin/settings/hide-submit');
    }

    $form['hide_submit_image_file_path'] = array('#type' => 'value', '#value' => $filepath);
    $form['hide_submit_image_file_name'] = array('#type' => 'value', '#value' => $filename);

    $msg = t('Are you sure you want to delete the image file %name', array ('%name' => check_plain($filename)));
    
    return confirm_form($form, $msg, 'admin/settings/hide-submit');
}


function hide_submit_admin_delete_image_submit($form, &$form_state) 
{ 
    $filepath = $form_state['values']['hide_submit_image_file_path'];
    $filename = $form_state['values']['hide_submit_image_file_name'];
    
    if (file_exists(HIDE_SUBMIT_IMG_DIRECTORY) && 
        $filepath &&
        file_check_location($filepath, HIDE_SUBMIT_IMG_DIRECTORY) &&
        file_delete($filepath) ) {
          drupal_set_message( t('%name deleted succesfully.', array('%name' => check_plain($filename))) );
   } else {
       drupal_set_message( t('Cannot delete %name, something went wrong.', array('%name' => check_plain($filename))) );
   }
}  
/**
 * Implementation of admin_settings callback
*/
function hide_submit_admin_settings(&$form_state) {
    
  drupal_add_js(HIDE_SUBMIT_ADMIN_SETTINGS_JS);
  
  $directory = HIDE_SUBMIT_IMG_DIRECTORY;
    
  // Check for a new uploaded image, and use that instead.
  if ($file = file_save_upload('hide_submit_image_upload', array('file_validate_is_image' => array()))) {
     if (file_check_directory( $directory, FILE_CREATE_DIRECTORY, 'hide_submit_image_upload')) {
        if (file_copy($file, $directory, FILE_EXISTS_RENAME)) {
           $_POST['hide_submit_custom_image_link'] = $file->filepath;
           $_POST['hide_submit_toggle_custom_image'] = 0;
        }
     }
  }
    
  // Create the form
  $form = array();
   
  // --- PREVIEW ---

  $form['fieldset_preview'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview of current animation and message settings'),
    '#collapsible' => FALSE,
  );

  $form['fieldset_preview']['preview'] = array(
    '#type' => 'markup',
    '#id' => 'hide-submit-preview-user',
    '#value' => filter_xss_admin( _hide_submit_clean_for_javascript( _hide_submit_get_message( ) ) ),
  );

  // --- LOAD OPTIONS FIELDSET ---

  $form['fieldset_js_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Load options'),
      '#collapsible' => TRUE,
  );

  $form['fieldset_js_settings']['hide_submit_js_load_option'] = array(
      '#type' => 'radios',
      '#title' => t('Javascript load options'),
      '#default_value' => variable_get('hide_submit_js_load_option', DEFAULT_JS_LOAD),
      '#options' => array(
            0 => t('Add/Edit node pages only'), 
            1 => t('Every page'), 
            2 => t('Only on the listed pages.'),
            3 => t('Every page except the listed pages.') ),
      '#description' => t("Add/Edit node view will load for pages with url '/node/add' and '/node/*/edit'.  Every page, is well, EVERY PAGE.  For the last two options use the page list box to set a list of include/exclude urls."),
  );
  
  $form['fieldset_js_settings']['hide_submit_js_load_pages'] = array(
      '#type' => 'textarea',
      '#title' => t('Page list'),
      '#default_value' => variable_get('hide_submit_js_load_pages', ''),
      '#description' => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>')),
  );

  // --- MESSAGE  ---
  
  $form['fieldset_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Message'),
    '#collapsible' => TRUE,
    '#description' => t("The message to print after submit is clicked and the button become invisible. i.e. Please wait.."),
  );

  $form['fieldset_message']['hide_submit_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Default Message'),
    '#rows' => 2,
    '#default_value' => variable_get('hide_submit_message', HIDE_SUBMIT_DEFAULT_MESSAGE ),
  );
  

  $form['fieldset_message']['fieldset_locale'] = array(
    '#type' => 'fieldset',
    '#title' => t('Localization'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $languages = language_list('language', TRUE);
  foreach ($languages as $langcode => $language) {
    if ($language->enabled) {
      $msg_for_language = "hide_submit_message_$langcode";
      $form['fieldset_message']['fieldset_locale'][$msg_for_language] = array(
        '#type' => 'textarea',
        '#rows' => 2,
        '#title' => check_plain($language->native),
        '#default_value' => variable_get($msg_for_language, ""),
      );
    }
  }

  // --- IMAGE SELECTION FIELDSET ---
  
  $form['fieldset_images'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image or animation'),
    '#collapsible' => FALSE,
    '#theme' => 'hide_submit_images_fieldset',
    //'#description' => t("The message to print after submit is clicked and the button become invisible. i.e. Please wait.."),
  );

  // jpg, jpeg, gif, png
  $mask = "^.+\.(([jJ][pP][gG)|([jJ][pP][eE][gG])|([gG][iI][fF])|([pP][nN][gG]))$";
  $default_images = file_scan_directory(HIDE_SUBMIT_MODULE_DIRECTORY, $mask);
  $user_images = file_scan_directory(HIDE_SUBMIT_IMG_DIRECTORY, $mask);

  $options = array();
  foreach( array_merge($default_images, $user_images) as $image_file) {
      //$count = 1;
      $key = md5($image_file->basename);
      $options[$key] = '';
      $list_of_images[$key] = $image_file->filename; 
      
      $form['fieldset_images']['hide_submit_images'][$key]['image'] = array(
        '#type' => 'markup',
        '#title' => $image_file->basename,
        '#value' => theme('image',$image_file->filename),
      );

      /*$form['fieldset_images']['hide_submit_images'][$key]['text'] = array(
        '#type' => 'textarea',
        '#rows' => 2,
        //'#title' => check_plain($language->native),
        //'#default_value' => variable_get($msg_for_language, ""),
      );*/
  }
  
  $form['list_of_images'] = array( 
      '#type' => 'value',
      '#value' => $list_of_images,
  );
  
  
  foreach( $user_images as $image_file) {
      $key = md5($image_file->basename);
      $form['fieldset_images']['hide_submit_images'][$key]['operations'] = array('#value' => l(t('delete'), 'admin/settings/hide-submit/delete/'.$image_file->basename) );
  }
  

  $form['fieldset_images']['hide_submit_random'] = array(
      '#type' => 'checkboxes',
      '#default_value' => variable_get('hide_submit_random', array() ),
      '#options' => $options,
  );

  $form['fieldset_images']['hide_submit_default_image'] = array(
      '#type' => 'radios',
      '#default_value' => variable_get('hide_submit_default_image', md5(basename(HIDE_SUBMIT_DEFAULT_IMAGE)) ),
      '#options' => $options,
  );
  
  $form['fieldset_images']["hide_submit_toggle_custom_image"] = array(
      '#type' => 'checkbox',
      '#title' => t('Use custom link'),
      '#default_value' => variable_get('hide_submit_toggle_custom_image', 0 ),
      '#tree' => FALSE,
      '#description' => t('Check here if you want use a custom image link.')
    );

  $form['fieldset_images']['hide_submit_custom_image_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom image or animation'),
    '#default_value' => variable_get('hide_submit_custom_image_link', HIDE_SUBMIT_DEFAULT_IMAGE ),
    '#description' => t("A link to an image, this image will be added to message.."),
  );

  // Upload picture
  $form['#attributes']['enctype'] = 'multipart/form-data';

  //$picture = variable_get('hide_submit_custom_image_link', HIDE_SUBMIT_DEFAULT_IMAGE ); 

  $form['fieldset_images']['hide_submit_image_upload'] = array(
    '#type' => 'file', 
    '#title' => t('Upload picture'), 
    '#size' => 48, 
    '#description' => t('Upload image or gif animation to display with the message.'),
  );
  
  $form['fieldset_images']["hide_submit_toggle_random"] = array(
      '#type' => 'checkbox',
      '#title' => t('Randomize images'),
      '#default_value' => variable_get('hide_submit_toggle_random', 0 ),
      '#tree' => FALSE,
      '#description' => t('Check here if you want use random image each time the script loads. You need to check the <em>random</em> box for at least two images')
    );

 
  $form = system_settings_form($form); 
  $form['#validate'][] = 'hide_submit_admin_settings_validate'; // add validation  
  $form['#submit'][] = 'hide_submit_admin_settings_submit';     // add submit handler 
  
  return $form;
}
  
/**
 * Implementation of admin_settings callback (expert mode)
*/

function hide_submit_admin_settings_expert(&$form_state, $key = '')
{
    return array( array('#value' => '<div class="messages">Coming soon...</div>') );   
}

function hide_submit_admin_settings_validate($form, &$form_state)
{
    $values = (object) $form_state['values'];
    
    if ($values->hide_submit_toggle_random) {
        $selected_count = 0;
        foreach ($values->hide_submit_random as $key => $v) {
            if ($v) {
                $selected_count++;
            }
        }
        if ($selected_count < 2) {
            form_set_error('hide_submit_toggle_random','Random feature requires at least two selected images '.$selected_count);
        }
    }  
}

function hide_submit_admin_settings_submit($form, &$form_state)
{
    $values = (object) $form_state['values'];
    $op = isset($values->op) ? $values->op : '';

    if ($op == t('Reset to defaults')) {
      variable_del('hide_submit_image');
    }
    // Set custom link
    if ($values->hide_submit_toggle_custom_image) {
        variable_set('hide_submit_image',$values->hide_submit_custom_image_link);
        drupal_set_message("Custom image is set..."); 
    }
    // Randomize
    else if ($values->hide_submit_toggle_random) {
        drupal_set_message("Random image feature is set..."); 
        $images_to_random = array();
        foreach ($values->hide_submit_random as $key => $v) {
            if ($v) {
                $images_to_random[] = $values->list_of_images[$key];
            }
        }
        variable_set('hide_submit_image',$images_to_random);    
    }
    // Use single default image
    else {
        variable_set('hide_submit_image',$values->list_of_images[$values->hide_submit_default_image]);
        drupal_set_message("Default image is set...");    
    }
}


//------------------------------------------------------------------------
//                          THEME FUNCTIONS
//------------------------------------------------------------------------

function theme_hide_submit_images_fieldset($form)
{
  $images = $form['hide_submit_images'];
  foreach (element_children($images) as $key) {

    $filename = $images[$key]['image']['#title'];
    unset($form[$key]['image']['#title']);                 
    
    $row = array();
    $row[] = array( 'data' => drupal_render($form['hide_submit_images'][$key]['image']).'<br />'.$filename, 'class' => 'hide-submit-image');
    $row[] = array( 'data' => drupal_render($form['hide_submit_random'][$key]), 'class' => 'hide-submit-random');
    $row[] = array( 'data' => drupal_render($form['hide_submit_default_image'][$key]), 'class' => 'hide-submit-radio');
    //$row[] = array( 'data' => drupal_render($form['hide_submit_images'][$key]['text']), 'class' => 'hide-submit-text');
    $row[] = array( 'data' => drupal_render($form['hide_submit_images'][$key]['operations']), 'class' => 'hide-submit-operations');
    
    $rows[] = $row;
  }

  $header = array( 
    t('Image'), 
    t('Random'), 
    t('Default'), 
//    t('Override message'), 
    t('Operations')
  );
  
  $output  = drupal_render($form).theme('table', $header, $rows);
  
  return $output;
}
  