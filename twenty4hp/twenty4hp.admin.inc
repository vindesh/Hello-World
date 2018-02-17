<?php

/**
 * @file
 * 24hourproject callbacks for configuration page.
 */

/**
 * Callback function for add 24hourproject.
 */
function twenty4hp_configure($form, &$form_state) {

  /*$form['rules_list'] = array(
    '#title' => t("Add Protected Page Relative path and password."),
    '#type' => 'fieldset',
    '#prefix' => '<div id="rules_list">',
    '#suffix' => '</div>',
    '#description' => t('Please enter the relative path and its corresponding
    password. When user opens this url, they will asked to enter password to
    view this page. For example, "node/5", "new-events" etc.'),
  );
  $form['rules_list']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Relative Path'),
    '#description' => t('Enter relative drupal path. For example, "node/5", "new-events" etc.'),
    '#required' => TRUE,
  );
  $form['rules_list']['password'] = array(
    '#type' => 'password_confirm',
    '#size' => 25,
    '#required' => TRUE,
  );
  $form['rules_list']['submit'] = array(
    '#type' => 'submit',
    '#ajax' => array(
      'callback' => 'twenty4hp_configure_submit_callback',
      'wrapper' => 'rules_list',
      'effect' => 'fade',
      'method' => 'replace',
      'progress' => array('type' => 'throbber', 'message' => t('Please wait. The data is being saved.')),
    ),
    '#value' => t('Save'),
  );*/

  $form['rules_list']['pages_table'] = array(
    '#markup' => twenty4hp_get_pages_list(),
  );

  return $form;
}

function twenty4hp_get_pages_list() {
  $view = views_get_view('event_registration');
  $view->set_display('event_registration');
  $view->set_exposed_input((array) $_GET);
  $view->execute();
  //print $view->render();

  return $view->render(); //theme('table' $view->render() );

}


/**
 * Callback to generate list of protected pages.
 */
function twenty4hp_get_pages_list1111() {


$view = views_get_view('event_registration');
$view->execute();
print $view->render();


  $header = array(
    array(
      'data' => t('#'),      
    ),
    array(
      'data' => t('Rid') , 'field' => 'reg.registration_id'     
    ),
    array(
      'data' => t('Type'), 'field' => 'reg.type'
    ),
    array(
      'data' => t('Uid'), 'field' => 'reg.user_uid'
    ),
    array(
      'data' => t('State'), 'field' => 'reg.state'
    ),
    array(
      'data' => t('Date'), 'field' => 'reg.created'
    ),
    array(
      'data' => t('Location'), 'field' => 'part.field_participant_location_regis_value'
    ),
    array(
      'data' => t('Operations'),
    ),
  );


/*
SELECT reg.registration_id as id, reg.type, reg.user_uid, reg.state, reg.created, part.field_participant_location_regis_value FROM nvh_registration reg INNER JOIN nvh_field_data_field_participant_location_regis part ON reg.registration_id = part.entity_id
*/ 
//  $query = db_select('field_revision_field_participant_location_regis', 'reg')->extend('PagerDefault')->extend('TableSort');


  $rows = array();
  $query = db_select('registration', 'reg')->extend('PagerDefault')->extend('TableSort');
  $query->join('field_data_field_participant_location_regis', 'part', 'reg.registration_id = part.entity_id');
  $query->fields('reg', array('registration_id', 'type', 'user_uid', 'state', 'created'))
        ->fields('part', array('field_participant_location_regis_value'))        
        ->limit(20)
        ->orderByHeader($header);
  

    /*    ->addField('part', 'field_participant_location_regis_value')
      ->extend('PagerDefault')
      ->extend('TableSort')
      ->limit(20)
      ->orderByHeader($header);*/



  $result = $query->execute();
//print_r($result);exit;
  $rows = array();
  $count = 1;
  foreach ($result as $data) {
    $rows[] = array(
      'data' =>
      array(
        $count,
        $data->registration_id, 
        $data->type, 
        $data->user_uid, 
        $data->state, 
        format_date($data->created ,'custom','F j, Y'), 
        $data->field_participant_location_regis_value,
        l(t('Edit'), "registration/" . $data->registration_id . "/edit"),
      ),
    );
    $count++;
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    "attributes" => array('width' => "100%"),
    "sticky" => TRUE,
    "caption" => "",
    "colgroups" => array(),
    "empty" => t("No record found!"),
      )
  );

  $output .= theme('pager', array(
    'tags' => array(),
      )
  );

  return $output;
}


/**
 * Ajax submit callback for add protected page form.
 */
function twenty4hp_configure_submit_callback($form, &$form_state) {

  $errors = form_get_errors();

  if (count($errors) < 1) {

    db_insert('field_revision_field_participant_location_regis')->fields(array(
      'password' => sha1(trim(check_plain($form_state['values']['password']))),
      'path' => check_plain($form_state['values']['path']),
    ))->execute();

    drupal_set_message('The protected page settings has been successfully saved.');
    $form['rules_list']['pages_table']['#markup'] = twenty4hp_get_pages_list();
    $form['rules_list']['path']['#value'] = '';
  }

  return $form['rules_list'];
}





/**
 * Callback to generate list of event registration type.
 */
function twenty4hp_get_event_registration_type() {
  $query = db_select('registration_type', 'reg');  
  $query->fields('reg', array('name'));
  $result = $query->execute();
  $rows = array();
  foreach ($result as $data) {     
    $rows[$data->name] = $data->name ; 
  }
  return $rows;
}

/*---------twenty4hp_event_registration_country--------------------------------------------*/

/**
 * Callback function to generate event registration country list.
 */
function twenty4hp_event_registration_country($form, &$form_state) {

  $form['event_reg'] = array(
    '#title' => t("Event registration country list."),
    '#type' => 'fieldset',
    '#prefix' => '<div id="event_reg">',
    '#suffix' => '</div>',
    '#description' => t(''),
  );
  
  $form['event_reg']['type'] = array(
    '#title' => t('Registration type'),
    '#type' => 'select',
    //'#description' => 'Select event registration type.',
    '#options' => array(t('--- Any ---')) + twenty4hp_get_event_registration_type(),
    //'#attributes' => array('class' => array('container-inline')),
    '#prefix' => '<div class="container-inline">',
  );
  $form['event_reg']['submit'] = array(
    '#type' => 'submit',
    '#ajax' => array(
      'callback' => 'twenty4hp_event_registration_country_submit_callback',
      'wrapper' => 'event_reg',
      'effect' => 'fade',
      'method' => 'replace',
      'progress' => array('type' => 'throbber', 'message' => t('Please wait. The data is being loading.')),
       '#suffix' => '</div>',
    ),
    '#value' => t('Apply'),
  );

  $form['event_reg']['country_table'] = array(
    '#markup' => twenty4hp_get_event_registration_country_list(),
  );

  return $form;
}

/**
 * Ajax submit callback for twenty4hp_event_registration_country form.
 */
function twenty4hp_event_registration_country_submit_callback($form, &$form_state) {

  $errors = form_get_errors();

  if (count($errors) < 1) { 
    drupal_set_message('The event registration country has been successfully filtered.');
    $form['event_reg']['country_table']['#markup'] = twenty4hp_get_event_registration_country_list($form_state['values']['type']);
    $form['event_reg']['path']['#value'] = '';
  }

  return $form['event_reg'];
}


/**
 * Callback to generate list of event registration distinct country.
 */
function twenty4hp_get_event_registration_country_list($type = '') {

  //$type = variable_get('twenty4hp_event_registration_type', '');
  
  $header = array(
    array(
      'data' => t('#'),      
    ),
    array(
      'data' => t('Country') , 'field' => 'country'     
    ),
    /*array(
      'data' => t('Count') , 'field' => 'part.count'     
    ),
    array(
      'data' => t('Location'), 'field' => 'part.field_participant_location_regis_value'
    ),*/
  );

  $rows = array();

  /*$query = db_select('field_data_field_participant_location_regis', 'part');
  $query->distinct()->addExpression("SUBSTRING_INDEX(part.field_participant_location_regis_value, ',', -1)", 'country') ; 
  if(!empty($type)) {
    $query->condition('bundle', $type);
  }
  $query->groupBy('part.field_participant_location_regis_value');  
  $result = $query->execute();
  $total_result_count =  $result->rowCount();*/


  //$query = db_select('field_data_field_participant_location_regis', 'part') ->extend('TableSort') ->extend('PagerDefault');
  //$query->fields('part', array('field_participant_location_regis_value'));
  //$query->addExpression('COUNT(part.field_participant_location_regis_value)', 'count') ;
  $query = db_select('field_data_field_participant_location_regis', 'part') ->extend('TableSort') ;
  $query->distinct()
        ->addExpression("SUBSTRING_INDEX(part.field_participant_location_regis_value, ',', -1)", 'country') ; 
  if(!empty($type)) {
    $query->condition('bundle', $type);
  }elseif($type != '0'){
    $query->condition('bundle', variable_get('twenty4hp_event_registration_type', '')); 
  }
  $query->groupBy('part.field_participant_location_regis_value');
  //$query->limit(10)
  $query->orderByHeader($header);  
  $result = $query->execute();
  //print  $result->rowCount();exit();

  $rows = array();
  $count = 1;
  foreach ($result as $data) {
    $rows[] = array(
      'data' =>
      array(
        $count,        
        $data->country, 
        //$data->count, 
        //$data->field_participant_location_regis_value,
      ),
    );
    $count++;
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    "attributes" => array('width' => "100%"),
    "sticky" => TRUE,
    "caption" => "",
    "colgroups" => array(),
    "empty" => t("No record found!"),
      )
  );


  /*$current_page = pager_default_initialize($total_result_count, $per_page=10);
  $output .= theme('pager', array(
    'tags' => array(),
      )
  );*/

  return $output;
}


/*---------twenty4hp_event_registration_city--------------------------------------------*/


/**
 * Callback function for event_registration_city.
 */
function twenty4hp_event_registration_city($form, &$form_state) {

  $form['event_reg'] = array(
    '#title' => t("Event registration city list."),
    '#type' => 'fieldset',
    '#prefix' => '<div id="event_reg">',
    '#suffix' => '</div>',
    '#description' => t(''),
  );
  
  $form['event_reg']['type'] = array(
    '#title' => t('Registration type'),
    '#type' => 'select',
    //'#description' => 'Select event registration type.',
    '#options' => array(t('--- Any ---')) + twenty4hp_get_event_registration_type(),
    //'#attributes' => array('class' => array('container-inline')),
    '#prefix' => '<div class="container-inline">',
  );
  $form['event_reg']['submit'] = array(
    '#type' => 'submit',
    '#ajax' => array(
      'callback' => 'twenty4hp_event_registration_city_submit_callback',
      'wrapper' => 'event_reg',
      'effect' => 'fade',
      'method' => 'replace',
      'progress' => array('type' => 'throbber', 'message' => t('Please wait. The data is being loading.')),
      '#sufix' => '</div>',
    ),
    '#value' => t('Apply'),
  );  

  $form['event_reg']['city_table'] = array(
    '#markup' => twenty4hp_get_event_registration_city_list(),
  );

  return $form;
}

/**
 * Ajax submit callback for twenty4hp_event_registration_city form.
 */
function twenty4hp_event_registration_city_submit_callback($form, &$form_state) {

  $errors = form_get_errors();

  if (count($errors) < 1) { 
    drupal_set_message('The event registration city has been successfully filtered.');
    $form['event_reg']['city_table']['#markup'] = twenty4hp_get_event_registration_city_list($form_state['values']['type']);
    $form['event_reg']['path']['#value'] = '';
  }

  return $form['event_reg'];
}



/**
 * Callback to generate list of event registration distinct city.
 */
function twenty4hp_get_event_registration_city_list($type = '') {

  //$type = variable_get('twenty4hp_event_registration_type', '');

  $header = array(
    array(
      'data' => t('#'),      
    ),
    array(
      'data' => t('City') , 'field' => 'part.city'     
    ),
    array(
      'data' => t('Count') , 'field' => 'part.count'     
    ),
    array(
      'data' => t('Location'), 'field' => 'part.field_participant_location_regis_value'
    ),
  );

  $rows = array();
  //$query = db_select('field_data_field_participant_location_regis', 'part')->extend('PagerDefault')->extend('TableSort'); 
  $query = db_select('field_data_field_participant_location_regis', 'part')->extend('TableSort');  
  $query->fields('part', array('field_participant_location_regis_value'));
  $query->addExpression('COUNT(part.field_participant_location_regis_value)', 'count') ; 
  $query->addExpression("SUBSTRING_INDEX(part.field_participant_location_regis_value, ',', 1)", 'city') ; 
  if(!empty($type)) {
    $query->condition('bundle', $type);
  }elseif($type != '0'){
    $query->condition('bundle', variable_get('twenty4hp_event_registration_type', '')); 
  }
  $query->groupBy('part.field_participant_location_regis_value')      
        //->limit(10)
        ->orderByHeader($header);
  
  $result = $query->execute();
  //print_r($result);exit;
  $rows = array();
  $count = 1;
  foreach ($result as $data) {
    $rows[] = array(
      'data' =>
      array(
        $count,        
        $data->city, 
        $data->count, 
        $data->field_participant_location_regis_value,
      ),
    );
    $count++;
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    "attributes" => array('width' => "100%"),
    "sticky" => TRUE,
    "caption" => "",
    "colgroups" => array(),
    "empty" => t("No record found!"),
      )
  );

  /*$output .= theme('pager', array(
    'tags' => array(),
      )
  );*/

  return $output;
}



/**
 * Callback function for protected pages settings.
 */
function twenty4hp_settings() {
  $form['twenty4hp_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Event Registration Settings'),
    '#description' => t('Configure Event Registration related settings.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['twenty4hp_fieldset']['twenty4hp_event_registration_type'] = array(
    '#type' => 'select',
    '#title' => t('Event Registration Type Setting'),
    '#default_value' => variable_get('twenty4hp_event_registration_type', ''),
    '#options' => array(t('--- Any ---')) + twenty4hp_get_event_registration_type(),
    '#description' => t('Please select the appropriate option for Event Registration Type.'),
  );

  
  //$form['#submit'][] = 'twenty4hp_settings_submit';
  return system_settings_form($form);
}

