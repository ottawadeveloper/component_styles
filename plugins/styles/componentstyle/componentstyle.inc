<?php

$plugin = array(
  'componentstyle' => array(
    'title' => t('Component Styles'),
    'description' => t('Provides a flexible Panels styles implementation'),
    'render pane' => 'componentstyle_render_pane',
    'render region' => 'componentstyle_render_region',
    'pane settings form' => 'componentstyle_settings_form_pane',
    'settings form' => 'componentstyle_settings_form_region',
  ),
);

function component_styles_get_callback(&$category, $callback) {
  if (empty($category['_file_included'])) {
    if (!empty($category['include'])) {
      module_load_include($category['include']['ext'],
          $category['include']['module'],
          $category['include']['name']);
    }
    $category['_file_included'] = TRUE;
  }
  if ((!empty($category['callbacks'][$callback])) &&
      function_exists($category['callbacks'][$callback])) {
    return $category['callbacks'][$callback];
  }
  return NULL;
}

function componentstyle_settings_form_pane($settings) {
  return _componentstyle_settings_form($settings, 'pane');
}

function componentstyle_settings_form_region($settings) {
  return _componentstyle_settings_form($settings, 'region');
}

function _componentstyle_settings_form($settings, $type) {
  $form = array();
  $form['advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#access' => user_access('component styles advanced'),
    '#tree' => TRUE,
    '#weight' => 100,
  );
  $categories = component_styles_get_form_categories($type);
  foreach ($categories as $categoryid => $category) {
    $simple_form = component_styles_get_callback($category, 'form_simple');
    if (!empty($simple_form)) {
      if (empty($settings[$categoryid])) {
        $settings[$categoryid] = array();
      }
      $form[$categoryid] = array(
        '#tree' => TRUE,
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => $category['title'],
        '#weight' => $category['weight'],
      );
      if (!empty($category['description'])) {
        $form[$categoryid]['#description'] = $category['description'];
      }
      $form[$categoryid] += $simple_form($settings[$categoryid]);
    }
    $advanced_form = component_styles_get_callback($category, 'form_advanced');
    if (!empty($advanced_form)) {
      if (empty($settings['advanced_settings'][$categoryid])) {
        $settings['advanced_settings'][$categoryid] = array();
      }
      $form['advanced_settings'][$categoryid] = array(
        '#tree' => TRUE,
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => $category['title'],
        '#weight' => $category['weight'],
      );
      if (!empty($category['advanced_description'])) {
        $form['advanced_settings'][$categoryid]['#description'] = $category['advanced_description'];
      }
      $form['advanced_settings'][$categoryid] += $advanced_form($settings['advanced_settings'][$categoryid]);
    }
  }
  return $form;
}

function _component_styles_default_render_array($content) {
  $render = array(
    'cs_render_div' => array(
      '#type' => 'component_styles_widget',
      '#theme' => 'component_styles_widget',
      '#tag' => 'div',
      '#parent_tag' => 'div',
      '#wrapper_tag' => 'div',
      '#content_wrapper' => FALSE,
      '#parent_wrapper' => FALSE,
      '#attach' => array(
        'js' => array(),
        'css' => array(),
      ),
      '#parent_classes' => array(),
      '#parent_ignore_classes' => array(),
      '#classes' => array(),
      '#ignore_classes' => array(),
      '#wrapper_classes' => array(),
      '#wrapper_ignore_classes' => array(),
      'content' => array(),
    ),
  );
  if (!is_array($content)) {
    $render['cs_render_div']['content'] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
  else {
    $render['cs_render_div']['content'] = $content;
  }
  return $render;
}

function _component_styles_apply_settings(&$content, $settings, $type) {
  $categories = component_styles_get_form_categories($type);
  foreach ($categories as $categoryid => $category) {
    $render_settings = array(
      '#cs_render_simple' => TRUE,
      '#cs_render_advanced' => TRUE,
      '#cs_simple_settings' => isset($settings[$categoryid]) ? 
          $settings[$categoryid] : array(),
      '#cs_advanced_settings' => isset($settings['advanced_settings'][$categoryid]) ? 
          $settings['advanced_settings'][$categoryid] : array(),
    ) + $settings;
    
    $preprocess = component_styles_get_callback($category, 'preprocess');
    if (!empty($preprocess)) {
      $preprocess($render_settings, $content);
    }
    
    if ($render_settings['#cs_render_simple']) {
      $simple = component_styles_get_callback($category, 'render_simple');
      if (!empty($simple)) {
        $simple($render_settings['#cs_simple_settings'], $content, $render_settings);
      }
    }
    
    if ($render_settings['#cs_render_advanced']) {
      $advanced = component_styles_get_callback($category, 'render_advanced');
      if (!empty($advanced)) {
        $advanced($render_settings['#cs_advanced_settings'], $content, $render_settings);
      }
    }
    
    $postprocess = component_styles_get_callback($category, 'postprocess');
    if (!empty($postprocess)) {
      $postprocess($render_settings, $content);
    }
  }
}

function theme_componentstyle_render_region($vars) {
  
  $content = '';
  foreach ($vars['panes'] as $pane) {
    if (is_array($pane)) {
      $content .= render($pane);
    }
    else {
      $content .= $pane;
    }
  }
  $rarray = _component_styles_default_render_array($content);
  _component_styles_apply_settings($rarray, $vars['settings'], 'region');
  return render($rarray);
}

function theme_componentstyle_render_pane($vars) {
  $content = _component_styles_default_render_array($vars['content']->content);
  _component_styles_apply_settings($content, $vars['settings'], 'pane');
  return render($content);
}